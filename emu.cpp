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

int emu::Emulator::assembleAndRun(const std::string& path) const {
    //assemble
    assembly::Assembler testAssembler(false, false);
    auto kernelRom = testAssembler.assembleFromFile("../projects/kernel_versions/kv_001.asm");
    auto userRom = testAssembler.assembleFromFile(path);

    //init emulated system
    Board board(false);
    board.loadKernelRomFromByteVector(kernelRom);
    board.loadUserRomFromByteVector(userRom);

    //run
    int exitCode = board.run();

    //display diagnostics
    board.printDiagnostics(false);
    std::cout << "Emulated process (version " + version + ") finished with exit code " << exitCode << std::endl;
    std::cout << std::endl;
    return exitCode;
}
