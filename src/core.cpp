//
// Created by Zachary Mahan on 5/18/25.
//
#include "core.h"
#include "assembly.h"
#include "fixpt8_8_t.h"
#include "isa.h"
#include "parts.h"
#include "preprocess.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#ifdef _MSC_VER
#include <intrin.h>
#define BSWAP64 _byteswap_uint64
#else
#define BSWAP64 __builtin_bswap64
#endif

// macros
// #define DEBUG_MODE
#ifdef DEBUG_MODE
#define LOG(x) std::cout << x << "\n"
#else
#define LOG(x)
#endif

// #define LOG_ERR(x) std::cerr << x << "\n"; define in preprocess already

// #define ENABLE_KEYBOARD_INTERRUPT
#ifdef ENABLE_KEYBOARD_INTERRUPT
#define HANDLE_KBD_INTERRUPT() interruptController.handleKeyboardInterrupt()
#else
#define HANDLE_KBD_INTERRUPT() ((void)0)
#endif

Board::Board(bool enableDebug)
    : isEnableDebug(enableDebug), cpu(sram, userRom, kernelRom, disk, enableDebug),
      inputController(sram), clock(sram), diskController(sram, disk) {}

// Board and loading ROM stuff
// -------------------------------------------------------
int Board::run() {
    int exitCode = 0;
    // init clock & SDL2 timings
    static constexpr int LOOP_SPEED_HZ =
        200'000; // estimated from SDL2 bottleneck, calibrated for 36 MHz
    static constexpr long long TARGET_CLOCK_SPEED_HZ = 36'000'000; // overall target clock speed
    static constexpr long STEPS_PER_LOOP = TARGET_CLOCK_SPEED_HZ / LOOP_SPEED_HZ;
    SDL_Event event;
    clock.resetCycles(); // set clock cycles to zero @ the start of a new process
    clock.initMemMappedTime();

    auto startTime = currentTimeMillis(); // for calculating clock speed after running ends
    constexpr double TARGET_FRAME_TIME = 1.0 / 60.0; // 60 FPS,
    uint64_t lastFrameTime = SDL_GetPerformanceCounter();
    const uint64_t freq = SDL_GetPerformanceFrequency();

    do {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                exitCode = 130;
                cpu.isHalted = true;
            case SDL_KEYDOWN:
                inputController.handleKeyboardPress(event);
                break;
            case SDL_KEYUP:
                inputController.handleKeyboardRelease(event);
                break;
            default:
                break;
            }
        }
        for (int i = 0; i < STEPS_PER_LOOP; ++i) {
            cpu.step();
            clock.tick();
            if (cpu.isHalted) {
                break;
            }
        }

        diskController.handleDiskOperation();

        const uint64_t now = SDL_GetPerformanceCounter();
        const double elapsed = static_cast<double>(now - lastFrameTime) / static_cast<double>(freq);

        if (elapsed >= TARGET_FRAME_TIME) {
            screen.renderSramToFB(sram);
            screen.updateFB();
            lastFrameTime = now;
            clock.incMemMappedTime(); // updates VM real-time
        }
    } while (!cpu.isHalted);

    auto endTime = currentTimeMillis();
    auto elapsedMillis = static_cast<double>(endTime - startTime);
    calcClockSpeedHz(elapsedMillis);

    return exitCode;
}
void Board::printDiagnostics(bool printMemAsChars) const {
    std::cout << std::dec;
    std::cout << "   DUMPED PROCESS STATE\n";
    std::cout << "==========================" << "\n";
    printAllRegisterContents();
    std::cout << "=====================" << "\n";
    std::cout << "First 64 bytes of RAM: \n";
    for (int i = 0; i < 64; i++) {
        std::cout << sram.at(i);
    }
    std::cout << "\n=====================" << "\n";
    uint16_t startingAddr = 0;
    uint16_t numBytes = 100;
    cpu.printSectionOfRam(startingAddr, numBytes, printMemAsChars);
    // startingAddr = 64000;
    // numBytes = isa::MAX_UINT_16BIT - startingAddr;
    // std::cout << "=====================" << "\n";
    // std::cout << "Bottom of stack: \n";
    // cpu.printSectionOfRam(startingAddr, numBytes, true);
    std::cout << "=====================" << "\n";
    std::cout << "Heap contents (first 512 bytes):" << "\n";
    startingAddr = isa::STARTING_HEAP_PTR_VALUE;
    numBytes = 512;
    cpu.printSectionOfRam(startingAddr, numBytes, true);
    std::cout << std::dec << "Total cycles: " << clock.getCycles() << "\n";
    std::cout << std::dec << "Clock Speed = " << clockSpeedHz << " Hz" << " = "
              << clockSpeedHz / 1'000'000 << " MHz" << "\n";
    std::cout << "=====================" << "\n";
    std::cout << "Disk contents (first 1024 bytes): \n";
    for (int i = 0; i < 1024; i++) {
        std::cout << disk.at(i);
        if (i % 128 == 127) {
            std::cout << "\n";
        }
    }
    std::cout << "=====================" << "\n";
}
void Board::printAllRegisterContents() const {
    int cnt = 0;
    std::string printBuffer{};
    for (const auto& regName : assembly::namedOperandsInOrder) {
        if (regName.at(0) == 'r' && regName != "ra" && regName != "rv") {
            continue;
        }
        std::ostringstream oss;
        oss << std::left << std::setw(5) << (regName + ": ") << std::left << std::setw(7)
            << std::to_string(cpu.getValInReg(assembly::namedOperandToBinMap.at(regName)));
        printBuffer += oss.str();
        cnt++;
        if (cnt % 4 == 0) {
            std::cout << printBuffer << "\n";
            printBuffer.clear();
        }
    }
    if (!printBuffer.empty()) {
        std::cout << printBuffer << "\n";
    }
}

