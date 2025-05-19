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

Board::Board(std::size_t romSize, std::size_t sramSize)
        : ROM_SIZE(romSize),
          SRAM_SIZE(sramSize),
          cpu(romSize, sramSize)
{}

void Board::loadRomFromBinFile(const std::string &path) {
    std::vector<uint8_t> byteRom{};
    cpu.setRom(byteRom);
}

void CPU16::setRom(std::vector<uint8_t> rom) {
    this->rom = std::move(rom);
}

void CPU16::step() {
    //fetch & prelim. decoding
    auto instr = parts::Instruction(fetchInstruction());
    //execute & writeback
    execute(instr);

}
uint64_t CPU16::fetchInstruction() {
    uint64_t instr = 0;
    for (int i = 0; i < 8; i++) {
        instr <<= 8;
        instr |= rom[pc + i];
    }
    pc += 8;
    return instr;
}
void CPU16::execute(parts::Instruction instr) {
    //opcode
    uint16_t op14 = instr.opCode14;
    //immediates
    bool im1 = instr.immFlags == 0x02 || instr.immFlags == 0x03;
    bool im2 = instr.immFlags == 0x01 || instr.immFlags == 0x03;
    //calc vals
    uint16_t src1Val = 0;
    uint16_t src2Val = 0;
    uint16_t destVal = 0;
    uint16_t dest = instr.dest;

    if (im1) {
        src1Val = instr.src1;
    } else if (instr.src1 < NUM_GEN_REGS) {
        src1Val = genRegs[instr.src1].val;
    } else if (instr.src1 < NUM_GEN_REGS + NUM_IO) {
        src1Val = inps[instr.src1 - NUM_GEN_REGS];
    } else if (instr.src1 == 0x0012) {
        src1Val = stackPtr.val;
    } else {
        //error in src1
    }
    if (im2) {
        src2Val = instr.src2;
    } else if (instr.src2 < NUM_GEN_REGS) {
        src2Val = genRegs[instr.src2].val;
    } else if (instr.src2 < NUM_GEN_REGS + NUM_IO) {
        src2Val = inps[instr.src2 - NUM_GEN_REGS];
    } else if (instr.src2 == 0x0012) {
        src2Val = stackPtr.val;
    } else {
        //error in src2
    }
    performOp(op14, src1Val, src2Val, dest);
}
uint16_t CPU16::performOp(uint16_t op14, uint16_t src1Val, uint16_t src2Val, uint16_t dest) {
    bool const isArith = op14 < 0x0010;
    bool const isCond  = op14 < 0x0020;
    bool const isDataTrans  = op14 < 0x0040;
    bool const isCtrlFlow = op14 < 0x0080;
    bool const isVid = op14 < 0x0100;

    uint16_t destVal = 0;
    if (isArith) {
        destVal = doArith(op14, src1Val, src2Val);
        writeback(dest, destVal);
    } else if (isCond) {
        //fn
    }
    else if (isDataTrans) {
        //fn
    } else if (isCtrlFlow) {
        //fn
    } else if (isVid) {
        //fn
    } else {
        //error in operation on SRC1 & SRC2
    }
    return destVal;
}

uint16_t CPU16::doArith(uint16_t op14, uint16_t src1Val, uint16_t src2Val) {
    uint16_t destVal = 0;
    switch (op14) {
        case(isa::Op::ADD):
            destVal = src1Val + src2Val;
            if (destVal > isa::MAX_UINT_16BIT) {
                flagReg.setCarry(true);
                destVal -= isa::MAX_UINT_16BIT;
            }
            break;
        case isa::Op::SUB:
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
        case (isa::Op::MULT):
            destVal = src1Val * src2Val;
            break;
        case (isa::Op::DIV):
            if (src2Val == 0) src2Val = 1; //prevent divide by zero
            destVal = src1Val / src2Val;
            break;
        case (isa::Op::MOD):
            if (src2Val == 0) src2Val = 1; //prevent divide by zero
            destVal = src1Val % src2Val;
            break;
        case (isa::Op::AND):
            destVal = src1Val & src2Val;
            break;
        case (isa::Op::OR):
            destVal = src1Val | src2Val;
            break;
        case (isa::Op::XOR):
            destVal = src1Val ^ src2Val;
            break;
        case (isa::Op::NOT):
            destVal = ~src1Val;
            break;
        case (isa::Op::NAND):
            destVal = ~(src1Val & src2Val);
            break;
        case (isa::Op::NOR):
            destVal = ~(src1Val | src2Val);
            break;
        case (isa::Op::NEG):
            destVal = -src1Val;
            break;
        case (isa::Op::LSH):
            destVal = src1Val << src2Val;
            break;
        case (isa::Op::RSH):
            destVal = src1Val >> src2Val;
            break;
        case (isa::Op::ROL):
            destVal = (src1Val << src2Val) | (src1Val >> (16 - src2Val));
            break;
        case (isa::Op::ROR):
            destVal = (src1Val >> src2Val) | (src1Val << (16 - src2Val));
            break;
        default:
            std::cout << "ERROR: Unknown op14" << std::endl;
            break;
    }
    return destVal;
}

void CPU16::doCond(uint16_t op14, uint16_t src1Val, uint16_t src2Val, uint16_t dest) {
    bool cond = false;
    auto thisOp = static_cast<isa::Op>(op14);
    if (thisOp < isa::Op::Z) {
        switch (thisOp) {
            case (isa::Op::EQ):
                cond = src1Val == src2Val;
                break;
            case (isa::Op::NE):
                cond = src1Val != src2Val;
                break;
            case (isa::Op::LT):
                cond = static_cast<int16_t>(src1Val) < static_cast<int16_t>(src2Val);
                break;
            case (isa::Op::LE):
                cond = static_cast<int16_t>(src1Val) <= static_cast<int16_t>(src2Val);
                break;
            case (isa::Op::GT):
                cond = static_cast<int16_t>(src1Val) > static_cast<int16_t>(src2Val);
            case (isa::Op::GE):
                cond = static_cast<int16_t>(src1Val) >= static_cast<int16_t>(src2Val);
                break;
            case (isa::Op::ULT):
                cond = src1Val < src2Val;
                break;
            case (isa::Op::ULE):
                cond = src1Val <= src2Val;
                break;
            case (isa::Op::UGT):
                cond = src1Val > src2Val;
                break;
            case (isa::Op::UGE):
                cond = src1Val >= src2Val;
                break;
            default:
                std::cout << "ERROR: Unknown op14" << std::endl;
                break;
        }
        if (cond) {
            setPc(dest); //sets program counter to the value in dest (for single op jumps)
        }
    } else {
        switch (thisOp) { //set flags in flagReg manually
            case (isa::Op::Z):
                flagReg.setZero(true);
                break;
            case(isa::Op::NZ):
                flagReg.setZero(false);
                break;
            case (isa::Op::C):
                flagReg.setCarry(true);
                break;
                case (isa::Op::NC):
                flagReg.setCarry(false);
            case(isa::Op::N):
                flagReg.setNegative(true);
                break;
            case (isa::Op::NN):
                flagReg.setNegative(false);
                break;
            default:
                std::cout << "ERROR: Unknown op14" << std::endl;
                break;
        }
    }
}

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
    } else {
        std::cout << "ERROR: Unknown dest" << std::endl;
    }
}
