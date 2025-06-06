#include <iostream>
#include "assembly.h"
#include "emu.h"

int main() {
    emu::Emulator emulator {};
    int exitCode = emulator.run();
    return exitCode;
}
