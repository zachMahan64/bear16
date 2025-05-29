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

    Board board(ROM_SIZE, SRAM_SIZE);
    // vector<uint8_t> testRom = {0xC0, 0x02,  //opcode
    //                            0x00, 0x00,  //dest
    //                            0x00, 0x07,  //src1
    //                            0x00, 0x08}; //src2
    // board.loadRomFromManualBinVec(testRom);
    //board.loadRomFromBinInTxtFile("../programs/bin_prog.txt");
    board.loadRomFromHexInTxtFile("../programs/hex_prog.txt");
    board.run();
    cout << "RESULTS\n========" << endl;
    cout << "pc: " << board.cpu.getPc() << endl;
    cout << "REG 0: " << board.cpu.peekInReg(0) << endl;
    cout << "REG 1: " << board.cpu.peekInReg(1) << endl;
    cout << "REG 2: " << board.cpu.peekInReg(2) << endl;
    cout << "REG 3: " << board.cpu.peekInReg(3) << endl;
    uint16_t addrToSee = 4;
    cout << "MEM @ addr = " << addrToSee << endl;
    cout << board.cpu.fetchWordFromMem(addrToSee) << endl;
    return 0;
}

int main() {
    //assembler::asmToBinMapGenerator();
    auto jawnt = assembler::tokenizeAsmFirstPass("../programs/asm_test.asm");
    assembler::parseFirstPassIntoSecondPass(jawnt);
    //runEmu();
    return 0;
}