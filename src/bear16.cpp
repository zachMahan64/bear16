//
// Created by Zachary Mahan on 6/4/25.
//
#include "bear16.h"
#include "assembly.h"
#include "cli_args.h"
#include "core.h"
#include "help_messages.h"
#include "isa.h"
#include "json.hpp"
#include "path_manager.h"
#include <algorithm>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <system_error>
#include <unordered_set>
#include <vector>
Bear16::Bear16(emu_launch launchState) : launchState(launchState) {}
int Bear16::launch(int argc, char** argv) {
    int exitCode = 0;
    getEmuStateFromConfigFile();
    buildBear16DirsIfDNE();
    if (launchState == emu_launch::tui) {
        enterTUI();
    } else {
        std::vector<std::string> args = vectorizeArgs(argc, argv);
        exitCode = performActionBasedOnArgs(args);
    }
    return exitCode;
}
int Bear16::assembleAndRunWithoutSavingExecutable() {
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
    board.printDiagnostics();
    printProcessTerminationMsg(exitCode);
    return exitCode;
}
int Bear16::performActionBasedOnArgs(const std::vector<std::string>& args) {
    int exitCode = 0;
    std::unordered_set<cli_error_e> errors{};
    std::unordered_set<cli_flag> flags = parseFlags(args, errors);
    MentionedFiles mentionedFiles = parseArgsForMentionedFiles(args, errors);
    if (mentionedFiles.asmFile.empty() && flags.contains(cli_flag::assemble)) {
        errors.insert(cli_error_e::missing_asm_file);
    }
    if (mentionedFiles.binFile.empty() &&
        (flags.contains(cli_flag::assemble) || flags.contains(cli_flag::run))) {
        errors.insert(cli_error_e::missing_bin_file);
    }
    bool errorFlag = parseForUnrecognizedArgs(args, errors);
    bool doDump = flags.contains(cli_flag::dump);
    bool doAssemble = flags.contains(cli_flag::assemble);
    bool doRun = flags.contains(cli_flag::run);
    bool doTUI = flags.contains(cli_flag::tui);
    bool doSetDisk = flags.contains(cli_flag::set_disk);
    bool doCheckDisk = flags.contains(cli_flag::check_disk);
    bool doResetDisk = flags.contains(cli_flag::reset_disk);
    bool doHelp = flags.contains(cli_flag::help);
    bool doVersion = flags.contains(cli_flag::version);
    bool doDoctor = flags.contains(cli_flag::doctor);

    getEmuStateFromConfigFile(); // for getting disk, everything else will be ignored while using
                                 // CLI

    if (doAssemble) {
        if (mentionedFiles.binFile.empty()) {
            errors.insert(cli_error_e::missing_bin_file);
        }
        if (mentionedFiles.asmFile.empty()) {
            errors.insert(cli_error_e::missing_asm_file);
        }
        if (!errors.empty()) {
            enumerateErrorsAndTerminate(errors);
        }
        projectPath = std::filesystem::canonical(mentionedFiles.asmFile).parent_path();
        entryFileName = std::filesystem::path(mentionedFiles.asmFile).filename().string();
        std::filesystem::path execPath = std::filesystem::path(mentionedFiles.binFile);
        if (assembleAndSaveExecutable(execPath)) {
            mentionedFiles.binFileState = bin_file_state::exists; // if assembly succeeds
        }
    }

    if (doRun) {
        if (mentionedFiles.binFile.empty()) {
            errors.insert(cli_error_e::missing_bin_file);
        }
        if (mentionedFiles.binFileState == bin_file_state::does_not_exist &&
            !errors.contains(cli_error_e::missing_bin_file)) {
            // condition simply avoids redundancy in error enumeration
            errors.insert(cli_error_e::bin_file_does_not_exist);
        }
        if (!errors.empty()) {
            enumerateErrorsAndTerminate(errors);
        }
        runMentionedExecutable(
            mentionedFiles.binFile,
            doDump); // dump state at process termination if --dump was specified!
    }

    if (!errors.empty()) {
        enumerateErrorsAndTerminate(errors); // guard after assemble or run
    }

    auto guardNoArgFlagCommands = [&errors, &args]() {
        if (args.size() > 2) {
            errors.insert(cli_error_e::too_many_arguments);
        }
        if (!errors.empty()) {
            enumerateErrorsAndTerminate(errors);
        }
    };

    if (doTUI) {
        guardNoArgFlagCommands();
        enterTUI();
    }

    if (doSetDisk) {
        guardNoArgFlagCommands();
        getDiskPathFromUser();
    }
    if (doResetDisk) {
        guardNoArgFlagCommands();
        resetDisk();
    }

    if (doCheckDisk) {
        guardNoArgFlagCommands();
        std::cout << "-> loaded disk: " << diskPath << "\n";
        if (std::filesystem::exists(diskPath)) {
            std::cout << "-> exists, valid path \n";
        } else {
            std::cout << "-> ERROR: does not exist, invalid path \n";
            std::cout << "[!] hint: use \"b16 --set-disk\" or \"b16 -sd\" to change the disk\n";
        }
    }

    if (doHelp) {
        guardNoArgFlagCommands();
        std::cout << helpMessageCli;
    }

    if (doVersion) {
        guardNoArgFlagCommands();
        std::cout << "bear16 version " + version << "\n";
    }

    if (doDoctor) {
        restoreDefaultConfigFile();
        std::string endOfSentence =
            buildBear16DirsIfDNE() ? " and rebuilt bear16 projects and disks directories." : ".";
        saveEmuStateToConfigFile();
        std::cout << "restored " << CONFIG_FILE_NAME << " to defaults" << endOfSentence << "\n";
    }

    return exitCode;
}