void Board::loadUserRomFromBinInTxtFile(const std::string& path) {
    std::ifstream file(path);
    std::vector<uint8_t> byteRom{};
    std::filesystem::path executableName = std::filesystem::path(path).filename();
    if (!file.is_open()) {
        std::cerr << "[ERROR] could not run executable (" << executableName << " does not exist)"
                  << "\n";
        return;
    }

    std::string allBits;
    char c;
    while (file.get(c)) {
        if (c == '0' || c == '1') {
            allBits += c;
        }
    }
    std::cout << "All bits: " << allBits << "\n";

    if (allBits.size() % 8 != 0) {
        std::cout << "[ERROR] Bitstream length (" << allBits.size()
                  << ") is not divisible by 8. File must contain errors." << "\n";
        file.close();
        return;
    }

    int byteNum = 0;
    for (size_t i = 0; i < allBits.size(); i += 8) {
        std::string byteStr = allBits.substr(i, 8);
        auto byte = static_cast<uint8_t>(std::stoi(byteStr, nullptr, 2));
        std::cout << "Byte " << byteNum << ": " << std::to_string(byte) << "\n";
        byteRom.push_back(byte);
        byteNum++;
    }

    std::cout << "------------ROM loaded------------" << "\n";
    file.close();
    setUserRom(byteRom);
}
void Board::loadUserRomFromHexInTxtFile(const std::string& path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    std::filesystem::path executableName = std::filesystem::path(path).filename();
    if (!file.is_open()) {
        std::cerr << "[ERROR] could not run executable (" << executableName << " does not exist)"
                  << "\n";
        return;
    }
    // read whole file into buffer
    std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    std::string allDigits;
    allDigits.reserve(buffer.size()); // reserve space

    bool inComment = false;
    for (char c : buffer) {
        if (inComment) {
            if (c == '\n') {
                inComment = false;
            }
            continue;
        }
        if (c == '#') {
            inComment = true;
            continue;
        }
        if (std::isxdigit(static_cast<unsigned char>(c))) {
            allDigits += c;
        }
    }
    std::cout << "All digits (debug): " << allDigits << "\n";

    if (allDigits.size() % 4 != 0) {
        std::cout << "[ERROR] Digit stream length (" << allDigits.size()
                  << ") is not divisible by 4. File must contain errors." << "\n";
        return;
    }

    std::vector<uint8_t> byteRom;
    byteRom.reserve(allDigits.size() / 2);
    int byteNum = 0;
    for (size_t i = 0; i < allDigits.size(); i += 2) {
        std::string byteStr = allDigits.substr(i, 2);
        auto byte = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
        std::cout << "Byte " << byteNum << ": " << std::to_string(byte) << "\n";
        byteRom.push_back(byte);
        byteNum++;
    }

    std::cout << "------------ROM loaded------------" << "\n";
    setUserRom(byteRom);
}
void Board::loadUserRomFromByteVector(std::vector<uint8_t>& rom) { setUserRom(rom); }
void Board::setUserRom(std::vector<uint8_t>& rom) {
    if (rom.size() > isa::ROM_SIZE) {
        std::cerr << "[ERROR] ROM size exceeds " << isa::ROM_SIZE << " bytes" << "\n";
        return;
    }
    std::ranges::copy(rom, this->userRom.begin());
}

void Board::loadKernelRomFromByteVector(std::vector<uint8_t>& rom) { setUserRom(rom); }

void Board::loadRomFromBinFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    std::filesystem::path executableName = std::filesystem::path(path).filename();
    if (!file.is_open()) {
        std::cerr << "[ERROR] could not run executable (" << executableName << " does not exist)"
                  << "\n";
        return;
    }

    const std::streamsize size = file.tellg();
    if (size > isa::ROM_SIZE) {
        LOG_ERR("[ERROR] File size exceeds disk capacity.");
        return;
    }
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        LOG_ERR("[ERROR] Failed to read entire file.");
        return;
    }

    // Copy into disk array
    std::ranges::copy(buffer, userRom.begin());
}

void Board::saveRomToBinFile(const std::string& path) const { writeToFile(path, userRom); }

void Board::loadDiskFromBinFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cout << "[ERROR] Could not open file" << "\n";
        return;
    }

    std::streamsize size = file.tellg();
    if (size > isa::DISK_SIZE) {
        LOG_ERR("[ERROR] File size exceeds disk capacity.");
        return;
    }
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        LOG_ERR("[ERROR] Failed to read entire file.");
        return;
    }

    // Copy into disk array
    std::ranges::copy(buffer, disk.begin());
}

void Board::saveDiskToBinFile(const std::string& path) const { writeToFile(path, disk); }

void Board::calcClockSpeedHz(double elapsedMillis) {
    clockSpeedHz = static_cast<double>(clock.getCycles()) / (elapsedMillis / 1000);
}

void Board::setKernelRom(std::vector<uint8_t>& rom) {
    if (rom.size() > isa::ROM_SIZE) {
        std::cerr << "[ERROR] ROM size exceeds " << isa::ROM_SIZE << " bytes" << "\n";
        return;
    }
    std::ranges::copy(rom, this->kernelRom.begin());
}

