//
// Created by Zachary Mahan on 5/18/25.
//
#include <iostream>
#include "core.h"
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


//Board and loading ROM stuff
Board::Board(std::size_t romSize, std::size_t sramSize)
        : ROM_SIZE(romSize),
          SRAM_SIZE(sramSize),
          cpu(romSize, sramSize)
{}
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
    cpu.setRom(byteRom);
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
    cpu.setRom(byteRom);
}
void Board::loadRomFromManualBinVec(std::vector<uint8_t> rom) {
    cpu.setRom(std::move(rom));
}
uint16_t CPU16::getPc() const {
    return pc;
}
void CPU16::setRom(std::vector<uint8_t> rom) {
    this->rom = std::move(rom);
}

//CPU16 inner workings
void Board::run() {
    do {
        cpu.step();
    } while (cpu.cpuIsHalted == false);
} //incomplete, doesn't use clock
void CPU16::step() {
    //fetch & prelim. decoding
    std::cout << "PC: " << std::to_string(pc) << std::endl;
    auto instr = parts::Instruction(fetchInstruction());
    pc += 8;
    //execute & writeback
    execute(instr);
}
//fetch
uint64_t CPU16::fetchInstruction() {
    uint64_t instr = 0;
    for (int i = 0; i < 8; i++) {
        instr <<= 8;
        instr |= rom[pc + i];
    }
    return instr;
}
//execute
void CPU16::execute(parts::Instruction instr) {
    //immediates
    bool im1 = instr.immFlags == 0x02 || instr.immFlags == 0x03;
    bool im2 = instr.immFlags == 0x01 || instr.immFlags == 0x03;
    //calc vals
    uint16_t src1Val = 0;
    uint16_t src2Val = 0;

    if (im1) {
        src1Val = instr.src1;
    } else if (instr.src1 < NUM_GEN_REGS) {
        src1Val = genRegs[instr.src1].val;
    } else if (instr.src1 < NUM_GEN_REGS + NUM_IO) {
        src1Val = inps[instr.src1 - NUM_GEN_REGS];
    } else if (instr.src1 == 0x0012) {
        src1Val = stackPtr.val;
    } else if (instr.src1 == 0x0013) {
        src1Val = framePtr.val;
    } else if (instr.src1 == 0x0013) {
        src1Val = pc;
    } else {
        std::cout << "ERROR: unknown operand (src1)" << instr.src1 << std::endl;
    }
    if (im2) {
        src2Val = instr.src2;
    } else if (instr.src2 < NUM_GEN_REGS) {
        src2Val = genRegs[instr.src2].val;
    } else if (instr.src2 < NUM_GEN_REGS + NUM_IO) {
        src2Val = inps[instr.src2 - NUM_GEN_REGS];
    } else if (instr.src2 == 0x0012) {
        src2Val = stackPtr.val;
    } else if (instr.src1 == 0x0013) {
        src1Val = pc;
    } else {
        std::cout << "ERROR: unknown operand (src2)" << instr.src2 << std::endl;
    }
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
        std::cout << "ERROR: Unknown op14, trace to performOp" << std::endl;
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
            setPc(dest); //sets program counter to the value in dest (for single op jumps)
            std::cout << "DEBUG: cond true, jumping to " << dest << std::endl;
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
            writeback(dest, fetchWordFromMem(src1Val + src2Val));
            break;
        }
        case(isa::Opcode_E::LB): {
            writeback(dest, fetchByteAsWordFromMem(src1Val + src2Val));
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
            writeWordToMem(stackPtr.val, src1Val);
            break;
        }
        case(isa::Opcode_E::POP): {
            src1Val = fetchWordFromMem(stackPtr.val);
            stackPtr.set(stackPtr.val + 2);
            writeback(dest, src1Val);
            break;
        }
        case (isa::Opcode_E::CLR): {
            writeback(src1Val, 0); //use first arg as register to clear
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
            if (tickWaitCnt == 0 && tickWaitStopPt == 0) {
                tickWaitStopPt = src2Val;
                pcIsStopped = true;
            }
            //instr loops to copy bytes in order
            if (pcIsStopped) {
                uint16_t startingSrcAddr = src1Val;
                uint16_t startingDestAddr = dest;
                writeByteToMem(startingDestAddr + tickWaitCnt, fetchByteFromMem(startingSrcAddr + tickWaitCnt));
                if (tickWaitCnt == tickWaitStopPt - 1) {
                    tickWaitCnt = 0; //reset
                    tickWaitStopPt = 0; //reset
                    pcIsStopped = false;
                }
                tickWaitCnt++;
            }
        }
        case(isa::Opcode_E::SW): {
            writeWordToMem(src1Val + dest, src2Val);
            std::cout << "DEBUG: SW " << std::endl;
            break;
        }
        case (isa::Opcode_E::SB): {
            writeByteToMem(src1Val + dest, static_cast<uint8_t>(src2Val));
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
    auto thisOp = static_cast<isa::Opcode_E>(op14);
    switch (thisOp) {
        case(isa::Opcode_E::CALL): {
            //push old frame pointer
            stackPtr.set(stackPtr.val - 2);
            writeWordToMem(stackPtr.val, framePtr.val);

            //push return address
            stackPtr.set(stackPtr.val - 2);
            writeWordToMem(stackPtr.val, pc);

            //set new frame pointer --> points to return addr
            framePtr.set(stackPtr.val);

            //reserve 32 bytes for frame
            stackPtr.set(stackPtr.val - isa::STACK_FRAME_SIZE); //=32

            //jump
            pc = dest;
            break;
        }
        case(isa::Opcode_E::RET): {
            //free local frame
            stackPtr.set(stackPtr.val + isa::STACK_FRAME_SIZE); //=32

            //restore return address
            uint16_t retAddr = fetchWordFromMem(stackPtr.val);
            stackPtr.set(stackPtr.val + 2);

            //restore old frame pointer
            uint16_t oldFP = fetchWordFromMem(stackPtr.val);
            stackPtr.set(stackPtr.val + 2);
            framePtr.set(oldFP);

            //return to caller
            pc = retAddr;
            break;
        }
        case(isa::Opcode_E::JMP): {
            pc = dest;
            break;
        }
        case(isa::Opcode_E::JCOND_Z): {
            if (flagReg.zero) pc = dest;
            break;
        }
        case (isa::Opcode_E::JCOND_NZ): {
            if (!flagReg.zero) pc = dest;
            break;
        }
        case (isa::Opcode_E::JCOND_NEG): {
            if (flagReg.negative) pc = dest;
            break;
        }
        case (isa::Opcode_E::JCOND_NNEG): {
            if (!flagReg.negative) pc = dest;
            break;
        }
        case (isa::Opcode_E::JCOND_POS): {
            if (!flagReg.negative && !flagReg.zero) pc = dest;
            break;
        }
        case (isa::Opcode_E::JCOND_NPOS): {
            if (flagReg.negative || flagReg.zero) pc = dest;
            break;
        }
        case (isa::Opcode_E::NOP): {
            //nada
            break;
        }
        case(isa::Opcode_E::HLT): {
            std::cout << "debug: HALTED" << std::endl;
            pcIsStopped = true;
            cpuIsHalted = true;
            break;
        }
        case(isa::Opcode_E::JAL): {
            writeback(isa::RA_INDEX, pc); //set ra to next instruction
            pc = dest;
            break;
            }
        case(isa::Opcode_E::RETL): {
            pc = genRegs[isa::RA_INDEX].val; //ra = r15
            break;
        }
        default: {
            std::cout << "ERROR: Unknown op14" << std::endl;
        }
    }
}

