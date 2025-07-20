#include <iostream>
#include "assembly.h"
#include "emu.h"
int main(int argc, char* argv[]) {
    Emulator emulator {};
    int exitCode = 0;
    //exitCode = emulator.assembleAndRun();
    emulator.enterTUI();
    return exitCode;
}