// CPU
// ----------------------------------------------------------------------------
CPU16::CPU16(std::array<uint8_t, isa::SRAM_SIZE>& sram, std::array<uint8_t, isa::ROM_SIZE>& userRom,
             std::array<uint8_t, isa::ROM_SIZE>& kernelRom, std::vector<uint8_t>& disk,
             bool enableDebug)
    : isEnableDebug(enableDebug), sram(sram), userRom(userRom), kernelRom(kernelRom),
      activeRom(this->userRom), disk(disk) {}

// CPU16 flow of execution
void CPU16::step() {
    // fetch & decode
    LOG("DEBUG: PC = " << pc);
    parts::Instruction instr(fetchInstruction());
    // execute & writeback
    execute(instr);
    if (!pcIsFrozenThisCycle) {
        pc += 8;
    }
    pcIsFrozenThisCycle = false;
}
// Interrupt interface
void CPU16::setActiveRomToUser() { activeRom = this->userRom; }
void CPU16::setActiveRomToKernel() { activeRom = this->kernelRom; }
void CPU16::setTrapReturnAddress(uint16_t trapRetAddr) { this->trapRetAddr.set(trapRetAddr); }

// fetch
uint64_t CPU16::fetchInstruction() const {
    uint64_t instr;
    memcpy(&instr, &activeRom[pc], sizeof(uint64_t));
    return BSWAP64(instr); // cuz big endian
}
// execute
void CPU16::execute(parts::Instruction instr) {
    // immediates
    bool im1 = instr.immFlags == 0x02 || instr.immFlags == 0x03;
    bool im2 = instr.immFlags == 0x01 || instr.immFlags == 0x03;
    // calc vals
    uint16_t src1Val = 0;
    uint16_t src2Val = 0;

    src1Val = im1 ? instr.src1 : getValInReg(instr.src1);
    src2Val = im2 ? instr.src2 : getValInReg(instr.src2);
    performOp(instr, src1Val, src2Val);
}
// carry out execution and writeback (when applicable)
void CPU16::performOp(const parts::Instruction& instr, uint16_t src1Val, uint16_t src2Val) {
    uint16_t op14 = instr.opCode14;
    uint16_t dest = instr.dest;
    bool const isArith = op14 < 0x0010;
    bool const isCond = op14 < 0x0020;
    bool const isDataTrans = op14 < 0x0040;
    bool const isCtrlFlow = op14 < 0x0080;
    if (isArith) {
        doArith(op14, src1Val, src2Val, dest);
    } else if (isCond) {
        doCond(op14, src1Val, src2Val, dest);
    } else if (isDataTrans) {
        doDataTrans(instr, src1Val, src2Val);
    } else if (isCtrlFlow) {
        doCtrlFlow(instr, src1Val, src2Val);
    } else {
        LOG_ERR("[ERROR] Unknown op14, trace to performOp | op14: "
                << std::hex << std::setw(4) << std::setfill('0') << op14 << "\n");
        LOG_ERR("op14: " << std::to_string(op14) << "\n");
    }
}
void CPU16::doArith(uint16_t op14, uint16_t src1Val, uint16_t src2Val, uint16_t dest) {
    uint16_t destVal = 0;
    // note flag updates not fully good to go
    auto thisOp = static_cast<isa::Opcode_E>(op14);
    switch (thisOp) {
    case (isa::Opcode_E::ADD): {
        uint32_t result = uint32_t(src1Val) + uint32_t(src2Val);
        destVal = static_cast<uint16_t>(result);
        flagReg.setCarry(result > 0xFFFF);
        flagReg.setZero(destVal == 0);
        flagReg.setNegative((destVal & 0x8000) != 0);
        auto overflow = static_cast<bool>((~(src1Val ^ src2Val) & (src1Val ^ destVal)) & 0x8000);
        flagReg.setOverflow(overflow);
        break;
    }
    case isa::Opcode_E::SUB: {
        destVal = src1Val - src2Val;
        // set flags
        flagReg.setCarry(src1Val < src2Val); // sets carry if borrow occured
        bool src1Neg = (src1Val & 0x8000) != 0;
        bool src2Neg = (src2Val & 0x8000) != 0;
        bool resultNeg = (destVal & 0x8000) != 0;
        bool overflow = (src1Neg != src2Neg) && (src1Neg != resultNeg);
        flagReg.setOverflow(overflow);
        flagReg.setZero(destVal == 0);
        flagReg.setNegative(resultNeg);
        break;
    }
    case (isa::Opcode_E::MULT): {
        destVal = src1Val * src2Val;
        break;
    }
    case (isa::Opcode_E::DIV): {
        if (src2Val == 0) {
            src2Val = 1; // prevent divide by zero
        }
        destVal = src1Val / src2Val;
        break;
    }
    case (isa::Opcode_E::MOD): {
        if (src2Val == 0) {
            src2Val = 1; // prevent divide by zero
        }
        destVal = src1Val % src2Val;
        break;
    }
    case (isa::Opcode_E::AND): {
        destVal = src1Val & src2Val;
        break;
    }
    case (isa::Opcode_E::OR): {
        destVal = src1Val | src2Val;
        break;
    }
    case (isa::Opcode_E::XOR): {
        destVal = src1Val ^ src2Val;
        break;
    }
    case (isa::Opcode_E::NOT): {
        destVal = ~src1Val;
        break;
    }
    case (isa::Opcode_E::NAND): {
        destVal = ~(src1Val & src2Val);
        break;
    }
    case (isa::Opcode_E::NOR): {
        destVal = ~(src1Val | src2Val);
        break;
    }
    case (isa::Opcode_E::NEG): {
        destVal = -src1Val;
        break;
    }
    case (isa::Opcode_E::LSH): {
        destVal = src1Val << src2Val;
        break;
    }
    case (isa::Opcode_E::RSH): {
        destVal = src1Val >> src2Val;
        break;
    }
    case (isa::Opcode_E::ROL): {
        destVal = (src1Val << src2Val) | (src1Val >> (16 - src2Val));
        break;
    }
    case (isa::Opcode_E::ROR): {
        destVal = (src1Val >> src2Val) | (src1Val << (16 - src2Val));
        break;
    }
    default: {
        LOG_ERR("[ERROR] Unknown op14" << "\n");
        break;
    }
    }
    writeback(dest, destVal);
}
void CPU16::doCond(uint16_t op14, uint16_t src1Val, uint16_t src2Val, uint16_t dest) {
    bool cond = false;
    auto thisOp = static_cast<isa::Opcode_E>(op14);
    if (thisOp < isa::Opcode_E::Z) {
        switch (thisOp) {
        case (isa::Opcode_E::EQ): {
            cond = src1Val == src2Val;
            break;
        }
        case (isa::Opcode_E::NE): {
            cond = src1Val != src2Val;
            break;
        }
        case (isa::Opcode_E::LT): {
            cond = static_cast<int16_t>(src1Val) < static_cast<int16_t>(src2Val);
            break;
        }
        case (isa::Opcode_E::LE): {
            cond = static_cast<int16_t>(src1Val) <= static_cast<int16_t>(src2Val);
            break;
        }
        case (isa::Opcode_E::GT): {
            cond = static_cast<int16_t>(src1Val) > static_cast<int16_t>(src2Val);
            break;
        }
        case (isa::Opcode_E::GE): {
            cond = static_cast<int16_t>(src1Val) >= static_cast<int16_t>(src2Val);
            break;
        }
        case (isa::Opcode_E::ULT): {
            cond = src1Val < src2Val;
            break;
        }
        case (isa::Opcode_E::ULE): {
            cond = src1Val <= src2Val;
            break;
        }
        case (isa::Opcode_E::UGT): {
            cond = src1Val > src2Val;
            break;
        }
        case (isa::Opcode_E::UGE): {
            cond = src1Val >= src2Val;
            break;
        }
        default: {
            LOG_ERR("[ERROR] Unknown op14");
            break;
        }
        }
        if (cond) {
            jumpTo(dest);
            LOG("DEBUG: cond true, jumping to " << dest);
        }
    } else {
        switch (thisOp) { // set flags in flagReg manually
        case (isa::Opcode_E::Z):
            flagReg.setZero(true);
            break;
        case (isa::Opcode_E::NZ):
            flagReg.setZero(false);
            break;
        case (isa::Opcode_E::C):
            flagReg.setCarry(true);
            break;
        case (isa::Opcode_E::NC):
            flagReg.setCarry(false);
            break;
        case (isa::Opcode_E::N):
            flagReg.setNegative(true);
            break;
        case (isa::Opcode_E::NN):
            flagReg.setNegative(false);
            break;
        default:
            LOG_ERR("[ERROR] Unknown op14");
            break;
        }
    }
}
void CPU16::doDataTrans(parts::Instruction instr, uint16_t src1Val, uint16_t src2Val) {
    uint16_t op14 = instr.opCode14;
    uint16_t dest = instr.dest;
    auto thisOp = static_cast<isa::Opcode_E>(op14);
    switch (thisOp) {
    case (isa::Opcode_E::MOV): {
        writeback(dest, src1Val); // no offset progged in
        break;
    }
    case (isa::Opcode_E::LW): {
        writeback(dest, fetchWordFromRam(src1Val + src2Val));
        break;
    }
    case (isa::Opcode_E::LB): {
        writeback(dest, fetchByteAsWordFromRam(src1Val + src2Val));
        break;
    }
    case (isa::Opcode_E::COMP): {
        uint16_t testVal = src1Val - src2Val;
        flagReg.setCarry(src1Val < src2Val); // sets carry if borrow occurred
        bool src1Neg = (src1Val & 0x8000) != 0;
        bool src2Neg = (src2Val & 0x8000) != 0;
        bool resultNeg = (testVal & 0x8000) != 0;
        bool overflow = (src1Neg != src2Neg) && (src1Neg != resultNeg);
        flagReg.setOverflow(overflow);
        flagReg.setZero(testVal == 0);
        flagReg.setNegative(resultNeg);
        break;
    }
    case (isa::Opcode_E::LEA): {
        writeback(dest, src1Val + src2Val);
        break;
    }
    case (isa::Opcode_E::PUSH): {
        stackPtr.set(stackPtr.val - 2);
        writeWordToRam(stackPtr.val, src1Val);
        LOG("DEBUG: pushing " << static_cast<int>(src1Val) << " to " << stackPtr.val);
        break;
    }
    case (isa::Opcode_E::POP): {
        src1Val = fetchWordFromRam(stackPtr.val);
        LOG("DEBUG: popping " << src1Val << " from " << stackPtr.val << " to " << dest);
        stackPtr.set(stackPtr.val + 2);
        writeback(dest, src1Val);
        break;
    }
    case (isa::Opcode_E::CLR): {
        writeback(dest, 0);
        break;
    }
    case (isa::Opcode_E::INC): {
        writeback(dest, genRegs[dest].val + 1);
        break;
    }
    case (isa::Opcode_E::DEC): {
        writeback(dest, genRegs[dest].val - 1);
        break;
    }
    case (isa::Opcode_E::MEMCPY): {
        // enter loop
        LOG("DEBUG: MEMCPY");
        if (tickWaitCnt == 0 && tickWaitStopPt == 0) {
            tickWaitStopPt = src2Val;
            isInMemcpyLoop = true;
        }
        // instr loops to copy bytes in order
        if (isInMemcpyLoop) {
            LOG("DEBUG: MEMCPY LOOP" << "\n" << "tickWaitStopPt: " << tickWaitCnt);
            uint16_t startingSrcAddr = src1Val;
            uint16_t startingDestAddr = getValInReg(dest);
            writeByteToRam(startingDestAddr + tickWaitCnt,
                           fetchByteFromRam(startingSrcAddr + tickWaitCnt));
            if (tickWaitCnt == tickWaitStopPt - 1) {
                tickWaitCnt = 0;    // reset
                tickWaitStopPt = 0; // reset
                pcIsFrozenThisCycle = false;
            } else {
                tickWaitCnt++;
                pcIsFrozenThisCycle = true; // freeze pc
            }
        }
        break;
    }
    case (isa::Opcode_E::SW): {
        writeWordToRam(getValInReg(dest) + src1Val, src2Val);
        break;
    }
    case (isa::Opcode_E::SB): {
        writeByteToRam(getValInReg(dest) + src1Val, static_cast<uint8_t>(src2Val));
        break;
    }
    case (isa::Opcode_E::LWROM): {
        const uint16_t& val = fetchWordFromRom(src1Val + src2Val);
        writeback(dest, val);
        break;
    }
    case (isa::Opcode_E::LBROM): {
        const uint16_t val = fetchByteAsWordFromRom(src1Val + src2Val);
        writeback(dest, val);
        break;
    }
    case (isa::Opcode_E::ROMCPY): {
        // enter loop
        LOG("DEBUG: ROMCPY");
        if (tickWaitCnt == 0 && tickWaitStopPt == 0) {
            tickWaitStopPt = src2Val;
            isInMemcpyLoop = true;
        }
        // instr loops to copy bytes in order
        if (isInMemcpyLoop) {
            LOG("DEBUG: ROMCPY LOOP" << "\n" << "tickWaitStopPt: " << tickWaitCnt);
            uint16_t startingSrcAddr = src1Val;
            uint16_t startingDestAddr = getValInReg(dest);
            writeByteToRam(startingDestAddr + tickWaitCnt,
                           fetchByteFromRom(startingSrcAddr + tickWaitCnt));
            if (tickWaitCnt == tickWaitStopPt - 1) {
                tickWaitCnt = 0;    // reset
                tickWaitStopPt = 0; // reset
                pcIsFrozenThisCycle = false;
            } else {
                tickWaitCnt++;
                pcIsFrozenThisCycle = true; // freeze pc
            }
        }
        break;
    }
    case (isa::Opcode_E::MULTS): {
        const uint16_t val = static_cast<int16_t>(src1Val) * static_cast<int16_t>(src2Val);
        writeback(dest, val);
        break;
    }
    case (isa::Opcode_E::DIVS): {
        if (src2Val == 0) {
            src2Val = isa::MAX_UINT16_T; // prevent divide by zero
        }
        const uint16_t val = static_cast<int16_t>(src1Val) / static_cast<int16_t>(src2Val);
        writeback(dest, val);
        break;
    }
    case (isa::Opcode_E::MODS): {
        if (src2Val == 0) {
            src2Val = isa::MAX_UINT16_T; // prevent divide by zero
        }
        const uint16_t val = static_cast<int16_t>(src1Val) % static_cast<int16_t>(src2Val);
        writeback(dest, val);
        break;
    }
    case (isa::Opcode_E::MULT_FPT): {
        const fixpt8_8_t fact1(src1Val);
        const fixpt8_8_t fact2(src2Val);
        const int32_t temp = static_cast<int32_t>(fact1.val) * fact2.val;
        const auto val = static_cast<uint16_t>(temp >> 8);
        writeback(dest, val);
        break;
    }
    case (isa::Opcode_E::DIV_FPT): {
        if (src2Val == 0) {
            src2Val = isa::MAX_UINT16_T; // prevent divide by zero
        }
        const fixpt8_8_t numer(src1Val);
        const fixpt8_8_t denom(src2Val);
        const int32_t temp = (static_cast<int32_t>(numer.val) << 8) / denom.val;
        const auto val = static_cast<uint16_t>(temp);
        writeback(dest, val);
        break;
    }
    case (isa::Opcode_E::MOD_FPT): {
        const int32_t dividend = static_cast<int16_t>(src1Val); // 8.8 fixed-point
        const int32_t divisor = static_cast<int16_t>(src2Val);  // 8.8 fixed-point

        int32_t quotient = dividend / divisor; // integer division (floor)
        int32_t remainder = dividend - (quotient * divisor);

        // Adjust remainder to be positive mod result
        if ((remainder < 0 && divisor > 0) || (remainder > 0 && divisor < 0)) {
            remainder += divisor;
        }

        const auto val = static_cast<uint16_t>(remainder);
        writeback(dest, val);
        break;
    }

    default: {
        LOG_ERR("[ERROR] Unknown op14");
    }
    }
}
void CPU16::doCtrlFlow(parts::Instruction instr, uint16_t src1Val, uint16_t src2Val) {
    uint16_t op14 = instr.opCode14;
    uint16_t dest = instr.dest;
    const auto thisOp = static_cast<isa::Opcode_E>(op14);
    switch (thisOp) {
    case (isa::Opcode_E::CALL): {
        LOG("DEBUG: CALL" << "\n");

        // Push return address
        stackPtr -= 2;
        uint16_t returnAddr = pc + 8;
        writeWordToRam(stackPtr.val, returnAddr); // [SP] = return addr
        LOG(std::dec << "DEBUG: storing return address "
                     << " at SP = " << stackPtr.val << "\n");

        // Push old FP
        stackPtr -= 2;
        writeWordToRam(stackPtr.val, framePtr.val); // [SP] = old FP

        // New FP points to old FP
        framePtr = stackPtr;
        // jump to function
        jumpTo(src1Val);
        break;
    }
    case (isa::Opcode_E::RET): {
        // store the current frame pointer's value (where the old FP and return
        // address are located)
        uint16_t currentFrameBaseAddr = framePtr.val;

        // load the old FP and return address using the current frame's base
        // address
        uint16_t oldFP = fetchWordFromRam(currentFrameBaseAddr); // [Current FP] = old FP
        uint16_t retAddr =
            fetchWordFromRam(currentFrameBaseAddr + 2); // [Current FP + 2] = return addr

        // restore the frame Pointer to the caller's FP
        framePtr.set(oldFP);

        // restore the Stack Pointer to where it was *before* the CALL
        // instruction.
        stackPtr.set(currentFrameBaseAddr + 4);

        // debug
        LOG(std::dec << "DEBUG: fetched return address = " << retAddr
                     << " from FP+2 = " << static_cast<int>(currentFrameBaseAddr) + 2 << "\n");

        // jump back to the caller
        jumpTo(retAddr);
        LOG(std::dec << "DEBUG: RET to " << retAddr);
        break;
    }
    case (isa::Opcode_E::JMP): {
        jumpTo(src1Val);
        break;
    }
    case (isa::Opcode_E::JCOND_Z): {
        if (flagReg.zero) {
            jumpTo(dest);
        }
        break;
    }
    case (isa::Opcode_E::JCOND_NZ): {
        if (!flagReg.zero) {
            jumpTo(dest);
        }
        break;
    }
    case (isa::Opcode_E::JCOND_NEG): {
        if (flagReg.negative) {
            jumpTo(dest);
        }
        break;
    }
    case (isa::Opcode_E::JCOND_NNEG): {
        if (!flagReg.negative) {
            jumpTo(dest);
        }
        break;
    }
    case (isa::Opcode_E::JCOND_POS): {
        if (!flagReg.negative && !flagReg.zero) {
            jumpTo(dest);
        }
        break;
    }
    case (isa::Opcode_E::JCOND_NPOS): {
        if (flagReg.negative || flagReg.zero) {
            jumpTo(dest);
        }
        break;
    }
    case (isa::Opcode_E::NOP): {
        // nada
        break;
    }
    case (isa::Opcode_E::HLT): {
        LOG("debug: HALTED");
        pcIsFrozenThisCycle = true;
        isHalted = true;
        break;
    }
    case (isa::Opcode_E::JAL): {
        writeback(isa::RA_REGISTER_INDEX, pc + 8); // set ra to next instruction
        jumpTo(dest);
        break;
    }
    case (isa::Opcode_E::RETL): {
        jumpTo(genRegs[isa::RA_REGISTER_INDEX].val); // ra = r15
        break;
    }
    default: {
        LOG_ERR("[ERROR] Unknown op14" << "\n");
    }
    }
}

