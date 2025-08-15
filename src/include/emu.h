//
// Created by Zachary Mahan on 6/4/25.
//
#ifndef EMU_H
#define EMU_H
#include "assembly.h"
#include "cli_args.h"
#include "path_manager.h"
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>
enum class emu_launch : uint8_t { cli_args, tui };
class Emulator {
    // version info & constant state
    const std::string version = "0.0.6";
    const std::string dateOfLastVersion = "2025-08-09";
    const emu_launch launchState;
    // PATHING
    // CONSTANTS
    const std::filesystem::path USER_HOME_DIR = getUserHomeDir();
    const std::filesystem::path PROJECTS_DIR = "bear16_projects";
    const std::filesystem::path DISKS_DIR = "bear16_disks";
    // DEFAULTS
    const std::filesystem::path CONFIG_FILE_NAME = ".b16.json";
    const std::string DEFAULT_ENTRY_FILE = "main.asm";
    const std::string DEFAULT_PROJECT_NAME = "bear16_os";
    const std::filesystem::path DEFAULT_PROJECT_PATH =
        USER_HOME_DIR / PROJECTS_DIR / DEFAULT_DISK_NAME;
    const std::string DEFAULT_DISK_NAME = "disk0.bin";
    const std::filesystem::path DEFAULT_DISK_PATH = USER_HOME_DIR / DISKS_DIR / DEFAULT_DISK_NAME;
    // VARIABLE
    std::filesystem::path projectPath = DEFAULT_PROJECT_PATH;
    std::string entryFileName = DEFAULT_ENTRY_FILE;
    std::filesystem::path diskPath = DEFAULT_DISK_PATH;
    // assembler
    assembly::Assembler testAssembler;
    // CLI
    int performActionBasedOnArgs(const std::vector<std::string>& args);
    int runMentionedExecutable(const std::string& executableFileName);
    void clearDisk();
    [[noreturn]] static void
    enumerateErrorsAndTerminate(const std::unordered_set<cli_error_e>& errors,
                                int exitCode = EXIT_FAILURE);
    // TUI
    int assembleAndRunWithoutSavingExecutable();
    // main menu
    void enterTUI();
    void printTUIMainMenu();
    // submenus/methods
    bool assembleAndSaveExecutable(const std::filesystem::path& executablePath);
    void runSavedExecutable();
    // config menu
    void enterConfigMenu();
    void printConfigMenu();
    // helpers
    bool buildBear16DirsIfDNE();
    void restoreDefaultEmuState();
    void restoreDefaultConfigFile();
    [[nodiscard]] std::filesystem::path computeDefaultExecutablePath() const;
    void saveEmuStateToConfigFile();
    void getEmuStateFromConfigFile();
    // getting input from user
    void getProjectPathFromUser();
    void getEntryFromUser();
    void getDiskPathFromUser();
    static void enterToContinue();

  public:
    explicit Emulator(emu_launch launchState);
    int launch(int argc, char** argv);
};

#endif // EMU_H
