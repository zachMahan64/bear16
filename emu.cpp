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

int Emulator::assembleAndRun() {
    //assemble
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
    auto printInvalidChoice = [] () {
        std::cout << "Invalid choice. Please try again." << std::endl;
    };
    std::string choice;
    bool hasLaunchedEmu = false;
    do {
        printTUIMainMenu();
        std::getline(std::cin, choice);
        std::ranges::transform(choice, choice.begin(), [] (char c) {return std::tolower(c);});
        if (choice.size() > 1 ) {
            printInvalidChoice();
            continue;
        }
        if (choice.empty()) {
            printInvalidChoice();
            continue;
        }

        switch (choice.at(0)) {
            case 'd': {
                assembleAndRun();
                hasLaunchedEmu = true;
                break;
            }
            case 's': {
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
            case 'p': {
                // select project to open
                break;
            }
            case 'h': {
                // display help
                break;
            }
            case 'q': {
                //we're done here
                break;
            }
            default: {
                printInvalidChoice();
                break;
            }
        }
    } while (choice != "q" && !hasLaunchedEmu);
}

void Emulator::printTUIMainMenu() {
#define MM_WIDTH 55
    std::string emuTitle = std::format("| {:^57} |", "BEAR16 Emulator & Assembler (Version " + version + ", " + dateOfLastVersion + ")");
    std::string author = "Made by Zach Mahan";
    std::string authorLine = std::format("| {:^57} |", author);
    std::string emuTitleBar = std::string(emuTitle.size(), '=');
    std::string emuDivideBar = std::string(emuTitle.size(), '-');
    std::string emuBorderDivLine = std::format("|{}|", std::string(emuTitle.size() - 2, '-'));
    auto getTrimmedProjectPath = [this]() {return projectPath.substr(projectPath.find_last_of("/\\") + 1);};
    std::string openProject = "Open Project: " + getTrimmedProjectPath();
    std::string openProjectLine = std::format("| {:^57} |", openProject);
#define PRINT_TITLE_BAR() std::cout << emuTitleBar << std::endl
#define PRINT_DIVIDE_BAR() std::cout << emuDivideBar << std::endl
#define PRINT_BORDER_DIV_LINE() std::cout << emuBorderDivLine << std::endl

    PRINT_TITLE_BAR();
    std::cout << emuTitle << std::endl;
    std::cout << authorLine << std::endl;
    PRINT_TITLE_BAR();
    std::cout << openProjectLine << std::endl;
    PRINT_TITLE_BAR();
    std::cout << std::format("| {:<57} |", "[D] Run Directly From Assembly") << std::endl;
    std::cout << std::format("| {:<57} |", "[A] Assemble & Save Executable") << std::endl;
    std::cout << std::format("| {:<57} |", "[R] Run Executable") << std::endl;
    std::cout << std::format("| {:<57} |", "[S] Assemble, Save Executable, & Run") << std::endl;
    PRINT_BORDER_DIV_LINE();
    std::cout << std::format("| {:<57} |", "[P] Open a Different Project") << std::endl;
    std::cout << std::format("| {:<57} |", "[C] Configure") << std::endl;
    std::cout << std::format("| {:<57} |", "[H] Help") << std::endl;
    std::cout << std::format("| {:<57} |", "[Q] Quit") << std::endl;
    PRINT_DIVIDE_BAR();
    std::cout << "Make a selection: " << std::endl;
}

