#include <iostream>
#include <cstdint>
#include <map>
#include <thread>
#include <vector>
#include "isa.h"
#include "parts.h"
#include "core.h"

using namespace std;

int runEmu() {
    constexpr std::size_t ROM_SIZE  = isa::ROM_SIZE;
    constexpr std::size_t SRAM_SIZE = isa::SRAM_SIZE;

    Board board(ROM_SIZE, SRAM_SIZE);
    // vector<uint8_t> testRom = {0xC0, 0x02,  //opcode
    //                            0x00, 0x00,  //dest
    //                            0x00, 0x07,  //src1
    //                            0x00, 0x08}; //src2
    // board.loadRomFromManualBinVec(testRom);
    board.loadRomFromBinInTxtFile("../programs/bin_prog.txt");
    board.cpu.step();
    board.cpu.run();
    cout << "pc: " << board.cpu.getPc() << endl;
    cout << board.cpu.peekInReg(0) << endl;
    cout << board.cpu.peekInReg(1) << endl;
    return 0;
}

int main() {
    return runEmu();
}