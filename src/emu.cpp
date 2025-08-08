//
// Created by Zachary Mahan on 6/4/25.
//
#include "emu.h"
#include "assembly.h"
#include "cli_args.h"
#include "core.h"
#include "json.hpp"
#include "path_manager.h"
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>
Emulator::Emulator(emu_launch launchState) : launchState(launchState) {}
int Emulator::assembleAndRunWithoutSavingExecutable() {
    testAssembler.openProject(projectPath, entryFileName);
    auto userRom = testAssembler.assembleOpenedProject();

    // init emulated system
    Board board(false);
    board.loadUserRomFromByteVector(userRom);
    board.loadDiskFromBinFile(diskPath.string());
    // run
    LOG_ERR("Launched the Bear16 Emulator...");
    int exitCode = board.run();

    // save disk state
    board.saveDiskToBinFile(diskPath.string());

    // display diagnostics:
    board.printDiagnostics(false);
    std::cout << "Emulated process (version " + version +
                     ") finished with exit code "
              << exitCode << std::endl;
    std::cout << std::endl;
    return exitCode;
}
int Emulator::launch(int argc, char **argv) {
    int exitCode = 0;
    if (launchState == emu_launch::tui) {
        enterTUI();
    } else {
        std::vector<std::string> args = vectorizeArgs(argc, argv);
        exitCode = performActionBasedOnArgs(args);
    }
    return exitCode;
}
int Emulator::performActionBasedOnArgs(const std::vector<std::string> &args) {
    int exitCode = 0;
    std::unordered_set<cli_flag> flags = parseFlags(args);

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
            enterConfigMenu();
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
    int LINE_LEN = 61;
    std::string emuTitle =
        std::format("| {:^57} |", std::string("BEAR16 Emulator & Assembler - v" + version +
                                      " (" + dateOfLastVersion + ")"));
    std::string author = "Made by Zach Mahan";
    std::string authorLine = std::format("| {:^57} |", author);
    std::string emuTitleBar = std::string(emuTitle.size(), '=');
    std::string emuDivideBar = std::string(emuTitle.size(), '-');
    std::string emuBorderDivLine =
        std::format("|{}|", std::string(emuTitle.size() - 2, '-'));
    auto getProjPathSubtitle = [this]() {
        return projectPath.string().substr(
            projectPath.string().find_last_of("/") + 1);
    };
    std::string openProject = "Open Project: " + getProjPathSubtitle();
    std::string openProjectLine = std::format("| {:^57} |", openProject);
    std::string fullPath =
        std::format("| {:^57} |", "(" + projectPath.string() + ")");
#define PRINT_TITLE_BAR() std::cout << emuTitleBar << std::endl
#define PRINT_DIVIDE_BAR() std::cout << emuDivideBar << std::endl
#define PRINT_BORDER_DIV_LINE() std::cout << emuBorderDivLine << std::endl

    PRINT_TITLE_BAR();
    std::cout << emuTitle << std::endl;
    std::cout << authorLine << std::endl;
    PRINT_TITLE_BAR();
    std::cout << openProjectLine << std::endl;
    if (fullPath.length() <= LINE_LEN) {
        std::cout << fullPath << std::endl;
    }
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
    std::cout << "Make a selection: ";
}
void Emulator::assembleAndSaveExecutable() {
    testAssembler.openProject(projectPath, entryFileName);
    auto userRom = testAssembler.assembleOpenedProject();

    std::filesystem::path executablePath = computeDefaultExecutablePath();

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
    board.loadRomFromBinFile(computeDefaultExecutablePath().string());
    board.loadDiskFromBinFile(diskPath.string());
    // run
    LOG_ERR("Launching the Bear16 Emulator...");
    int exitCode = board.run();

    // save disk state
    board.saveDiskToBinFile(diskPath.string());

    // display diagnostics
    board.printDiagnostics(false);
    std::cout << "Emulated process (version " + version +
                     ") finished with exit code "
              << exitCode << std::endl;
    std::cout << std::endl;
}

