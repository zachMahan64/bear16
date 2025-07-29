#include "emu.h"

int main(int argc, char *argv[]) {
    int exitCode = 0;
    emu_launch launchState = (argc == 1) ? emu_launch::tui : emu_launch::cli_args;
    Emulator emu{launchState};
    emu.launch(argc, argv);
    return exitCode;
}