// gen purpose
void CPU16::writeback(uint16_t dest, uint16_t val) {
    LOG("DEBUG: writeback: " << std::hex << std::setw(4) << std::setfill('0') << dest << " "
                             << val);
    if (dest < NUM_GEN_REGS) {
        genRegs[dest].set(val);
    } else if (dest < NUM_GEN_REGS + NUM_IO) {
        outs[dest - NUM_GEN_REGS] = val;
    } else if (dest == 0x001D) {
        stackPtr.set(val);
    } else if (dest == 0x001E) {
        framePtr.set(val);
    } else if (dest == 0x001F) {
        pc = val;
    } else {
        LOG_ERR("[ERROR] Unknown dest when writing back: " << std::hex << std::setw(4)
                                                           << std::setfill('0') << dest << "\n");
    }
}
uint16_t CPU16::getValInReg(uint16_t reg) const {
    uint16_t regVal = 0;
    if (reg < NUM_GEN_REGS) {
        regVal = genRegs[reg].val;
    } else if (reg == 0x001D) {
        regVal = stackPtr.val;
    } else if (reg == 0x001E) {
        regVal = framePtr.val;
    } else if (reg == 0x001F) {
        regVal = pc;
    } else if (reg < NUM_GEN_REGS + NUM_IO) {
        regVal = inps[reg - NUM_GEN_REGS];
    } else {
        LOG_ERR("[ERROR] Unknown dest when getValInReg:" << reg << "\n");
    }
    return regVal;
}
// RAM
inline uint16_t CPU16::fetchByteAsWordFromRam(uint16_t addr) const {
    return static_cast<uint16_t>(sram[addr]);
}
inline uint8_t CPU16::fetchByteFromRam(uint16_t addr) const { return sram[addr]; }
inline uint16_t CPU16::fetchWordFromRam(uint16_t addr) const {
    uint16_t word = static_cast<uint16_t>(sram[addr]) | static_cast<uint16_t>(sram[addr + 1] << 8);
    return word;
}
inline std::array<uint8_t, 2> CPU16::convertWordToBytePair(uint16_t val) {
    std::array<uint8_t, 2> bytePair{};
    bytePair[0] = static_cast<uint8_t>(val & 0xFF);
    bytePair[1] = static_cast<uint8_t>((val >> 8) & 0xFF);
    return bytePair;
}
inline void CPU16::writeWordToRam(uint16_t addr, uint16_t val) {
    sram[addr] = static_cast<uint8_t>(val & 0xFF);
    sram[addr + 1] = static_cast<uint8_t>((val >> 8) & 0xFF);
    LOG(std::dec << "DEBUG: wrote word to addr " << addr << ", val:" << val << "\n" << std::hex);
}
void CPU16::writeByteToRam(uint16_t addr, uint8_t val) {
    sram[addr] = val;
    LOG(std::dec << "DEBUG: wrote byte to addr " << addr << ", val:" << val << "\n" << std::hex);
}
void CPU16::printSectionOfRam(uint16_t& startingAddr, uint16_t& numBytes, bool asChar) const {
    std::cout << "Starting Address: " << startingAddr << " | # bytes read: " << numBytes << "\n";
    std::cout << "Leading word @ starting addr: " << (int16_t)fetchWordFromRam(startingAddr)
              << "\n";
    for (uint16_t i = 0; i < numBytes; i++) {
        if (asChar) {
            std::cout << sram.at(startingAddr + i);
        } else {
            std::cout << static_cast<int>(sram.at(startingAddr + i));
        }
    }
    std::cout << "\n";
}

