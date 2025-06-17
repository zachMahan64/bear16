//
// Created by Zachary Mahan on 5/18/25.
//
#include <iostream>
#include "core.h"
#include "assembly.h"
#include <utility>
#include <vector>
#include "isa.h"
#include "parts.h"
#include <cstdint>
#include <thread>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <cstddef>
#include <filesystem>
#include <SDL2/SDL.h>

#ifdef DEBUG_MODE
  #define LOG(x) std::cout << x << std::endl
#else
  #define LOG(x)
#endif

Board::Board(bool enableDebug): isEnableDebug(enableDebug), cpu(sram, rom, enableDebug) {}

//Board and loading ROM stuff -------------------------------------------------------
int Board::run() {
    constexpr int DELAY = 0;
    clock.resetCycles(); //set clock cycles to zero @ the start of a new process
    SDL_Event e;

    constexpr double TARGET_FRAME_TIME = 1.0 / 60.0; // 60 FPS, TODO: link to start of VRAM later
    uint64_t lastFrameTime = SDL_GetPerformanceCounter();
    uint64_t freq = SDL_GetPerformanceFrequency();

    do {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return 0;
        }
        clock.tick(DELAY);
        cpu.step();

        const uint64_t now = SDL_GetPerformanceCounter();
        const double elapsed = static_cast<double>(now - lastFrameTime) / freq;

        if (elapsed >= TARGET_FRAME_TIME) {
            screen.renderSramToFB(sram);
            screen.updateFB();
            lastFrameTime = now;
        }


    } while (cpu.isHalted == false);
    return 0;
}
void Board::printDiagnostics(bool printMemAsChars) const {
    std::cout << std::dec;
    std::cout << "RESULTS\n========" << std::endl;
    printAllRegisterContents();
    std::cout << "=====================" << std::endl;
    std::cout << "First 64 bytes of RAM: \n";
    for (int i = 0; i < 64; i++) {
        std::cout << cpu.sram.at(i);
    }
    std::cout << "\n=====================" << std::endl;
    uint16_t startingAddr = 4096;
    uint16_t numBytes = 20;
    cpu.printSectionOfRam(startingAddr, numBytes, printMemAsChars);
    std::cout << "Total cycles: " << clock.getCycles() << std::endl;
    std::cout << "=====================" << std::endl;
}
void Board::printAllRegisterContents() const {
    int cnt = 0;
    std::string printBuffer{};
    for (const auto& regName : assembly::namedOperandsInOrder) {
        if (regName.at(0) == 'r' && regName != "ra" && regName != "rv") {
            continue;
        }
        std::ostringstream oss;
        oss << std::left << std::setw(5) << (regName + ": ") << std::left << std::setw(7) << std::to_string(cpu.getValInReg(assembly::namedOperandToBinMap.at(regName)));
        printBuffer += oss.str();
        cnt++;
        if (cnt % 4 == 0) {
            std::cout << printBuffer << std::endl;
            printBuffer.clear();
        }
    }
    if (!printBuffer.empty()) {
        std::cout << printBuffer << std::endl;
    }
}
void Board::loadRomFromBinInTxtFile(const std::string &path) {
    std::ifstream file(path);
    std::vector<uint8_t> byteRom{};
    if (!file.is_open()) {
        std::cout << "ERROR: Could not open file" << std::endl;
        return;
    }

    std::string allBits;
    char c;
    while (file.get(c)) {
        if (c == '0' || c == '1') {
            allBits += c;
        }
    }
    std::cout << "All bits: " << allBits << std::endl;

    if (allBits.size() % 8 != 0) {
        std::cout << "ERROR: Bitstream length (" << allBits.size()
                  << ") is not divisible by 8. File must contain errors." << std::endl;
        file.close();
        return;
    }

    int byteNum = 0;
    for (size_t i = 0; i < allBits.size(); i += 8) {
        std::string byteStr = allBits.substr(i, 8);
        auto byte = static_cast<uint8_t>(std::stoi(byteStr, nullptr, 2));
        std::cout << "Byte " << byteNum << ": " << std::to_string(byte) << std::endl;
        byteRom.push_back(byte);
        byteNum++;
    }

    std::cout << "------------ROM loaded------------" << std::endl;
    file.close();
    setRom(byteRom);
}
void Board::loadRomFromHexInTxtFile(const std::string &path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file) {
        std::cout << "ERROR: Could not open file" << std::endl;
        return;
    }
    //read whole file into buffer
    std::string buffer((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

    std::string allDigits;
    allDigits.reserve(buffer.size()); //reserve space

    bool inComment = false;
    for (char c : buffer) {
        if (inComment) {
            if (c == '\n') inComment = false;
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
    std::cout << "All digits (debug): " << allDigits << std::endl;

    if (allDigits.size() % 4 != 0) {
        std::cout << "ERROR: Digit stream length (" << allDigits.size()
                  << ") is not divisible by 4. File must contain errors." << std::endl;
        return;
    }

    std::vector<uint8_t> byteRom;
    byteRom.reserve(allDigits.size() / 2);
    int byteNum = 0;
    for (size_t i = 0; i < allDigits.size(); i += 2) {
        std::string byteStr = allDigits.substr(i, 2);
        auto byte = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
        std::cout << "Byte " << byteNum << ": " << std::to_string(byte) << std::endl;
        byteRom.push_back(byte);
        byteNum++;
    }

    std::cout << "------------ROM loaded------------" << std::endl;
    setRom(byteRom);
}
void Board::loadRomFromByteVector(std::vector<uint8_t>& rom) {
    setRom(rom);
}
void Board::setRom(std::vector<uint8_t>& rom) {
    if (rom.size() > isa::ROM_SIZE) {
        std::cerr << "ERROR: ROM size exceeds " << isa::ROM_SIZE << " bytes" << std::endl;
        return;
    }
    std::ranges::copy(rom, this->rom.begin());
}

CPU16::CPU16(std::array<uint8_t, isa::SRAM_SIZE>& sram, std::array<uint8_t, isa::ROM_SIZE>& rom, bool enableDebug)
    : isEnableDebug(enableDebug), sram(sram), rom(rom)
{}

//CPU ----------------------------------------------------------------------------
uint16_t CPU16::getPc() const {
    return pc;
}

//CPU16 flow of execution
void CPU16::step() {
    //fetch & prelim. decoding
    LOG("DEBUG: PC = " << std::to_string(pc));
    auto instr = parts::Instruction(fetchInstruction());
    //execute & writeback
    execute(instr);
    if (!pcIsFrozenThisCycle) {
        pc += 8;
    }
    pcIsFrozenThisCycle = false;
}
//fetch
uint64_t CPU16::fetchInstruction() const {
    uint64_t instr;
    std::memcpy(&instr, &rom[pc], sizeof(uint64_t));
    return __builtin_bswap64(instr); // cuz big endian
}
//execute
void CPU16::execute(parts::Instruction instr) {
    //immediates
    bool im1 = instr.immFlags == 0x02 || instr.immFlags == 0x03;
    bool im2 = instr.immFlags == 0x01 || instr.immFlags == 0x03;
    //calc vals
    uint16_t src1Val = 0;
    uint16_t src2Val = 0;

    src1Val = im1 ? instr.src1 : getValInReg(instr.src1);
    src2Val = im2 ? instr.src2 : getValInReg(instr.src2);
    performOp(instr, src1Val, src2Val);
}
//carry out execution and writeback (when applicable)
void CPU16::performOp(const parts::Instruction &instr, uint16_t src1Val, uint16_t src2Val) {
    uint16_t op14 = instr.opCode14;
    uint16_t dest = instr.dest;
    bool const isArith = op14 < 0x0010;
    bool const isCond  = op14 < 0x0020;
    bool const isDataTrans  = op14 < 0x0040;
    bool const isCtrlFlow = op14 < 0x0080;
    bool const isVid = op14 < 0x0100;
    if (isArith) {
        doArith(op14, src1Val, src2Val, dest);
    } else if (isCond) {
        doCond(op14, src1Val, src2Val, dest);
    }
    else if (isDataTrans) {
        doDataTrans(instr, src1Val, src2Val);
    } else if (isCtrlFlow) {
        doCtrlFlow(instr, src1Val, src2Val);
    } else if (isVid) {
        //fn
    } else {
        std::cout << "ERROR: Unknown op14, trace to performOp | op14: " << std::hex << std::setw(4) << std::setfill('0') << op14 << std::endl;
        std::cout << "op14: " << std::to_string(op14) << std::endl;
    }
}
void CPU16::doArith(uint16_t op14, uint16_t src1Val, uint16_t src2Val, uint16_t dest) {
    uint16_t destVal = 0;
    //note flag updates not fully good to go
    auto thisOp = static_cast<isa::Opcode_E>(op14);
    switch (thisOp) {
        case(isa::Opcode_E::ADD): {
            uint32_t result = uint32_t(src1Val) + uint32_t(src2Val);
            destVal = static_cast<uint16_t>(result);
            flagReg.setCarry(result > 0xFFFF);
            flagReg.setZero(destVal == 0);
            flagReg.setNegative((destVal & 0x8000) != 0);
            bool overflow = (~(src1Val ^ src2Val) & (src1Val ^ destVal)) & 0x8000;
            flagReg.setOverflow(overflow != 0);
            break;
        }
        case isa::Opcode_E::SUB: {
            destVal = src1Val - src2Val;
            //set flags
            flagReg.setCarry(src1Val < src2Val); //sets carry if borrow occured
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
            if (src2Val == 0) src2Val = 1; //prevent divide by zero
            destVal = src1Val / src2Val;
            break;
        }
        case (isa::Opcode_E::MOD):{
            if (src2Val == 0) src2Val = 1; //prevent divide by zero
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
            std::cout << "ERROR: Unknown op14" << std::endl;
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
                std::cout << "ERROR: Unknown op14" << std::endl;
                break;
            }
        }
        if (cond) {
            jumpTo(dest);
            LOG("DEBUG: cond true, jumping to " << dest);
        }
    } else {
        switch (thisOp) { //set flags in flagReg manually
            case (isa::Opcode_E::Z):
                flagReg.setZero(true);
                break;
            case(isa::Opcode_E::NZ):
                flagReg.setZero(false);
                break;
            case (isa::Opcode_E::C):
                flagReg.setCarry(true);
                break;
            case (isa::Opcode_E::NC):
                flagReg.setCarry(false);
                break;
            case(isa::Opcode_E::N):
                flagReg.setNegative(true);
                break;
            case (isa::Opcode_E::NN):
                flagReg.setNegative(false);
                break;
            default:
                std::cout << "ERROR: Unknown op14" << std::endl;
                break;
        }
    }
}
void CPU16::doDataTrans(parts::Instruction instr, uint16_t src1Val, uint16_t src2Val) {
    uint16_t op14 = instr.opCode14;
    uint16_t dest = instr.dest;
    auto thisOp = static_cast<isa::Opcode_E>(op14);
    switch (thisOp) {
        case(isa::Opcode_E::MOV): {
            writeback(dest, src1Val); //no offset progged in
            break;
        }
        case(isa::Opcode_E::LW): {
            writeback(dest, fetchWordFromRam(src1Val + src2Val));
            break;
        }
        case(isa::Opcode_E::LB): {
            writeback(dest, fetchByteAsWordFromRam(src1Val + src2Val));
            break;
        }
        case(isa::Opcode_E::COMP): {
            uint16_t testVal = src1Val - src2Val;
            flagReg.setCarry(src1Val < src2Val); //sets carry if borrow occurred
            bool src1Neg = (src1Val & 0x8000) != 0;
            bool src2Neg = (src2Val & 0x8000) != 0;
            bool resultNeg = (testVal & 0x8000) != 0;
            bool overflow = (src1Neg != src2Neg) && (src1Neg != resultNeg);
            flagReg.setOverflow(overflow);
            flagReg.setZero(testVal == 0);
            flagReg.setNegative(resultNeg);
            break;
        }
        case(isa::Opcode_E::LEA): {
            writeback(dest, src1Val + src2Val);
            break;
        }
        case(isa::Opcode_E::PUSH): {
            stackPtr.set(stackPtr.val - 2);
            writeWordToRam(stackPtr.val, src1Val);
            LOG("DEBUG: pushing " << static_cast<int>(src1Val) << " to " << stackPtr.val);
            break;
        }
        case(isa::Opcode_E::POP): {
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
            writeback(dest, genRegs[dest].val + 2);
            break;
        }
        case(isa::Opcode_E::MEMCPY): {
            //enter loop
            LOG("DEBUG: MEMCPY");
            if (tickWaitCnt == 0 && tickWaitStopPt == 0) {
                tickWaitStopPt = src2Val;
                isInMemcpyLoop = true;
            }
            //instr loops to copy bytes in order
            if (isInMemcpyLoop) {
                LOG(
                    "DEBUG: MEMCPY LOOP" << std::endl
                    << "tickWaitStopPt: " << tickWaitCnt
                );
                uint16_t startingSrcAddr = src1Val;
                uint16_t startingDestAddr = getValInReg(dest);
                writeByteToRam(startingDestAddr + tickWaitCnt, fetchByteFromRam(startingSrcAddr + tickWaitCnt));
                if (tickWaitCnt == tickWaitStopPt - 1) {
                    tickWaitCnt = 0; //reset
                    tickWaitStopPt = 0; //reset
                    pcIsFrozenThisCycle = false;
                } else {
                    tickWaitCnt++;
                    pcIsFrozenThisCycle = true; //freeze pc
                }
            }
            break;
        }
        case(isa::Opcode_E::SW): {
            writeWordToRam(getValInReg(dest) + src1Val, src2Val);
            break;
        }
        case (isa::Opcode_E::SB): {
            writeByteToRam(getValInReg(dest) + src1Val, static_cast<uint8_t>(src2Val));
            break;
        }
        case(isa::Opcode_E::LWROM): {
            const uint16_t& val = fetchWordFromRom(src1Val + src2Val);
            writeback(dest, val);
            break;
        }
        case(isa::Opcode_E::LBROM): {
            const uint16_t val = fetchByteAsWordFromRom(src1Val + src2Val);
            writeback(dest, val);
            break;
        }
        case(isa::Opcode_E::ROMCPY): {
            //enter loop
            LOG("DEBUG: ROMCPY");
            if (tickWaitCnt == 0 && tickWaitStopPt == 0) {
                tickWaitStopPt = src2Val;
                isInMemcpyLoop = true;
            }
            //instr loops to copy bytes in order
            if (isInMemcpyLoop) {
                LOG(
                     "DEBUG: ROMCPY LOOP" << std::endl <<
                     "tickWaitStopPt: " << tickWaitCnt
                );
                uint16_t startingSrcAddr = src1Val;
                uint16_t startingDestAddr = getValInReg(dest);
                writeByteToRam(startingDestAddr + tickWaitCnt, fetchByteFromRom(startingSrcAddr + tickWaitCnt));
                if (tickWaitCnt == tickWaitStopPt - 1) {
                    tickWaitCnt = 0; //reset
                    tickWaitStopPt = 0; //reset
                    pcIsFrozenThisCycle = false;
                } else {
                    tickWaitCnt++;
                    pcIsFrozenThisCycle = true; //freeze pc
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
            if (src2Val == 0) src2Val = 1; //prevent divide by zero
            const uint16_t val = static_cast<int16_t>(src1Val) / static_cast<int16_t>(src2Val);
            writeback(dest, val);
            break;
        }
        case (isa::Opcode_E::MODS):{
            if (src2Val == 0) src2Val = 1; //prevent divide by zero
            const uint16_t val = static_cast<int16_t>(src1Val) % static_cast<int16_t>(src2Val);
            writeback(dest, val);
            break;
        }
        default: {
            std::cout << "ERROR: Unknown op14" << std::endl;
        }
    }
}
void CPU16::doCtrlFlow(parts::Instruction instr, uint16_t src1Val, uint16_t src2Val) {
    uint16_t op14 = instr.opCode14;
    uint16_t dest = instr.dest;
    const auto thisOp = static_cast<isa::Opcode_E>(op14);
    switch (thisOp) {
        case(isa::Opcode_E::CALL): {
            LOG("DEBUG: CALL" << std::endl);

            // Push return address
            stackPtr -= 2;
            uint16_t returnAddr = pc + 8;
            writeWordToRam(stackPtr.val, returnAddr);            // [SP] = return addr
            LOG(std::dec << "DEBUG: storing return address " <<" at SP = " << stackPtr.val << "\n");

            // Push old FP
            stackPtr -= 2;
            writeWordToRam(stackPtr.val, framePtr.val);  // [SP] = old FP

            // New FP points to old FP
            framePtr = stackPtr;
            // jump to function
            jumpTo(dest);
            break;
        }
        case(isa::Opcode_E::RET): {
            // store the current frame pointer's value (where the old FP and return address are located)
            uint16_t currentFrameBaseAddr = framePtr.val;

            // load the old FP and return address using the current frame's base address
            uint16_t oldFP   = fetchWordFromRam(currentFrameBaseAddr);       // [Current FP] = old FP
            uint16_t retAddr = fetchWordFromRam(currentFrameBaseAddr + 2);   // [Current FP + 2] = return addr

            // restore the frame Pointer to the caller's FP
            framePtr.set(oldFP);

            // restore the Stack Pointer to where it was *before* the CALL instruction.
            stackPtr.set(currentFrameBaseAddr + 4);

            // debug
            LOG(std::dec << "DEBUG: fetched return address = " << retAddr << " from FP+2 = " << static_cast<int>(currentFrameBaseAddr) + 2 << "\n");

            // jump back to the caller
            jumpTo(retAddr);
            LOG(std::dec<< "DEBUG: RET to " << retAddr);
            break;
        }
        case(isa::Opcode_E::JMP): {
            jumpTo(dest);
            break;
        }
        case(isa::Opcode_E::JCOND_Z): {
            if (flagReg.zero) jumpTo(dest);
            break;
        }
        case (isa::Opcode_E::JCOND_NZ): {
            if (!flagReg.zero) jumpTo(dest);
            break;
        }
        case (isa::Opcode_E::JCOND_NEG): {
            if (flagReg.negative) jumpTo(dest);
            break;
        }
        case (isa::Opcode_E::JCOND_NNEG): {
            if (!flagReg.negative) jumpTo(dest);
            break;
        }
        case (isa::Opcode_E::JCOND_POS): {
            if (!flagReg.negative && !flagReg.zero) jumpTo(dest);
            break;
        }
        case (isa::Opcode_E::JCOND_NPOS): {
            if (flagReg.negative || flagReg.zero) jumpTo(dest);
            break;
        }
        case (isa::Opcode_E::NOP): {
            //nada
            break;
        }
        case(isa::Opcode_E::HLT): {
            LOG("debug: HALTED");
            pcIsFrozenThisCycle = true;
            isHalted = true;
            break;
        }
        case(isa::Opcode_E::JAL): {
            writeback(isa::RA_INDEX, pc + 8); //set ra to next instruction
            jumpTo(dest);
            break;
            }
        case(isa::Opcode_E::RETL): {
            jumpTo(genRegs[isa::RA_INDEX].val); //ra = r15
            break;
        }
        default: {
            std::cout << "ERROR: Unknown op14" << std::endl;
        }
    }
}

//gen purpose
void CPU16::writeback(uint16_t dest, uint16_t val) {
    LOG("DEBUG: writeback: " << std::hex << std::setw(4) << std::setfill('0') << dest << " " << val);
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
        std::cout << "ERROR: Unknown dest when writing back: " << std::hex << std::setw(4) << std::setfill('0') << dest << std::endl;
    }
}
uint16_t CPU16::getValInReg(uint16_t reg) const {
    uint16_t regVal = 0;
    if (reg < NUM_GEN_REGS) {
        regVal = genRegs[reg].val;
    } else if (reg < NUM_GEN_REGS + NUM_IO) {
        regVal = inps[reg - NUM_GEN_REGS];
    } else if (reg == 0x001D) {
        regVal = stackPtr.val;
    } else if (reg == 0x001E) {
        regVal = framePtr.val;
    } else if (reg == 0x001F) {
        regVal = pc;
    } else {
        std::cout << "ERROR: Unknown dest when getValInReg:" << reg << std::endl;
    }
    return regVal;
}
//RAM
inline uint16_t CPU16::fetchByteAsWordFromRam(uint16_t addr) const {
    return static_cast<uint16_t>(sram[addr]);
}
inline uint8_t CPU16::fetchByteFromRam(uint16_t addr) const {
    return sram[addr];
}
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
    LOG(std::dec << "DEBUG: wrote word to addr " << addr << ", val:" << val << std::endl << std::hex);
}
void CPU16::writeByteToRam(uint16_t addr, uint8_t val) {
    sram[addr] = val;
    LOG(std::dec << "DEBUG: wrote byte to addr " << addr << ", val:" << val << std::endl << std::hex);
}
void CPU16::printSectionOfRam(uint16_t& startingAddr, uint16_t& numBytes, bool asChar) const {
    std::cout << "Starting Address: " << startingAddr << " | # bytes read: " << numBytes << std::endl;
    std::cout << "Leading word @ starting addr: " << (int16_t) fetchWordFromRam(startingAddr) << "\n";
    for (uint16_t i = 0; i < numBytes; i++) {
        if (asChar) std::cout << "Index: " << i << " = " << sram.at(startingAddr + i) << "\n";
        else std::cout << "Index: " << i << " = " << static_cast<int>(sram.at(startingAddr + i)) << "\n";
       }
}

//Screen
Screen::Screen() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
    }
    window.reset(SDL_CreateWindow("CPU16 Display",
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  WIDTH * SCALE,
                                  HEIGHT * SCALE,
                                  SDL_WINDOW_SHOWN));
    renderer.reset(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_PRESENTVSYNC));
    texture.reset(SDL_CreateTexture(renderer.get(),
                                    SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_STREAMING,
                                    WIDTH, HEIGHT));
}
Screen::~Screen() {
    SDL_Quit();
}
void Screen::updateFB() {
    uint32_t* fbPtr = framebuffer.data();
    SDL_UpdateTexture(texture.get(), nullptr, fbPtr, WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer.get());
    SDL_RenderCopy(renderer.get(), texture.get(), nullptr, nullptr);
    SDL_RenderPresent(renderer.get());
}
void Screen::renderSramToFB(const std::array<uint8_t, isa::SRAM_SIZE>& sram, const uint16_t fbAddr) {
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


//ROM
inline uint16_t CPU16::fetchByteAsWordFromRom(uint16_t addr) const {
    return static_cast<uint16_t>(rom[addr]);
}
inline uint8_t CPU16::fetchByteFromRom(uint16_t addr) const {
    return rom[addr];
}
inline uint16_t CPU16::fetchWordFromRom(uint16_t addr) const {
    uint16_t word = static_cast<uint16_t>(rom[addr]) | static_cast<uint16_t>(rom[addr + 1] << 8);
    return word;
}
void CPU16::jumpTo(uint16_t destAddrInRom) {
    pc = destAddrInRom;
    pcIsFrozenThisCycle = true;
}
