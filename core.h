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
    //MEMORY
    std::vector<uint8_t> sram;
    std::vector<uint8_t> rom;
    //CTRL FLOW (primitive registers)
    uint16_t pc = 0;
    uint16_t tickWaitCnt = 0; //for multicycle operations
    uint16_t tickWaitStopPt = 0;
    bool pcIsStopped = false;
    bool cpuIsHalted = false;
    //CLOCK
    parts::Clock clk = parts::Clock();
    //IO
    std::vector<uint16_t> inps  = std::vector<uint16_t>(NUM_IO);
    std::vector<uint16_t> outs  = std::vector<uint16_t>(NUM_IO);
    //REG
    std::vector<parts::GenRegister> genRegs = std::vector<parts::GenRegister>(NUM_GEN_REGS);
    parts::FlagRegister flagReg = parts::FlagRegister();
    parts::GenRegister  stackPtr = parts::GenRegister(static_cast<uint16_t>(isa::SRAM_SIZE)); //sp stacks at end of RAM for downward growth
    parts::GenRegister  framePtr = parts::GenRegister(static_cast<uint16_t>(isa::SRAM_SIZE));
public:
    //Constr
    CPU16(std::size_t romSize, std::size_t sramSize) : sram(sramSize), rom(romSize) {}
    //doStuff
    [[nodiscard]] uint16_t getPc() const;
    void setRom(std::vector<uint8_t> rom);
    void run();
    void step();
    uint64_t fetchInstruction();
    void execute(parts::Instruction instr);
    void performOp(const parts::Instruction &instr, uint16_t src1Val, uint16_t src2Val);
    void doArith(uint16_t op14, uint16_t src1Val, uint16_t src2Val, uint16_t dest);
    void doCond(uint16_t op14, uint16_t src1Val, uint16_t src2Val, uint16_t dest);
    void setPc(uint16_t newPc);
    void doDataTrans(parts::Instruction instr, uint16_t src1Val, uint16_t src2Val);
    void doCtrlFlow(parts::Instruction instr, uint16_t src1Val, uint16_t src2Val);
    void doVid(uint16_t op14, uint16_t src1Val, uint16_t src2Val);
    void writeback(uint16_t dest, uint16_t val);
    [[nodiscard]] uint16_t peekInReg(uint16_t reg) const;
    [[nodiscard]] uint16_t fetchByteAsWordFromMem(uint16_t addr) const;
    [[nodiscard]] uint8_t fetchByteFromMem(uint16_t addr) const;
    [[nodiscard]] uint16_t fetchWordFromMem(uint16_t addr) const; //little endian
    static std::array<uint8_t, 2> convertWordToBytePair(uint16_t val);
    void writeWordToMem(uint16_t addr, uint16_t val); //little endian
    void writeByteToMem(uint16_t addr, uint8_t val);
};
class Board {
    bool power = true;
    const std::size_t ROM_SIZE;
    const std::size_t SRAM_SIZE;
    uint64_t input  = 0;
    uint64_t output = 0;
public:
    CPU16 cpu; //public for testing purposes (change later)
    Board(std::size_t romSize, std::size_t sramSize);
    void loadRomFromBinInTxtFile(const std::string &path);
    void loadRomFromManualBinVec(std::vector<uint8_t> rom);
};

#endif //CORE_H