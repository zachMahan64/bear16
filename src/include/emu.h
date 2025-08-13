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
    std::filesystem::path bear16RootDir = getBear16DefaultRootDir();
    std::filesystem::path projectsRootDir = "projects_b16";
    std::filesystem::path projectPath = bear16RootDir / projectsRootDir / "bear16_os";
    std::string entryFileName = "main.asm";
    std::filesystem::path disksRootDir = "disks_b16";
    std::filesystem::path diskPath = bear16RootDir / disksRootDir / "default_disk.bin";
    // user-facing data paths
    const std::filesystem::path CONFIG_ROOT = bear16RootDir / "config_b16";
    const std::filesystem::path RESOURCES_PATH = bear16RootDir / "resources";
    const std::filesystem::path HELP_MESSAGE_TUI = "help_message_tui.txt";
    const std::filesystem::path HELP_MESSAGE_CLI = "help_message_cli.txt";
    const std::filesystem::path CONFIG_FILE = "config.json";
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
    // help menu
    void printMsgFile(const std::filesystem::path& messageFileToPrint);
    // helpers
    [[nodiscard]] std::filesystem::path computeDefaultExecutablePath() const;
    void saveEmuStateToConfigFile();
    void getEmuStateFromConfigFile();
    static std::filesystem::path snipBear16RootDir(const std::filesystem::path& path);
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
