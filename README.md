# Bear16: A Custom, Fully Featured 16-Bit ISA, Assembler, Emulator, & OS
## Set-up
### Linux:
Installing SDL2 packages:
- Fedora:        `sudo dnf install SDL2-devel`
- Debian/Ubuntu: `sudo apt install libsdl2-dev`
- Arch:          `sudo pacman -S sdl2`
Cloning the repo:
- `git clone --depth=1 https://github.com/zachMahan64/bear16.git ~/bear16`
### macOS:
- `git clone --depth=1 https://github.com/zachMahan64/bear16.git ~/bear16`
### Windows
- `git clone --depth=1 https://github.com/zachMahan64/bear16.git %USERPROFILE%/bear16`
- Note (for all platforms): You can clone the repo into any location, just make a symlink in your home dir or change the config.
- **You should export the bear16 directory to your path for proper CLI support (or whichever your b16 executable lives, like a build dir).**
## Build Instructions and Toolchain Tips
- **Buildable with CMake** directly after cloning the GitHub repo.
- **Platform support:** Linux, MacOS, and Windows.
- **Dependencies:**
  - CMake >= v3.10
  - C++23 compiler (altough C++20 should work too with a couple tweaks)
     - Linux & MacOS: Both gcc & clang work.
     - Windows: MinGW recomended (tested & working), altough MSVC should work with some manual config.
  - SDL2 (for framebuffer and input)
- **IDE Recommendation:**
- I used CLion & NeoVim, and they worked well.
## Technical Overview
### Background
- I built Bear16 after hundreds of hours of labor. This was an exploratory, self-directed project made during the summer before my freshman year of college.
    - The OS is effectively a monolithic kernel with fully featured utility libraries, a shell/CLI, file IO, keyboard IO,
      framebuffering, blitting, and memory allocator.
    - The VM and Assembler are written in modern C++ 23.
    - All programming for the Bear16 system must be done in raw assembly.
    - The Bear16 architecture is inspired by x86 and RISC-V, although it deviates in its syntax, directives, and instruction layout.
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
    - [16] [16] [16] [16]
    - [opcode] [destination] [source_one] [source_two]
    - Each opcode contains two sub-fields:
      - [opcode]
      - [2][14]
      - [immediate_flags][instruction_code]
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

### A Cycle-Accurate RTL Emulator
*(To be added soon.)*

### A Preprocessor, Recursive Linker, and Multi-pass Assembler
*(To be added soon.)*

### A Monolithic OS and Kernel
*(To be added soon.)*

### Unfinished and Unused Features
*(To be added soon.)*

### Bear16: A Reflection
*(To be added soon.)*
