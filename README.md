# Bear16: A Custom, Fully Featured 16-Bit ISA, Assembler, Emulator, & OS
## Set-up
### Linux:
#### Installing SDL2 packages:
- Fedora:        `sudo dnf install SDL2-devel`
- Debian/Ubuntu: `sudo apt install libsdl2-dev`
- Arch:          `sudo pacman -S sdl2`
- `git clone --depth=1 https://github.com/zachMahan64/bear16.git ~/dev/cpp/bear16` or wherever you want the project to live
### macOS:
- `git clone --depth=1 https://github.com/zachMahan64/bear16.git ~/dev/cpp/bear16` or wherever
### Windows
- `git clone --depth=1 https://github.com/zachMahan64/bear16.git %USERPROFILE%/dev/cpp/bear16` or wherever
### All Platforms
- **You should export the b16 executable's parent directory to your path for proper CLI support (where your b16 executable lives, like a build dir).**
## Build Instructions
- **Buildable with CMake** directly after cloning the GitHub repo.
- **Platform support:** Linux, MacOS, and Windows.
- **Dependencies:**
  - CMake >= v3.10
  - Make or Ninja
  - C++23 compiler:
     - Linux & MacOS: gcc or clang
     - Windows: MinGW
  - SDL2 (for framebuffer and input)
  - Nlohmann (header-only inside the repo)
### Linux & MacOS
- `cd path/to/bear16`
- `mkdir build`
- `cd build`
- `cmake ..`
- `make`
- `echo 'export PATH="$HOME/path/to/bear16/build:$PATH"' >> ~/.bashrc` or `.zshrc` if you're using zsh instead of bash
- `source ~/.bashrc` or `.zshrc`
### Windows (Msys2)
- Visit: `https://www.msys2.org/`, then download and run the Msys2 setup exe
- Open Msys2 once installed
- `pacman -Syu` and restart when prompted
- `pacman -Su`
- `pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-cmake`
- `cd /c/path/to/bear16`
- `mkdir build`
- `cd build`
- `cmake -G "MinGW Makefiles" ..`
- `mingw32-make`
- `echo 'export PATH="$PWD:$PATH"' >> ~/.bashrc`
- `source ~/.bashrc`
## Using Bear16: Some Example Workflows
- (...)
## Technical Overview
### Background
- I built Bear16 after hundreds of hours of labor. This was an exploratory, self-directed project made during the summer before my freshman year of college.
    - The OS is effectively a monolithic kernel with fully featured utility libraries, a shell/CLI, file IO, keyboard IO,
      framebuffering, blitting, and memory allocator.
    - The VM and Assembler are written in modern C++ 23.
    - All programming for the Bear16 system must be done in raw assembly.
    - The Bear16 architecture is inspired by x86's expressiveness and RISC-V's simplicity, although it deviates in its syntax, directives, and instruction layout.
    - Bear16 is a pure Harvard architecture in which ROM and RAM occupy separate address spaces. This
      was done as a design choice largely to expand system memory given 16-bit address space constraints without
      the need for banking or wider-than-16-bit register addressing.
    - The VM has been optimized heavily and can run well over 100 MHz real-time, although this is, of course, overkill and
      it has been throttled to 40 MHz. The VM operates at the RTL and is cycle-accurate.
    - Bear16 was primarily an educational endeavor for me, though the entire toolchain is usable by anyone.