//gen purpose
void CPU16::setPc(const uint16_t newPc) {
    pc = newPc;
}
void CPU16::writeback(uint16_t dest, uint16_t val) {
    if (dest < NUM_GEN_REGS) {
        genRegs[dest].set(val);
    } else if (dest < NUM_GEN_REGS + NUM_IO) {
        outs[dest - NUM_GEN_REGS] = val;
    } else if (dest == 0x0012) {
        stackPtr.set(val);
    } else if (dest == 0x0013) {
        framePtr.set(val);
    } else {
        std::cout << "ERROR: Unknown dest when writing back" << std::endl;
        std::cout << "ERROR: dest = " << dest << std::endl;
    }
}
uint16_t CPU16::peekInReg(uint16_t reg) const {
    uint16_t regVal = 0;
    if (reg < NUM_GEN_REGS) {
        regVal = genRegs[reg].val;
    } else if (reg < NUM_GEN_REGS + NUM_IO) {
        regVal = inps[reg - NUM_GEN_REGS];
    } else if (reg == 0x0012) {
        regVal = stackPtr.val;
    } else {
        std::cout << "ERROR: Unknown dest when peeking" << std::endl;
    }
    return regVal;
}
inline uint16_t CPU16::fetchByteAsWordFromMem(uint16_t addr) const {
    return static_cast<uint16_t>(sram[addr]);
}
inline uint8_t CPU16::fetchByteFromMem(uint16_t addr) const {
    return sram[addr];
}
inline uint16_t CPU16::fetchWordFromMem(uint16_t addr) const {
    uint16_t word = static_cast<uint16_t>(sram[addr]) | static_cast<uint16_t>(sram[addr + 1] << 8);
    return word;
}
inline std::array<uint8_t, 2> CPU16::convertWordToBytePair(uint16_t val) {
    std::array<uint8_t, 2> bytePair{};
    bytePair[0] = static_cast<uint8_t>(val & 0xFF);
    bytePair[1] = static_cast<uint8_t>((val >> 8) & 0xFF);
    return bytePair;
}
inline void CPU16::writeWordToMem(uint16_t addr, uint16_t val) {
    sram[addr] = static_cast<uint8_t>(val & 0xFF);
    sram[addr + 1] = static_cast<uint8_t>((val >> 8) & 0xFF);
    std::cout << "addr: " << addr << " val:" << val << std::endl;
}
void CPU16::writeByteToMem(uint16_t addr, uint8_t val) {
    sram[addr] = val;
}