[[noreturn]] void Bear16::enumerateErrorsAndTerminate(const std::unordered_set<cli_error_e>& errors,
                                                      int exitCode) {
    std::cerr << "Bear16 found critical errors. \n";
    for (const auto& error : errors) {
        std::cerr << " -> " << errMsgMap.at(error) << '\n';
    }
    std::exit(exitCode);
}

int Bear16::runMentionedExecutable(const std::string& executableFileName,
                                   bool dumpStateAtTermination) {
    std::vector<uint8_t> userRom{};

    // init emulated system
    Board board(false);
    board.loadRomFromBinFile(executableFileName);
    board.loadDiskFromBinFile(diskPath.string());
    // run
    LOG_ERR("Launching the Bear16 Emulator...");
    int exitCode = board.run();

    // save disk state
    board.saveDiskToBinFile(diskPath.string());

    // display diagnostics
    if (dumpStateAtTermination) {
        board.printDiagnostics();
    }
    printProcessTerminationMsg(exitCode);
    return exitCode;
}
void Bear16::resetDisk() {
    std::cout << "Clear current disk (" << diskPath << ")?\n";
    std::cout
        << "-> This action is permanent, so back up the disk if you care about its contents. \n";
    std::cout << "[y/n] \n";
    std::string choice;
    std::getline(std::cin, choice);
    std::ranges::transform(choice, choice.begin(), [](char c) { return std::tolower(c); });
    if (choice != "y") {
        return;
    }
    std::vector emptyDiskData(isa::DISK_SIZE, 0);
    writeToFile(diskPath.string(), emptyDiskData);
    std::cout << "Cleared current disk. \n";
}
void Bear16::enterTUI() {
    getEmuStateFromConfigFile();
    auto printInvalidChoice = []() { std::cout << "Invalid choice. Please try again." << "\n"; };
    std::string choice;
    bool hasLaunchedEmu = false;
    do {
        printTUIMainMenu();
        std::getline(std::cin, choice);
        std::ranges::transform(choice, choice.begin(), [](char c) { return std::tolower(c); });
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
            assembleAndSaveExecutable(computeDefaultExecutablePath(build_type::release));
            break;
        }
        case '2': {
            runSavedExecutable(build_type::release);
            hasLaunchedEmu = true;
            break;
        }
        case '3': {
            assembleAndSaveExecutable(computeDefaultExecutablePath(build_type::release));
            runSavedExecutable(build_type::release);
            hasLaunchedEmu = true;
            break;
        }
        case '4': {
            assembleAndSaveExecutable(computeDefaultExecutablePath(build_type::debug));
            break;
        }
        case '5': {
            runSavedExecutable(build_type::debug);
            hasLaunchedEmu = true;
            break;
        }
        case '6': {
            assembleAndSaveExecutable(computeDefaultExecutablePath(build_type::debug));
            runSavedExecutable(build_type::debug);
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
            std::cout << helpMessageTui;
            enterToContinue();
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
void Bear16::printTUIMainMenu() {
    size_t LINE_LEN = 80;
    size_t FORMAT_LINE_LEN = LINE_LEN - 4;
    std::string emuTitle =
        std::format("| {:^76} |", std::string("BEAR16 Emulator & Assembler - v" + version + " (" +
                                              dateOfLastVersion + ")"));
    std::string author = "Made by Zach Mahan";
    std::string authorLine = std::format("| {:^76} |", author);
    std::string emuTitleBar = std::string(emuTitle.size(), '=');
    std::string emuDivideBar = std::string(emuTitle.size(), '-');
    std::string emuBorderDivLine = std::format("|{}|", std::string(emuTitle.size() - 2, '-'));
    auto getProjPathSubtitle = [this]() {
        return projectPath.string().substr(projectPath.string().find_last_of('/') + 1);
    };
    std::string openProject = "Open Project: " + getProjPathSubtitle();
    std::string openProjectLine = std::format("| {:^76} |", openProject);
    std::string fullPath = std::format("| {:^76} |", "(" + projectPath.string() + ")");
#define PRINT_TITLE_BAR() std::cout << emuTitleBar << "\n"
#define PRINT_DIVIDE_BAR() std::cout << emuDivideBar << "\n"
#define PRINT_BORDER_DIV_LINE() std::cout << emuBorderDivLine << "\n"

    PRINT_TITLE_BAR();
    std::cout << emuTitle << "\n";
    std::cout << authorLine << "\n";
    PRINT_TITLE_BAR();
    std::cout << openProjectLine << "\n";
    if (fullPath.length() <= LINE_LEN) {
        std::cout << fullPath << "\n";
    }
    PRINT_TITLE_BAR();
    std::cout << std::format("| {:<76} |", "[1] Assemble Project (Release)") << "\n";
    std::cout << std::format("| {:<76} |", "[2] Run Executable   (Release)") << "\n";
    std::cout << std::format("| {:<76} |", "[3] Assemble & Run   (Release)") << "\n";
    PRINT_BORDER_DIV_LINE();
    std::cout << std::format("| {:<76} |", "[4] Assemble Project (Debug)") << "\n";
    std::cout << std::format("| {:<76} |", "[5] Run Executable   (Debug)") << "\n";
    std::cout << std::format("| {:<76} |", "[6] Assemble & Run   (Debug)") << "\n";
    PRINT_BORDER_DIV_LINE();
    std::cout << std::format("| {:<76} |", "[P] Open a Different Project") << "\n";
    std::cout << std::format("| {:<76} |", "[C] Configure") << "\n";
    std::cout << std::format("| {:<76} |", "[H] Help") << "\n";
    std::cout << std::format("| {:<76} |", "[Q] Quit") << "\n";
    PRINT_DIVIDE_BAR();
    std::cout << "Make a selection: ";
}
bool Bear16::assembleAndSaveExecutable(const std::filesystem::path& executablePath) {
    testAssembler.openProject(projectPath, entryFileName);
    auto userRom = testAssembler.assembleOpenedProject();
    std::filesystem::path executableParentPath = executablePath.parent_path();
    if (!std::filesystem::exists(executableParentPath)) {
        std::error_code errorCode;
        std::filesystem::create_directories(executableParentPath, errorCode);
        if (errorCode) {
            std::cout << "Failed to build path to specified executable\n";
        }
    }
    std::ofstream executableFile(executablePath, std::ios::binary);
    if (!executableFile) {
        std::cerr << "Failed to open output file: " << executablePath << "\n";
        return false;
    }

    executableFile.write(reinterpret_cast<const char*>(userRom.data()),
                         static_cast<long>(userRom.size()));
    if (!executableFile) {
        std::cerr << "Failed to write to file: " << executablePath << "\n";
        return false;
    }
    executableFile.flush();
    executableFile.close();

    std::cout << "Executable saved to " << executablePath << "\n";
    enterToContinue();
    return true;
}
void Bear16::runSavedExecutable(Bear16::build_type buildType) {
    std::vector<uint8_t> userRom{};

    // init emulated system
    Board board(false);
    board.loadRomFromBinFile(computeDefaultExecutablePath(buildType).string());
    board.loadDiskFromBinFile(diskPath.string());
    // run
    LOG_ERR("Launching the Bear16 Emulator...");
    int exitCode = board.run();

    // save disk state
    board.saveDiskToBinFile(diskPath.string());

    // display diagnostics only when we're debugging
    if (buildType == Bear16::build_type::debug) {
        board.printDiagnostics();
    }
    printProcessTerminationMsg(exitCode);
}

void Bear16::enterConfigMenu() {
    auto printInvalidChoice = []() { std::cout << "Invalid choice. Please try again." << "\n"; };
    std::string line;
    do {
        printConfigMenu();
        std::cout << "Make a selection: ";
        std::getline(std::cin, line);
        std::ranges::transform(line, line.begin(), [](char c) { return std::tolower(c); });

        if (line.length() != 1) {
            printInvalidChoice();
            enterToContinue();
            continue;
        }
        char selection = line.at(0);
        switch (selection) {
        case ('1'): {
            getProjectPathFromUser();
            break;
        }
        case ('2'): {
            getEntryFromUser();
            break;
        }
        case ('3'): {
            getDiskPathFromUser();
            break;
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

void Bear16::printConfigMenu() {
    using namespace std;
    cout << "\n";
    cout << " Enter a [#] to make a change" << "\n";
    cout << "==============================" << "\n";
    cout << " [1] Project Directory:     " << projectPath << "\n";
    cout << " [2] Entry .asm file:       \"" << entryFileName << "\"\n";
    cout << " [3] Disk Path:             " << diskPath << "\n";
    cout << " NOTE: You can also edit\n";
    cout << "       the config manually in your\n";
    cout << "       \".b16.json\" file.\n";
    cout << " [C] Cancel" << "\n";
}

bool Bear16::buildBear16DirsIfDNE() {
    bool atLeastOneDirDNE = false; // return val flag for potential logging
    const std::filesystem::path diskDir = diskPath.parent_path();
    if (!std::filesystem::exists(diskDir)) {
        atLeastOneDirDNE = true;
        std::error_code errorCode;
        if (std::filesystem::create_directories(diskDir, errorCode)) {
            // success, log here?
        } else {
            if (errorCode) {
                std::cerr << "Error creating Bear16 disk directory: " << errorCode.message()
                          << "\n";
            }
        }
    }
    if (!std::filesystem::exists(projectPath)) {
        atLeastOneDirDNE = true;

        std::error_code errorCode;
        if (std::filesystem::create_directories(projectPath, errorCode)) {
            // success, log here?
        } else {
            if (errorCode) {
                std::cerr << "Error creating Bear16 project directory: " << errorCode.message()
                          << "\n";
            }
        }
    }
    return atLeastOneDirDNE;
}

void Bear16::restoreDefaultEmuState() {
    projectPath = DEFAULT_PROJECT_PATH;
    entryFileName = DEFAULT_ENTRY_FILE;
    diskPath = DEFAULT_DISK_PATH;
}

void Bear16::restoreDefaultConfigFile() {
    restoreDefaultEmuState();
    saveEmuStateToConfigFile(); // saves the restored defaults
}

std::filesystem::path Bear16::computeDefaultExecutablePath(Bear16::build_type buildType) const {
    std::string buildDir = (buildType == Bear16::build_type::release) ? "build" : "debug";

    // compute the output binary path
    std::filesystem::path executableName =
        projectPath.parent_path()
            .filename(); // gets project name from dir name, TODO: may be implementation defined
    std::filesystem::path executablePath =
        projectPath / buildDir / (executableName.string() + ".bin");
    return executablePath;
}

void Bear16::saveEmuStateToConfigFile() {
    try {
        nlohmann::json jsonObject{};
        jsonObject["projectPath"] = projectPath;
        jsonObject["entry"] = entryFileName;
        jsonObject["diskPath"] = diskPath;

        std::ofstream outStream(std::filesystem::path(USER_HOME_DIR / CONFIG_FILE_NAME));
        if (!outStream) {
            std::cerr << "ERROR: Could not open config file." << "\n";
            return;
        }
        outStream << jsonObject.dump(4);
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Could not save to config file: " << e.what() << "\n";
    }
}

void Bear16::getEmuStateFromConfigFile() {
    std::filesystem::path CONFIG_PATH = std::filesystem::path(USER_HOME_DIR / CONFIG_FILE_NAME);
    try {
        if (!std::filesystem::exists(CONFIG_PATH)) {
            std::cout << "Created default config file at \"" << CONFIG_PATH.string() << "\".\n";
            restoreDefaultConfigFile();
            enterToContinue();
        }
        std::ifstream inStream(CONFIG_PATH);
        if (!inStream) {
            std::cerr << "ERROR: Could not open config file." << "\n";
            return;
        }
        nlohmann::json jsonObject{};
        inStream >> jsonObject;
        projectPath = std::filesystem::path(jsonObject["projectPath"]);
        entryFileName = jsonObject["entry"];
        diskPath = std::filesystem::path(jsonObject["diskPath"]);
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Could not read config file: " << e.what() << "\n";
    }
}
void Bear16::getProjectPathFromUser() {
    std::string projectDirName;
    std::filesystem::path projectsParentDir =
        projectPath.parent_path().parent_path(); // TODO might be implementation defined, funky
                                                 // behavior likely caused by trailing backslash
    std::cout << "Enter the name of the project directory: " << "\n";
    std::cout << projectsParentDir.string() << '/';
    std::getline(std::cin, projectDirName);
    if (projectDirName.empty()) {
        std::cout << "ERROR: Project path cannot be empty." << "\n";
        enterToContinue();
        return;
    }
    std::filesystem::path potentialnewProjectPath(projectsParentDir / projectDirName);

    auto saveChanges = [this, &potentialnewProjectPath, &projectDirName]() {
        this->projectPath = potentialnewProjectPath;
        std::cout << "Project path set to: " << projectDirName << "\n";
        saveEmuStateToConfigFile();
        enterToContinue();
    };

    if (!std::filesystem::exists(potentialnewProjectPath)) {
        std::cout << "Project path does not exist: " << potentialnewProjectPath << "\n";
        bool madeValidChoice = false;
        do {
            std::cout << " [N] Create new project directory" << "\n";
            std::cout << " [C] Cancel" << "\n";
            std::cout << "Make a selection: ";
            std::string choice;
            std::getline(std::cin, choice);
            if (!(choice == "c" || choice == "C" || choice == "n" || choice == "N")) {
                std::cout << "Invalid choice. Please try again." << "\n";
            }
            if (choice == "n" || choice == "N") {
                std::filesystem::create_directory(potentialnewProjectPath);
                std::cout << "Project directory created at " << potentialnewProjectPath.string()
                          << "\n";
                madeValidChoice = true;
                saveChanges();
            } else if (choice == "C" || choice == "c") {
                madeValidChoice = true;
            }
        } while (!madeValidChoice);
        return;
    }
    saveChanges();
}

void Bear16::getDiskPathFromUser() {
    std::string diskFileName;
    std::filesystem::path diskParentDir(diskPath.parent_path());
    std::cout << "Enter the name of the disk: " << "\n";
    std::cout << diskParentDir.string() << '/';
    std::getline(std::cin, diskFileName);
    if (diskFileName.empty()) {
        std::cout << "ERROR: Disk cannot be empty." << "\n";
        enterToContinue();
        return;
    }
    if (!diskFileName.ends_with(BIN_SUFFIX)) {
        std::cout << "ERROR: Disk must be a \".bin\" file." << "\n";
        enterToContinue();
        return;
    }
    std::filesystem::path potentialNewDiskPath(diskParentDir / diskFileName);
    if (!std::filesystem::exists(potentialNewDiskPath)) {
        std::fstream createFileStream(potentialNewDiskPath, std::ios::out);
        if (!createFileStream) {
            std::cout << "ERROR: Failed to create new disk file \"" << potentialNewDiskPath
                      << "\"\n";
            enterToContinue();
            return;
        }
        std::cout << "Created new disk file \n";
    }
    this->diskPath = potentialNewDiskPath;
    std::cout << "Disk set to: \"" << diskFileName << "\"\n";
    saveEmuStateToConfigFile();
    enterToContinue();
}

void Bear16::getEntryFromUser() {
    std::string entryFileName;
    std::cout << "Enter the name of the entry file: " << "\n";
    std::cout << projectPath.string();
    std::getline(std::cin, entryFileName);
    if (entryFileName.empty()) {
        std::cout << "ERROR: Entry file name cannot be empty." << "\n";
        enterToContinue();
        return;
    }
    if (!entryFileName.ends_with(ASM_SUFFIX)) {
        std::cout << "ERROR: Entry must be a \".asm\" file." << "\n";
        enterToContinue();
        return;
    }
    std::filesystem::path entryFilePath(projectPath / entryFileName);
    if (!std::filesystem::exists(entryFilePath)) {
        std::cout << "ERROR: Specified entry file does not exist\n";
        enterToContinue();
        return;
    }
    this->entryFileName = entryFileName;
    std::cout << "Entry set to: \"" << entryFilePath.string() << "\"\n";
    saveEmuStateToConfigFile();
    enterToContinue();
}

void Bear16::enterToContinue() {
    std::cout << "[ENTER] to continue" << "\n";
    std::cin.get();
}

void Bear16::printProcessTerminationMsg(int exitCode) {
    std::cout << "Emulated process (bear16 version " + version + ") finished with exit code "
              << exitCode << "\n";
    std::cout << "\n";
}