### ISA and Assembly Language Details
- Full ISA spreadsheet available [here](https://docs.google.com/spreadsheets/d/1skLFHBtt_hR7RHbrW7IGVIHvV-CCc16sBRJoc0tyrCA/edit?usp=sharing)
- Bear16 is a 16-bit, little-endian architecture with 16-bit memory addressing and 64-bit fixed-width instructions.
- Instruction format:
  - Each instruction is broken up into four distinct 16-bit fields
    - `{16} {16} {16} {16}`
    - `{opcode} {destination} {source_one} {source_two}`
    - Each opcode contains two sub-fields:
      - `{opcode}`
      - `{2} {14}`
      - {immediate_flags}{instruction_code}
        - The immediate flags can be 00, 01, 10, and 11
        - Bit 15 (MSB) indicates that source_one is an immediate; bit 14 indicates the same for source_two.
        - This can be specified manually in text by `{op}{imm_suffix}` where the suffix can either be absent (00), i1 (10), i2 (01), or i (11).
        - The immediate system allows for full 16-bit immediate inlining which makes code much more concise, at the cost of each instruction being wide (8 bytes!). I will discuss this design decision more in the reflection section.
  - Example Instruction:
    - `add t0, t1, 2`
    - Explanation: This instruction adds the value stored in register t1 (temporary/volatile 1) and stores the result into t0. Notice how the source_two field contains the immediate value 2. So this instruction really should be: `addi2 t0, t1, 2`. Thankfully, the assembler recognizes this necessity and resolves all inlined immediates. Manual suffixing (`i1`, `i2`, `i`) is entirely optional and generally discouraged.

- Directives:
  - `@include "relative_path_if_in_sub_directory/file_name.asm"`
    - For C-style linking
  - .data
    - Denotes constant data
  - .text
    - Denotes executable code
  - `.const CONSTANT_NAME = {SOME VALUE}`
      - Value must evaluate to a 16-bit quantity. Supported types:
          - Signed/unsigned integers
          - 8.8 fixed-point numbers
          - ASCII characters
          - Pointers or label references
          - Other constants (if forward-defined and file-local)
      - Expressions are allowed, but:
          - Must be wrapped in parentheses
          - Must resolve to one of the above value types
  - Expressions:
    - Must always be wrapped in parentheses
    - Can be used when defining constants or directly inlined in assembly instructions
    - Can contain labels or constants

### Memory Layout: Stack, Heap, and Memory Mapped IO
- ROM Layout: 64 KB
    - After assembly, the executables layout will be in this order:
    - `[0x0000: entry_point]`
    - `[<varies>: combined_text_section]` → 8-byte aligned
    - `[<varies>: combined_data_section]` → Byte aligned
- RAM Layout: 64 KB
    - `[0x0000 - 0x17FF: framebuffer]`
      - 6 KB
    - `[0x1800 - 0x19FF: privileged_space]`
      - 2 KB
      - Reserved for memory mapped IO
      - See `isa.h` or `bear16_os` for details
    - `[0x1800 - 0x9FFF: heap]`
      - 32 KB
    - `[0xA000-0xFFFF: stack]`
      - 24 KB

## The Bear16 Design Process and a Toolchain Overview

### A Cycle-Accurate RTL Emulator, Preprocessor, Recursive Linker, and Multi-pass Assembler
    The design philosophy behind Bear16 is one of maximizing speed. The emulator, despite being cycle-accurate, can run well above 100 MHz in real-time. However, it has been calibrated to target ~36-40 MHz. The code is Object-Based, following principles of encapsulation, but uses composition with zero inheritance for maximum runtime speed. The general structure of b16 is as follows:
- [bear16 interface](src/include/bear16.h)
    - controlled via CLI arguments or the TUI (a click-and-run experience that runs in the terminal).
    - owns: assmebler, board
- [assembler](src/include/assembly.h)
    - assembles .asm files (links included files recursively -> create an executable binary compliant with the Bear16 ISA)
    - owns preprocessor for linking
    - assembles in a multi-pass fashion: tokenize -> break into text & data sections -> resolve constant expressions -> resolve references to labels and constants -> convert into binary that Bear16 CPU understands!
- [board](src/include/core.h)
    - owns a CPU16: the CPU core
    - owns memory and disk (ram and rom are std::arrays and disk is an std::vector of uint8_t's)
    - owns a Screen (wrapper around an SDL2 window with framebuffer rendering functionality)
    - owns a DiskController & a InputController (memory-mapped IO controllers)
    - all subcomponents of Board have non-owning views into memory/disk
    - all objects follow RAII and everything lives on stack (for maximized cache-locality) besides the SDL2 window and the disk (an std::vector).
    - heres the simplified C++/pseudo-code for the Board's main method ("run"):
```cpp
int Board::run() {
    int exitCode = 0;
    // init clock & SDL2 timings
    STEPS_PER_LOOP = ... enough to target 36'000'000 Hz;
    SDL_Event event;
    clock.resetCycles(); // set clock cycles to zero @ the start of a new process
    clock.initMemMappedTime();

    auto startTime = currentTimeMillis(); // for calculating clock speed after running ends
    constexpr double TARGET_FRAME_TIME = 1.0 / 60.0; // 60 FPS,
    uint64_t lastFrameTime = SDL_GetPerformanceCounter();
    const uint64_t freq = SDL_GetPerformanceFrequency();

    do {
        while (SDL_PollEvent(&event)) {
            ... poll for an event, like a keypress
        }
        for (int i = 0; i < STEPS_PER_LOOP; ++i) {
            cpu.step();
            clock.tick();
            if (cpu.isHalted) {
                break;
            }
        } // this inner loop lets the cpu wait less often for (slow) SDL2 polling

        diskController.handleDiskOperation();

        const uint64_t now = SDL_GetPerformanceCounter();
        const double elapsed = static_cast<double>(now - lastFrameTime) / static_cast<double>(freq);

        if (elapsed >= TARGET_FRAME_TIME) {
            ... render framebuffer to SDL2 window & update memory-mapped time
        }
    } while (!cpu.isHalted);

    // calculate some clock stats
    auto endTime = currentTimeMillis();
    auto elapsedMillis = static_cast<double>(endTime - startTime);
    calcClockSpeedHz(elapsedMillis);

    return exitCode;
}
```
- [cpu16](core.h)
    - main interface through the "step" method:
```cpp
void CPU16::step() {
    // fetch & decode
    LOG("DEBUG: PC = " << pc);
    parts::Instruction instr(fetchInstruction());
    // execute & writeback
    execute(instr);
    if (!pcIsFrozenThisCycle) {
        pc += 8;
    }
    pcIsFrozenThisCycle = false;
}
```

### Bear16: A Reflection
*(To be added soon.)*
