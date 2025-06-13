#include <iostream>
#include "assembly.h"
#include "emu.h"

int main() {
    //assembly::asmMnemonicSetGenerator();
    emu::Emulator emulator {};
    std::string path = "../programs/asm_test.asm";
    int exitCode = emulator.assembleAndRun(path);
    return exitCode;
}
