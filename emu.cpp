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

#include <regex>

int Emulator::assembleAndRun() const {
    //assemble
    assembly::Assembler testAssembler(false, false);
    //testAssembler.openProject("../projects_b16/kernel_versions/", "kv_001.asm");
    //auto kernelRom = testAssembler.assembleOpenedProject();
    std::vector<uint8_t> kernelRom {};
    testAssembler.openProject(projectPath, entry);
    auto userRom = testAssembler.assembleOpenedProject();

    //init emulated system
    Board board(false);
    board.loadKernelRomFromByteVector(kernelRom);
    board.loadUserRomFromByteVector(userRom);
    board.loadDiskFromBinFile(diskPath);
    //run
    int exitCode = board.run();

    //save disk state
    board.saveDiskToBinFile(diskPath);

    //display diagnostics
    board.printDiagnostics(false);
    std::cout << "Emulated process (version " + version + ") finished with exit code " << exitCode << std::endl;
    std::cout << std::endl;
    return exitCode;
}

void Emulator::enterTUI() {
    printTUIStartUp();
    auto printOptions = [] () {
        std::cout << " [S] Assemble & Run" << std::endl;
        std::cout << " [A] Assemble" << std::endl;
        std::cout << " [R] Run" << std::endl;
        std::cout << " [C] Configure" << std::endl;
        std::cout << " [H] Help" << std::endl;
        std::cout << " [Q] Quit" << std::endl;
    };
    auto printInvalidChoice = [] () {
        std::cout << "Invalid choice. Please try again." << std::endl;
    };
    std::string choice;
    bool hasLaunchedEmu = false;
    do {
        printOptions();
        std::getline(std::cin, choice);
        std::ranges::transform(choice, choice.begin(), [] (char c) {return std::tolower(c);});
        if (choice.size() > 1 ) {
            printInvalidChoice();
            continue;
        }

        switch (choice.at(0)) {
            case 's': {
                assembleAndRun();
                hasLaunchedEmu = true;
                break;
            }
            case 'a': {
                // assemble
                break;
            }
            case 'r': {
                //run
                hasLaunchedEmu = true;
                break;
            }
            case 'c': {
                // do config
                break;
            }
            case 'h': {
                // display help
                break;
            }
            default: {
                printInvalidChoice();
                break;
            }
        }
    } while (choice != "q" && !hasLaunchedEmu);
}

void Emulator::printTUIStartUp() {
    std::string emuTitle = " BEAR16 Emulator & Assembler (Version " + version + ", 20250718) ";
    std::string author = "Made by Zach Mahan";
    std::string authorLine = std::string((emuTitle.size() - author.size()) / 2, ' ') + author;
    std::string emuTitleBar = std::string(emuTitle.size(), '=');

    std::cout << emuTitleBar << std::endl;
    std::cout << emuTitle << std::endl;
    std::cout << authorLine << std::endl;
    std::cout << emuTitleBar << std::endl;
}

