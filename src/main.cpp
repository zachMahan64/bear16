#include "emu.h"

// #define BOOT_DIRECTLY

#ifdef BOOT_DIRECTLY
#define LAUNCH_EMU()                                                           \
    exitCode = emulator.assembleAndRunWithoutSavingExecutable();
#else
#define LAUNCH_EMU() emulator.enterTUI();
#endif

int main(int argc, char *argv[]) {
    Emulator emulator{};
    int exitCode = 0;
    LAUNCH_EMU();
    return exitCode;
}