// ROM
inline uint16_t CPU16::fetchByteAsWordFromRom(uint16_t addr) const {
    return static_cast<uint16_t>(activeRom[addr]);
}
inline uint8_t CPU16::fetchByteFromRom(uint16_t addr) const { return activeRom[addr]; }
inline uint16_t CPU16::fetchWordFromRom(uint16_t addr) const {
    uint16_t word =
        static_cast<uint16_t>(activeRom[addr]) | static_cast<uint16_t>(activeRom[addr + 1] << 8);
    return word;
}
void CPU16::jumpTo(uint16_t destAddrInRom) {
    pc = destAddrInRom;
    pcIsFrozenThisCycle = true;
}

// Screen
Screen::Screen() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << "\n";
    }
    window.reset(SDL_CreateWindow("Bear16 Display", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  WIDTH * SCALE, HEIGHT * SCALE,
                                  SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE));
    renderer.reset(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_PRESENTVSYNC));
    texture.reset(SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT));
}
Screen::~Screen() { SDL_Quit(); }
void Screen::updateFB() {
    uint32_t* fbPtr = framebuffer.data();
    SDL_UpdateTexture(texture.get(), nullptr, fbPtr, WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer.get());
    SDL_RenderCopy(renderer.get(), texture.get(), nullptr, nullptr);
    SDL_RenderPresent(renderer.get());
}
void Screen::renderSramToFB(const std::array<uint8_t, isa::SRAM_SIZE>& sram,
                            const uint16_t fbAddr) {
    // we have WIDTH * HEIGHT pixels total
    // each byte in SRAM encodes 8 horizontal pixels (MSB first)
    // so number of bytes = total pixels / 8
    constexpr int totalPixels = WIDTH * HEIGHT;
    constexpr int bytesPerFrame = totalPixels / 8;

    for (int byteIdx = 0; byteIdx < bytesPerFrame; ++byteIdx) {
        uint8_t byte = sram[fbAddr + byteIdx];

        int basePixelIndex = byteIdx * 8; // (for this byte in framebuffer)

        // unpack and convert to 32bit pix vals
        framebuffer[basePixelIndex + 0] = (byte & 0x80) ? FB_COLOR : 0x00000000;
        framebuffer[basePixelIndex + 1] = (byte & 0x40) ? FB_COLOR : 0x00000000;
        framebuffer[basePixelIndex + 2] = (byte & 0x20) ? FB_COLOR : 0x00000000;
        framebuffer[basePixelIndex + 3] = (byte & 0x10) ? FB_COLOR : 0x00000000;
        framebuffer[basePixelIndex + 4] = (byte & 0x08) ? FB_COLOR : 0x00000000;
        framebuffer[basePixelIndex + 5] = (byte & 0x04) ? FB_COLOR : 0x00000000;
        framebuffer[basePixelIndex + 6] = (byte & 0x02) ? FB_COLOR : 0x00000000;
        framebuffer[basePixelIndex + 7] = (byte & 0x01) ? FB_COLOR : 0x00000000;
    }
}

