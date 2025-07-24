//
// Created by Zachary Mahan on 6/4/25.
//
#ifndef EMU_H
#define EMU_H
#include <string>
#include "assembly.h"
#include "json.hpp"
class Emulator {
    //version info
    const std::string version = "0.0.5";
    const std::string dateOfLastVersion = "2025-07-19";
    //emu state
    std::filesystem::path projectPath = "../projects_b16/bear16_os";
    std::string entryFileName = "main.asm";
    std::filesystem::path diskPath = "../disks_b16/default_disk.bin";
    //assembler
    assembly::Assembler testAssembler{false, false};
public:
    int assembleAndRunWithoutSavingExecutable();


    //main menu
    void enterTUI();
    void printTUIMainMenu();
    //submenus/methods
    void assembleAndSaveExecutable();
    void runSavedExecutable();
    //config menu
    void enterConfigMenu();
    void printConfigMenu();
    //help menu
    void printHelpMessage();
    //helpers
    [[nodiscard]] std::filesystem::path computeBinPath() const;
    void saveEmuStateToConfigFile();
    void getEmuStateFromConfigFile();
    //getting input from user
    void getProjectPathFromUser();
    void getEntryFromUser();
    void getDiskPathFromUser();
    void enterToContinue();
};

#endif //EMU_H