void Emulator::enterConfigMenu() {
    auto printInvalidChoice = []() {
        std::cout << "Invalid choice. Please try again." << std::endl;
    };
    std::string line;
    do {
        printConfigMenu();
        std::cout << "Make a selection: ";
        std::getline(std::cin, line);
        std::ranges::transform(line, line.begin(),
                               [](char c) { return std::tolower(c); });

        if (line.length() != 1) {
            printInvalidChoice();
            enterToContinue();
            continue;
        }
        char selection = line.at(0);
        switch (selection) {
        case ('1'): {
            break;
        }
        case ('2'): {
            getProjectPathFromUser();
            break;
        }
        case ('3'): {
            break;
        }
        case ('4'): {
            break;
        }
        case ('5'): {
        }
        case ('c'): {
            break;
        }
        default: {
            printInvalidChoice();
            enterToContinue();
            continue;
        }
        }
    } while (line.empty() || line.at(0) != 'c');
}

void Emulator::printConfigMenu() {
    using namespace std;
    cout << "\n";
    cout << " Enter a [#] to make a change" << "\n";
    cout << "==============================" << "\n";
    cout << " [1] Bear16 Root Directory: " << bear16RootDir << "\n";
    cout << " [2] Project Directory: " << projectPath << "\n";
    cout << " [3] Entry .asm file: " << entryFileName << "\n";
    cout << " [4] Disk Path: " << diskPath << "\n";
    cout << " [5] Restore defaults" << "\n";
    cout << " [C] Cancel" << "\n";
}

void Emulator::printHelpMessage() {
    std::ifstream helpMessageFile(
        std::filesystem::path(TUI_PATH / HELP_MESSAGE));
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
std::filesystem::path Emulator::computeDefaultExecutablePath() const {
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
        j["bear16RootDir"] = bear16RootDir;
        j["projectPath"] = snipBear16RootDir(projectPath);
        j["entry"] = entryFileName;
        j["diskPath"] = snipBear16RootDir(diskPath);

        std::ofstream outStream(std::filesystem::path(TUI_PATH / CONFIG));
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
    std::filesystem::path configPath = std::filesystem::path(TUI_PATH / CONFIG);
    try {
        if (!std::filesystem::exists(configPath)) {
            std::cout << "Oh, it's your first time using Bear16? Let's do some "
                         "automatic set up.\n";
            std::cout << "Generating default config.json ...\n";
            enterToContinue();
            saveEmuStateToConfigFile(); // saves the defaults, which the emu
                                        // boots up w/
        }
        std::ifstream inStream(configPath);
        if (!inStream) {
            std::cerr << "ERROR: Could not open config file." << std::endl;
            return;
        }
        nlohmann::json j{};
        inStream >> j;
        if (j[bear16RootDir.string()].empty()) {
            bear16RootDir = std::filesystem::path(getBear16DefaultRootDir());
        } else {
            bear16RootDir = std::filesystem::path(j["bear16RootDir"]);
        }
        projectPath = bear16RootDir / std::filesystem::path(j["projectPath"]);
        entryFileName = j["entry"];
        diskPath = bear16RootDir / std::filesystem::path(j["diskPath"]);
    } catch (const std::exception &e) {
        std::cerr << "ERROR: Could not read config file: " << e.what()
                  << std::endl;
    }
}
std::filesystem::path
Emulator::snipBear16RootDir(const std::filesystem::path &path) {
    std::string snippedOfHomeDir = snipHomeDir(path.string());
    size_t pos = snippedOfHomeDir.find_first_of("/\\");
    std::string snippedOfBear16RootDir{};
    if (pos != std::string::npos) {
        snippedOfBear16RootDir = snippedOfHomeDir.substr(pos + 1);
    } else {
        snippedOfBear16RootDir = snippedOfHomeDir;
    }
    return std::filesystem::path(snippedOfBear16RootDir);
}
void Emulator::getProjectPathFromUser() {
    std::string projectDir;
    std::cout << "Enter the name of the project directory: " << "\n";
    std::cout << bear16RootDir.string() << '/';
    std::getline(std::cin, projectDir);
    if (projectDir.empty()) {
        std::cout << "ERROR: Project path cannot be empty." << std::endl;
        enterToContinue();
        return;
    }
    std::string projectPath((bear16RootDir / projectDir).string());
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
