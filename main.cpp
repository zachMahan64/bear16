#include <iostream>
#include "assembly.h"
#include "emu.h"

int main() {
    //assembly::asmMnemonicSetGenerator();
    emu::Emulator emulator {};
    int exitCode = emulator.run();
    return exitCode;
}
