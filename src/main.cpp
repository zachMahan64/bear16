#include "emu.h"
int main(int argc, char* argv[]) {
    emu_launch launchState = (argc == 1) ? emu_launch::tui : emu_launch::cli_args;
    Emulator emu{launchState};
    return emu.launch(argc, argv);
}
