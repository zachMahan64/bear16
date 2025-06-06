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
    bool isEnableDebug = false;
public: //only all public for debugging ease
    //ISA specs
    const size_t NUM_GEN_REGS = isa::GEN_REG_COUNT;
    const short NUM_IO = isa::IO_COUNT;
    //MEMORY
    std::array<uint8_t, isa::SRAM_SIZE> sram {};
    std::array<uint8_t, isa::SRAM_SIZE> rom {};
    //CTRL FLOW (primitive registers)
    uint16_t pc = 0;
    uint16_t tickWaitCnt = 0; //for multicycle operations
    uint16_t tickWaitStopPt = 0;
    bool pcIsFrozen = false;
    bool pcIsStoppedThisCycle = false;
    bool cpuIsHalted = false;
    //IO
    std::array<uint16_t, isa::IO_COUNT> inps {};
    std::array<uint16_t, isa::IO_COUNT> outs {};
    //REG
    std::array<parts::GenRegister, isa::GEN_REG_COUNT> genRegs{};
    parts::FlagRegister flagReg = parts::FlagRegister();
    parts::GenRegister  stackPtr = parts::GenRegister(static_cast<uint16_t>(isa::SRAM_SIZE)); //sp stacks at end of RAM for downward growth
    parts::GenRegister  framePtr = parts::GenRegister(static_cast<uint16_t>(isa::SRAM_SIZE));
    //Constr
    explicit CPU16(bool enableDebug) : isEnableDebug(enableDebug) {};
    //doStuff
    [[nodiscard]] uint16_t getPc() const;
    void setRom(std::vector<uint8_t>& rom);
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
    [[nodiscard]] uint16_t getValInReg(uint16_t reg) const;
    [[nodiscard]] uint16_t fetchByteAsWordFromRam(uint16_t addr) const;
    [[nodiscard]] uint8_t fetchByteFromRam(uint16_t addr) const;
    [[nodiscard]] uint16_t fetchWordFromRam(uint16_t addr) const; //little endian
    static std::array<uint8_t, 2> convertWordToBytePair(uint16_t val);
    void writeWordToRam(uint16_t addr, uint16_t val); //little endian
    void writeByteToRam(uint16_t addr, uint8_t val);
    [[nodiscard]] uint16_t fetchByteAsWordFromRom(uint16_t addr) const;
    [[nodiscard]] uint8_t fetchByteFromRom(uint16_t addr) const;
    [[nodiscard]] uint16_t fetchWordFromRom(uint16_t addr) const;

    void jumpTo(const uint16_t& destAddrInRom);

    //diagnostic
    void printSectionOfRam(uint16_t& startingAddr, uint16_t& numBytes, bool asChars) const;
};
class Board {
    bool power = true;
    bool isEnableDebug = false;
    const std::size_t ROM_SIZE = isa::ROM_SIZE;
    const std::size_t SRAM_SIZE = isa::SRAM_SIZE;
    uint64_t input  = 0;
    uint64_t output = 0;
public:
    CPU16 cpu; //public for testing purposes (change later)
    parts::Clock clock = parts::Clock();
    explicit Board(bool enableDebug);
    int run();
    void printDiagnostics(bool printMemAsChars) const;
    void loadRomFromBinInTxtFile(const std::string &path);
    void loadRomFromHexInTxtFile(const std::string &path);
    void loadRomFromByteVector(std::vector<uint8_t>& rom);
};

#endif //CORE_H