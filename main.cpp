#include <iostream>
#include <cstdint>
#include <map>
#include <thread>
#include <vector>

#include "assembler.h"
#include "isa.h"
#include "parts.h"
#include "core.h"

using namespace std;

int runEmu() {
    constexpr std::size_t ROM_SIZE  = isa::ROM_SIZE;
    constexpr std::size_t SRAM_SIZE = isa::SRAM_SIZE;

    std::string path = "../programs/asm_test.asm";
    assembler::Assembler testAssembler = assembler::Assembler::initInstance(true, false);
    auto byteVec = testAssembler.assembleFromFile(path);

    Board board(ROM_SIZE, SRAM_SIZE);
    board.loadRomFromManualBinVec(byteVec);
    //board.loadRomFromBinInTxtFile("../programs/bin_prog.txt");
    //board.loadRomFromHexInTxtFile("../programs/hex_prog.txt");
    board.run();
    std::cout << std::dec;
    cout << "RESULTS\n========" << endl;
    cout << "pc: " << board.cpu.getPc() << endl;
    int x = board.cpu.peekInReg(0);
    cout << "t0: " << board.cpu.peekInReg(0) << endl;
    cout << "t1: " << board.cpu.peekInReg(1) << endl;
    cout << "t2: " << board.cpu.peekInReg(2) << endl;
    cout << "t3: " << board.cpu.peekInReg(3) << endl;
    cout << "s0: " << board.cpu.peekInReg(4) << endl;
    cout << "s1: " << board.cpu.peekInReg(5) << endl;
    cout << "s2: " << board.cpu.peekInReg(6) << endl;
    cout << "s3: " << board.cpu.peekInReg(7) << endl;
    cout << "s4: " << board.cpu.peekInReg(8) << endl;
    cout << "s5: " << board.cpu.peekInReg(9) << endl;
    cout << "s6: " << board.cpu.peekInReg(10) << endl;
    uint16_t startingAddr = 4096;
    uint16_t numBytes = 20;
    board.cpu.printSectionOfMem(startingAddr, numBytes, false);
    return 0;
}

int main() {
    runEmu();
    return 0;
}