InputController::InputController(std::array<uint8_t, isa::SRAM_SIZE>& sramRef) : sram(sramRef) {}

void InputController::handleKeyboardPress(const SDL_Event& event) const {
    // Only process key presses with ASCII range (0-127)
    const int keycode = event.key.keysym.sym;
    if (keycode >= 0 && keycode < 128) {
        const auto charVal = static_cast<uint8_t>(keycode);
        sram[isa::KEY_IO_MEM_LOC] = charVal;
        // std::cout << "DEBUG: key pressed: " << std::to_string(keycode) <<
        // "\n";
        HANDLE_KBD_INTERRUPT();
        return;
    }
    if (keycode == SDLK_LSHIFT || keycode == SDLK_RSHIFT) {
        sram[isa::SHIFT_KEY_IO_MEM_LOC] = 0xFF;
        HANDLE_KBD_INTERRUPT();
        return;
    }
    switch (keycode) {
    case SDLK_UP:
        sram[isa::ARROW_KEY_STATE] |= ARROW_UP_MASK;
        break;
    case SDLK_DOWN:
        sram[isa::ARROW_KEY_STATE] |= ARROW_DOWN_MASK;
        break;
    case SDLK_LEFT:
        sram[isa::ARROW_KEY_STATE] |= ARROW_LEFT_MASK;
        break;
    case SDLK_RIGHT:
        sram[isa::ARROW_KEY_STATE] |= ARROW_RIGHT_MASK;
        break;
    default:
        break;
    }
}
void InputController::handleKeyboardRelease(const SDL_Event& event) const {
    int keycode = event.key.keysym.sym;
    if (keycode == SDLK_LSHIFT || keycode == SDLK_RSHIFT) {
        sram[isa::SHIFT_KEY_IO_MEM_LOC] = 0x00;
        HANDLE_KBD_INTERRUPT();
        return;
    }
    switch (keycode) {
    case SDLK_UP:
        sram[isa::ARROW_KEY_STATE] &= ~ARROW_UP_MASK;
        break;
    case SDLK_DOWN:
        sram[isa::ARROW_KEY_STATE] &= ~ARROW_DOWN_MASK;
        break;
    case SDLK_LEFT:
        sram[isa::ARROW_KEY_STATE] &= ~ARROW_LEFT_MASK;
        break;
    case SDLK_RIGHT:
        sram[isa::ARROW_KEY_STATE] &= ~ARROW_RIGHT_MASK;
        break;
    default:
        break;
    }
}

