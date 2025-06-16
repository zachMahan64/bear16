#include <iostream>
#include "assembly.h"
#include "emu.h"
#include "SDL2/SDL.h"

int main(int argc, char* argv[]) {
    //assembly::asmMnemonicSetGenerator();

    emu::Emulator emulator {};
    std::string path = "../programs/asm_test.asm";
    int exitCode = emulator.assembleAndRun(path);
    return exitCode;
}

