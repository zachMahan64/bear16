//
// Created by Zachary Mahan on 6/4/25.
//
#include "emu.h"
#include "assembly.h"
#include "core.h"
#include "json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

int Emulator::assembleAndRunWithoutSavingExecutable() {
    testAssembler.openProject(projectPath, entryFileName);
    auto userRom = testAssembler.assembleOpenedProject();

    // init emulated system
    Board board(false);
    board.loadUserRomFromByteVector(userRom);
    board.loadDiskFromBinFile(diskPath);
    // run
    LOG_ERR("Launching the Bear16 Emulator...");
    int exitCode = board.run();

    // save disk state
    board.saveDiskToBinFile(diskPath);

    // display diagnostics:
    board.printDiagnostics(false);
    std::cout << "Emulated process (version " + version +
                     ") finished with exit code "
              << exitCode << std::endl;
    std::cout << std::endl;
    return exitCode;
}
void Emulator::enterTUI() {
    getEmuStateFromConfigFile();
    auto printInvalidChoice = []() {
        std::cout << "Invalid choice. Please try again." << std::endl;
    };
    std::string choice;
    bool hasLaunchedEmu = false;
    do {
        printTUIMainMenu();
        std::getline(std::cin, choice);
        std::ranges::transform(choice, choice.begin(),
                               [](char c) { return std::tolower(c); });
        if (choice.size() > 1) {
            printInvalidChoice();
            continue;
        }
        if (choice.empty()) {
            printInvalidChoice();
            continue;
        }
        switch (choice.at(0)) {
        case '1': {
            assembleAndRunWithoutSavingExecutable();
            hasLaunchedEmu = true;
            break;
        }
        case '2': {
            assembleAndSaveExecutable();
            break;
        }
        case '3': {
            runSavedExecutable();
            hasLaunchedEmu = true;
            break;
        }
        case '4': {
            assembleAndSaveExecutable();
            runSavedExecutable();
            hasLaunchedEmu = true;
            break;
        }
        case 'p': {
            getProjectPathFromUser();
            break;
        }
        case 'c': {
            std::cout << "THIS FEATURE IS WIP." << std::endl;
            enterToContinue();
            break;
        }
        case 'h': {
            printHelpMessage();
            break;
        }
        case 'q': {
            // we're done here
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
    std::string emuTitle =
        std::format("| {:^57} |", "BEAR16 Emulator & Assembler - v" + version +
                                      " (" + dateOfLastVersion + ")");
    std::string author = "Made by Zach Mahan";
    std::string authorLine = std::format("| {:^57} |", author);
    std::string emuTitleBar = std::string(emuTitle.size(), '=');
    std::string emuDivideBar = std::string(emuTitle.size(), '-');
    std::string emuBorderDivLine =
        std::format("|{}|", std::string(emuTitle.size() - 2, '-'));
    auto getTrimmedProjectPath = [this]() {
        return projectPath.string().substr(
            projectPath.string().find_last_of("/") + 1);
    };
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
    std::cout << std::format("| {:<57} |",
                             "[1] Assemble & Run Without Saving Executable")
              << std::endl;
    std::cout << std::format("| {:<57} |", "[2] Assemble & Save Executable")
              << std::endl;
    std::cout << std::format("| {:<57} |", "[3] Run Saved Executable")
              << std::endl;
    std::cout << std::format("| {:<57} |",
                             "[4] Assemble, Save, & Run Executable")
              << std::endl;
    PRINT_BORDER_DIV_LINE();
    std::cout << std::format("| {:<57} |", "[P] Open a Different Project")
              << std::endl;
    std::cout << std::format("| {:<57} |", "[C] Configure") << std::endl;
    std::cout << std::format("| {:<57} |", "[H] Help") << std::endl;
    std::cout << std::format("| {:<57} |", "[Q] Quit") << std::endl;
    PRINT_DIVIDE_BAR();
    std::cout << "Make a selection: " << std::endl;
}
void Emulator::assembleAndSaveExecutable() {
    testAssembler.openProject(projectPath, entryFileName);
    auto userRom = testAssembler.assembleOpenedProject();

    std::filesystem::path executablePath = computeBinPath();

    std::ofstream executableFile(executablePath, std::ios::binary);
    if (!executableFile) {
        std::cerr << "Failed to open output file: " << executablePath
                  << std::endl;
        return;
    }

    executableFile.write(reinterpret_cast<const char *>(userRom.data()),
                         userRom.size());
    if (!executableFile) {
        std::cerr << "Failed to write to file: " << executablePath << std::endl;
        return;
    }
    executableFile.flush();
    executableFile.close();

    std::cout << "Executable saved to " << executablePath << std::endl;
    enterToContinue();
}
void Emulator::runSavedExecutable() {
    std::vector<uint8_t> userRom{};

    // init emulated system
    Board board(false);
    board.loadRomFromBinFile(computeBinPath().string());
    board.loadDiskFromBinFile(diskPath);
    // run
    LOG_ERR("Launching the Bear16 Emulator...");
    int exitCode = board.run();

    // save disk state
    board.saveDiskToBinFile(diskPath);

    // display diagnostics
    board.printDiagnostics(false);
    std::cout << "Emulated process (version " + version +
                     ") finished with exit code "
              << exitCode << std::endl;
    std::cout << std::endl;
}

void Emulator::enterConfigMenu() {}

void Emulator::printConfigMenu() { std::cout << " [1] Change .asm entry file"; }

void Emulator::printHelpMessage() {
    std::ifstream helpMessageFile("../tui/help_message.txt");
    if (helpMessageFile) {
        std::string helpMessageBuffer(
            (std::istreambuf_iterator<char>(helpMessageFile)),
            std::istreambuf_iterator<char>());
        std::cout << helpMessageBuffer;
    } else {
        std::cerr << "ERROR: Could not open help message file." << std::endl;
    }
    std::cout << std::endl;
    enterToContinue();
}
std::filesystem::path Emulator::computeBinPath() const {
    // Ensure projectPath is a valid path
    std::filesystem::path projectFilePath(projectPath);
    if (!std::filesystem::exists(projectFilePath)) {
        std::cerr << "Project file does not exist: " << projectPath
                  << std::endl;
        return {};
    }
    // compute the output binary path
    std::filesystem::path executableName =
        projectFilePath.stem(); // filename w/o extension
    std::filesystem::path executablePath =
        projectFilePath / (executableName.string() + ".bin");
    return executablePath;
}

void Emulator::saveEmuStateToConfigFile() {
    try {
        nlohmann::json j{};
        j["projectPath"] = projectPath;
        j["entry"] = entryFileName;
        j["diskPath"] = diskPath;

        std::ofstream outStream("../tui/config.json");
        if (!outStream) {
            std::cerr << "ERROR: Could not open config file." << std::endl;
            return;
        }
        outStream << j.dump(4);
    } catch (const std::exception &e) {
        std::cerr << "ERROR: Could not save to config file: " << e.what()
                  << std::endl;
    }
}

void Emulator::getEmuStateFromConfigFile() {
    try {
        std::ifstream inStream("../tui/config.json");
        if (!inStream) {
            std::cerr << "ERROR: Could not open config file." << std::endl;
            return;
        }
        nlohmann::json j{};
        inStream >> j;
        projectPath = std::filesystem::path(j["projectPath"]);
        entryFileName = j["entry"];
        diskPath = std::filesystem::path(j["diskPath"]);
    } catch (const std::exception &e) {
        std::cerr << "ERROR: Could not read config file: " << e.what()
                  << std::endl;
    }
}

void Emulator::getProjectPathFromUser() {
    std::string projectDir;
    std::cout << "Enter the name of the project directory: ";
    std::getline(std::cin, projectDir);
    if (projectDir.empty()) {
        std::cout << "ERROR: Project path cannot be empty." << std::endl;
        enterToContinue();
        return;
    }
    std::string projectPath(
        (std::filesystem::path("../projects_b16") / projectDir).string());
    if (!std::filesystem::exists(projectPath)) {
        std::cout << "Project path does not exist: " << projectPath
                  << std::endl;
        bool madeValidChoice = false;
        do {
            std::cout << " [1] Create new project directory" << std::endl;
            std::cout << " [2] Cancel" << std::endl;
            std::cout << "Make a selection: ";
            std::string choice;
            std::getline(std::cin, choice);
            if (!(choice == "1" || choice == "2")) {
                std::cout << "Invalid choice. Please try again." << std::endl;
            }
            if (choice == "1") {
                std::filesystem::create_directory(projectPath);
                std::cout << "Project directory created at " << projectPath
                          << std::endl;
                madeValidChoice = true;
                enterToContinue();
            } else if (choice == "2") {
                madeValidChoice = true;
            }
        } while (!madeValidChoice);
        return;
    }
    this->projectPath = projectPath;
    std::cout << "Project path set to: " << projectDir << std::endl;
    saveEmuStateToConfigFile();
    enterToContinue();
}
void Emulator::enterToContinue() {
    std::cout << "[ENTER] to continue" << std::endl;
    std::cin.get();
}
