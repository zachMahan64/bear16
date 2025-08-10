//
// Created by Zachary Mahan on 6/4/25.
//
#ifndef EMU_H
#define EMU_H
#include "assembly.h"
#include "cli_args.h"
#include "path_manager.h"
#include <filesystem>
#include <string>
#include <vector>
enum class emu_launch { cli_args, tui };
class Emulator {
    // version info & constant state
    const std::string version = "0.0.6";
    const std::string dateOfLastVersion = "2025-08-09";
    const emu_launch launchState;
    // PATHING
    std::filesystem::path bear16RootDir = getBear16DefaultRootDir();
    std::filesystem::path projectPath = bear16RootDir / "projects_b16" / "bear16_os";
    std::string entryFileName = "main.asm";
    std::filesystem::path diskPath = bear16RootDir / "disks_b16" / "default_disk.bin";
    // user data paths
    const std::filesystem::path TUI_PATH = bear16RootDir / "tui";
    const std::filesystem::path HELP_MESSAGE = "help_message.txt";
    const std::filesystem::path CONFIG = "config.json";
    // assembler
    assembly::Assembler testAssembler{};
    // CLI
    int performActionBasedOnArgs(const std::vector<std::string> &args);
    int runMentionedExecutable(const std::string &executableFileName);
    [[noreturn]] static void
    enumerateErrorsAndTerminate(const std::unordered_set<cli_error_e> errors,
                                int exitCode = EXIT_FAILURE);
    // TUI
    int assembleAndRunWithoutSavingExecutable();
    // main menu
    void enterTUI();
    void printTUIMainMenu();
    // submenus/methods
    void assembleAndSaveExecutable(std::filesystem::path executablePath);
    void runSavedExecutable();
    // config menu
    void enterConfigMenu();
    void printConfigMenu();
    // help menu
    void printHelpMessage();
    // helpers
    [[nodiscard]] std::filesystem::path computeDefaultExecutablePath() const;
    void saveEmuStateToConfigFile();
    void getEmuStateFromConfigFile();
    std::filesystem::path snipBear16RootDir(const std::filesystem::path &path);
    // getting input from user
    void getProjectPathFromUser();
    void getEntryFromUser();
    void getDiskPathFromUser();
    void enterToContinue();

  public:
    explicit Emulator(emu_launch launchState);
    int launch(int argc, char **argv);
};

#endif // EMU_H
