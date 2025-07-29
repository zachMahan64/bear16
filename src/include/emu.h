//
// Created by Zachary Mahan on 6/4/25.
//
#ifndef EMU_H
#define EMU_H
#include "assembly.h"
#include <filesystem>
#include <string>
#include "path_manager.h"
enum class emu_launch {
    cli_args,
    tui
}; 
class Emulator {
    // version info
    const std::string version = "0.0.5";
    const std::string dateOfLastVersion = "2025-07-19";
    // PATHING
    std::filesystem::path bear16RootDir = getBear16RootDir();
    // emu state
    std::filesystem::path projectPath = bear16RootDir / "projects_b16" / "bear16_os";
    std::string entryFileName = "main.asm";
    std::filesystem::path diskPath = bear16RootDir / "disks_b16" / "default_disk.bin";
    // user data paths
    const std::filesystem::path TUI_PATH = bear16RootDir / "tui";
    const std::filesystem::path HELP_MESSAGE = "help_message.txt";
    const std::filesystem::path CONFIG = "config.json";
    // assembler
    assembly::Assembler testAssembler{};

  public:
    int assembleAndRunWithoutSavingExecutable();

    // main menu
    void enterTUI();
    void printTUIMainMenu();
    // submenus/methods
    void assembleAndSaveExecutable();
    void runSavedExecutable();
    // config menu
    void enterConfigMenu();
    void printConfigMenu();
    // help menu
    void printHelpMessage();
    // helpers
    [[nodiscard]] std::filesystem::path computeBinPath() const;
    void saveEmuStateToConfigFile();
    void getEmuStateFromConfigFile();
    // getting input from user
    void getProjectPathFromUser();
    void getEntryFromUser();
    void getDiskPathFromUser();
    void enterToContinue();
};

#endif // EMU_H
