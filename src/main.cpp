#include "emu.h"

// #define BOOT_DIRECTLY

#ifdef BOOT_DIRECTLY
#define LAUNCH_EMU()                                                           \
    exitCode = emulator.assembleAndRunWithoutSavingExecutable();
#else
#define LAUNCH_EMU() emulator.enterTUI();
#endif

int main(int argc, char *argv[]) {
    int exitCode = 0;
    if (argc == 1) {
    Emulator emulator{};
    LAUNCH_EMU();
    } else {
        // add CLI parsing
    }
    return exitCode;
}
