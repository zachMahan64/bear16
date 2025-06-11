//
// Created by Zachary Mahan on 6/4/25.
//
#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>
#include <cstddef>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <thread>
#include <vector>
#include "assembly.h"
#include "isa.h"
#include "parts.h"
#include "core.h"
#include "emu.h"

int emu::Emulator::assembleAndRun() {
    //assemble
    std::string path = "../programs/asm_test.asm";
    assembly::Assembler testAssembler(false, false);
    auto byteVec = testAssembler.assembleFromFile(path);

    //init emulated system
    Board board(true);
    board.loadRomFromByteVector(byteVec);
    //board.loadRomFromBinInTxtFile("../programs/bin_prog.txt");
    //board.loadRomFromHexInTxtFile("../programs/hex_prog.txt");

    //run
    int exitCode = board.run();

    //display diagnostics
    board.printDiagnostics(true);
    std::cout << "Emulated process (version " + version + ") finished with exit code " << exitCode << std::endl;
    return exitCode;
}
