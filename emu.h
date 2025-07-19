//
// Created by Zachary Mahan on 6/4/25.
//
#ifndef EMU_H
#define EMU_H
#include <string>
class Emulator {
    //version info
    const std::string version = "0.0.3";
    //emu state
    std::string projectPath = "../projects_b16/bear16_os";
    std::string entry = "main.asm";
    std::string diskPath = "../disks/default_disk.bin";
public:
    int assembleAndRun() const;
    void enterTUI();

    void printTUIStartUp();
    void getProjectPathFromUser();
    void getEntryFromUser();
    void getDiskPathFromUser();
};

#endif //EMU_H
