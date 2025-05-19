//
// Created by Zachary Mahan on 5/18/25.
//

#ifndef CORE_H
#define CORE_H
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

class CPU16 {
    //ISA specs
    const short NUM_GEN_REGS = isa::GEN_REG_COUNT;
    const short NUM_IO = isa::IO_COUNT;
    //CTRL Flow
    std::vector<uint8_t> sram;
    std::vector<uint8_t> rom;
    uint16_t pc = 0;
    parts::Clock clk = parts::Clock();
    //IO
    std::vector<uint16_t> inps  = std::vector<uint16_t>(NUM_IO);
    std::vector<uint16_t> outs  = std::vector<uint16_t>(NUM_IO);
    //REG
    std::vector<parts::GenRegister> genRegs = std::vector<parts::GenRegister>(NUM_GEN_REGS);
    parts::FlagRegister flagReg = parts::FlagRegister();
    parts::GenRegister  stackPtr = parts::GenRegister();
public:
    //Constr
    CPU16(std::size_t romSize, std::size_t sramSize) : sram(sramSize), rom(romSize) {}
    //doStuff
    void setRom(std::vector<uint8_t> rom);
    void step();
    uint64_t fetchInstruction();
    void execute(parts::Instruction instr);
    uint16_t performOp(uint16_t op14, uint16_t src1Val, uint16_t src2Val, uint16_t dest);
    uint16_t doArith(uint16_t op14, uint16_t src1Val, uint16_t src2Val);
    uint16_t doCond(uint16_t op14, uint16_t src1Val, uint16_t src2Val);
    uint16_t doDataTrans(uint16_t op14, uint16_t src1Val, uint16_t src2Val);
    uint16_t doCtrlFlow(uint16_t op14, uint16_t src1Val, uint16_t src2Val);
    uint16_t doVid(uint16_t op14, uint16_t src1Val, uint16_t src2Val);
    void writeback(uint16_t dest, uint16_t val);
};
class Board {
    const std::size_t ROM_SIZE;
    const std::size_t SRAM_SIZE;
    uint64_t input  = 0;
    uint64_t output = 0;
    CPU16 cpu;
public:
    Board(std::size_t romSize, std::size_t sramSize);
    void loadRomFromBinFile(const std::string &path);
};

#endif //CORE_H