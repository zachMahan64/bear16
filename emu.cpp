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

int emu::Emulator::assembleAndRun(const std::string &projectPath, const std::string& entry,
            const std::string& diskDataPath) const {
    //assemble
    assembly::Assembler testAssembler(false, false);
    //testAssembler.openProject("../projects/kernel_versions/", "kv_001.asm");
    //auto kernelRom = testAssembler.assembleOpenedProject();
    std::vector<uint8_t> kernelRom {};
    testAssembler.openProject(projectPath, entry);
    auto userRom = testAssembler.assembleOpenedProject();

    //init emulated system
    Board board(false);
    board.loadKernelRomFromByteVector(kernelRom);
    board.loadUserRomFromByteVector(userRom);
    board.loadDiskFromBinFile(diskDataPath);
    //run
    int exitCode = board.run();

    //save disk state
    board.saveDiskToBinFile(diskDataPath);

    //display diagnostics
    board.printDiagnostics(false);
    std::cout << "Emulated process (version " + version + ") finished with exit code " << exitCode << std::endl;
    std::cout << std::endl;
    return exitCode;
}

