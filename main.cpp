#include <iostream>
#include "assembly.h"
#include "emu.h"
int main(int argc, char* argv[]) {
    //assembly::asmMnemonicSetGenerator();
    emu::Emulator emulator {};
    std::string projectPath = "../projects/bear16_os/";
    std::string entry = "main.asm";
    std::string diskPath = "../disks/disk.bin";
    int exitCode = emulator.assembleAndRun(projectPath, entry, diskPath);
    return exitCode;
}