// Interrupt Controller
void InterruptController::handleKeyboardInterrupt() {}
// Disk Controller
DiskController::DiskController(std::array<uint8_t, isa::SRAM_SIZE>& sramRef,
                               std::vector<uint8_t>& disk)
    : disk(disk), sram(sramRef) {}

void DiskController::handleDiskOperation() {
    if (sram[isa::DISK_OP] == NO_OP) {
        return;
    }
    addrPtr = static_cast<uint32_t>(sram[isa::DISK_ADDR_LO]) |
              (static_cast<uint32_t>(sram[isa::DISK_ADDR_MID]) << 8) |
              (static_cast<uint32_t>(sram[isa::DISK_ADDR_HI]) << 16);

    if (addrPtr >= isa::DISK_SIZE) {
        LOG_ERR("[ERROR] Disk address out of bounds: " << addrPtr << "\n");
        sram[isa::DISK_STATUS] |= OVERFLOW_ERROR;
        return;
    }
    if (sram[isa::DISK_OP] == READ_BYTE_OP) {
        sram[isa::DISK_DATA] = disk[addrPtr];
        sram[isa::DISK_STATUS] |= READ_DONE;
        // LOG_ERR("read byte into data, val:" << std::dec << static_cast<int>(sram[isa::DISK_DATA])
        //                                     << " @ addr: " << addrPtr); // for debugging, if
        //                                     necessary
        sram[isa::DISK_OP] = NO_OP; // reset/clear operation mem LOC
    } else if (sram[isa::DISK_OP] == WRITE_BYTE_OP) {
        disk[addrPtr] = sram[isa::DISK_DATA];
        sram[isa::DISK_STATUS] |= WRITE_DONE;
        sram[isa::DISK_OP] = NO_OP; // reset/clear operation mem LOC
    } else if (sram[isa::DISK_OP] == READ_WORD_OP) {
        if (addrPtr + 1 >= isa::DISK_SIZE) {
            LOG_ERR("[ERROR] Disk word access out of bounds: " << addrPtr << "\n");
            sram[isa::DISK_STATUS] |= OVERFLOW_ERROR;
            return;
        }
        sram[isa::DISK_DATA] = disk[addrPtr];
        sram[isa::DISK_DATA + 1] = disk[addrPtr + 1]; // complete word
        sram[isa::DISK_STATUS] |= READ_DONE;
        sram[isa::DISK_OP] = NO_OP; // reset/clear operation mem LOC
    } else if (sram[isa::DISK_OP] == WRITE_WORD_OP) {
        if (addrPtr + 1 >= isa::DISK_SIZE) {
            LOG_ERR("[ERROR] Disk word access out of bounds: " << addrPtr << "\n");
            sram[isa::DISK_STATUS] |= OVERFLOW_ERROR;
            return;
        }
        disk[addrPtr] = sram[isa::DISK_DATA];
        disk[addrPtr + 1] = sram[isa::DISK_DATA + 1]; // complete word
        sram[isa::DISK_STATUS] = WRITE_DONE;
        sram[isa::DISK_OP] = NO_OP; // reset/clear operation mem LOC
    } else if (sram[isa::DISK_OP] == RESET_STATUS_OP) {
        sram[isa::DISK_STATUS] = 0; // reset
    } else {
        LOG("[ERROR] Unknown disk operation: " << sram[isa::DISK_OP] << "\n");
        sram[isa::DISK_STATUS] |= UNKNOWN_OP_ERROR;
    }
}

// helper
uint64_t currentTimeMillis() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto millis = duration_cast<milliseconds>(now.time_since_epoch()).count();
    return static_cast<uint64_t>(millis);
}
