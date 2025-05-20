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
    vector<uint8_t> testRom = {0xC0, 0x02,  //opcode
                               0x00, 0x00,  //dest
                               0x00, 0x07,  //src1
                               0x00, 0x08}; //src2
    board.loadRomFromManualBinVec(testRom);
    board.cpu.step();
    cout << board.cpu.peekInReg(0) << endl;

    //while (true) {}
    return 0;
}

int main() {
    return runEmu();
}