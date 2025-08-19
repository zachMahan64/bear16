# Bear16: A Custom, Fully Featured 16-Bit ISA, Assembler, Emulator, & OS
## Set-up
### Linux:
#### Installing SDL2 packages:
- Fedora:        `sudo dnf install SDL2-devel`
- Debian/Ubuntu: `sudo apt install libsdl2-dev`
- Arch:          `sudo pacman -S sdl2`
- `git clone --depth=1 https://github.com/zachMahan64/bear16.git ~/path/to/bear16`             or wherever you want the project to live
### macOS:
- `git clone --depth=1 https://github.com/zachMahan64/bear16.git ~/path/to/bear16`             ^
### Windows
- `git clone --depth=1 https://github.com/zachMahan64/bear16.git %USERPROFILE%/path/to/bear16` ^
### All Platforms
- **You should export the b16 executable's parent directory to your path for proper CLI support (where your b16 executable lives, like a build directory; directions below).**
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
### Windows (Powershell)
- (... coming soon)
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
    - heres the simplified C++ code for the Board's main method ("run"):
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

### Writing Bear16 Assembly Language

#### Some Examples
- All syntax is provided above here in the README (for directives/other assembly-time related syntax) and the instruction syntax is defined [here](https://docs.google.com/spreadsheets/d/1skLFHBtt_hR7RHbrW7IGVIHvV-CCc16sBRJoc0tyrCA/edit?usp=sharing).
- The best example is [Bear16 OS itself](https://github.com/zachMahan64/bear16_os)! It is entirely written in raw Bear16 asm lang. Check out the repo to read over the source code if you're curious.

```asm
#PROPER RECURSIVE FIBONACCI CALCULATOR, MON 20250609
.text
.const FIB_N = 5
.const STO_LOC = 4096
start:
    mov a0, FIB_N
    call fibonacci
    sw STO_LOC, rv
    hlt

fibonacci:
    # a0 = n
    le base_case, a0, 2   # if a0 <= 1 goto base_case

    push a0            # save n

    sub a0, a0, 1         # a0 = n - 1
    call fibonacci        # fib(n - 1)
    pop t0 # n -> t0
    push rv            # save result onto stack

    sub a0, t0, 2         # a0 = n - 3
    call fibonacci        # fib(n - 2)
    push rv           # save result onto stack
    pop t1 # get fib(n - 2) from stack
    pop t2 # get fib(n - 2) from stack
    add rv, t1, t2        # rv = fib(n-1) + fib(n-2)
    ret

base_case:
    mov rv, 1
    ret
```

```asm
#SIMPLE QUADRATIC SOLVER USING A LOOK-UP TABLE, SAT 20250614 (UPDATED TUE 8/19/2025)
.data
#struct: val {2-byte}, sqrt(val) {2-byte}
sqrt_table:
    .word 0 1
    .word 1 1
    .word 4 2
    .word 9 3
    # ... all the way through 16-bit unsigned integers ->
    .word 65025 255
sqrt_table_size:
    .word 256
quadratic_params:
    _a:
    .word 1
    _b:
    .word -82
    _c:
    .word 1645
str_a:
    .string "a = "
str_b:
    .string "b = "
str_c:
    .string "c = "
str_ans:
    .string "ans: "
str_and:
    .string " and "
.text
.const STO_LOC = 4096
.const PRNT_STRT_LOC = 0
start:
    call pr_params
    call solve_quad
    call pr_ans
    hlt

pr_ans:
    sb s6, ' '
    inc s6
    add s6, s6, 5
    lea t4, 26
    romcpy t4, str_ans, 5 #prnt "ans: "
    lw a0, STO_LOC
    call pr_s_num_four_dig
    romcpy s6, str_and, 5 #prnt " and "
    add s6, s6, 5
    add t0, STO_LOC, 2
    lw a0, t0
    call pr_s_num_four_dig
    sb s6, ' '
    ret

pr_params: #print into memory
    clr s6 # this will be our print address ptr
    add s6, s6, 4
    lea t4, 0
    romcpy t4, str_a, 4 #prnt "a="
    lwrom a0, _a
    call pr_s_num_four_dig
    sb s6, ' '
    add s6, s6, 4
    lea t4, 9
    romcpy t4, str_b, 4 #prnt "b = "
    lwrom a0, _b
    call pr_s_num_four_dig
    sb s6, ' '
    add s6, s6, 4
    lea t4, 18
    romcpy t4, str_c, 4 #prnt "c = "
    lwrom a0, _c
    call pr_s_num_four_dig
    sb s6, ' '
    ret

pr_s_num_four_dig: #fn(a0, &s6) = fn(val_of_num, print_loc_ptr)
    comp a0, 0
    jcond_neg _neg
fd_main:
    divs t0, a0, 1000
    add t0, t0, 48
    sb s6, t0 # #000
    inc s6
    divs t0, a0, 100
    mods t0, t0, 10
    add t0, t0, 48
    sb s6, t0 # 0#00
    inc s6
    divs t0, a0, 10
    mods t0, t0, 10
    add t0, t0, 48
    sb s6, t0 # 00#0
    inc s6
    mods t0, a0, 10
    add t0, t0, 48
    sb s6, t0 # 000#
    inc s6
    ret
_neg:
    sb s6, '-'
    neg a0, a0
    inc s6
    jmp fd_main



solve_quad:
    lwrom s0, _a
    lwrom s1, _b
    lwrom s2, _c
    call pos_sol
    lea t3, STO_LOC
    sw t3, rv
    call neg_sol
    sw t3, 2, rv
    ret
pos_sol:
    neg t0, s1 # -b
    push t0 # save -b

    mults t1, s1, s1 # b^2
    mults t2, s0, s2 # a*c
    mults t2, t2, 4 # 4ac
    sub a0, t1, t2 # b^2 - 4ac
    lt b_ret a0, 0 # terminate if sqrt is of a neg number

    call sqrt  # sqrt(a0) = sqrt(b^2 - 4ac)
    mov t1, rv #save sqrt(b^2 - 4ac) into t1
    mults t2, s0, 2 # 2a
    pop t0 #get our beloved -b back
    add t0, t0, t1 # -b + sqrt(b^2 - 4ac)

    divs t0, t0, t2 # (-b + sqrt(b^2 - 4ac)) / 2a
    mov rv, t0 #return answer
    ret #"
    b_ret:
        mov rv, -1
        ret

neg_sol:
        neg t0, s1 # -b
        push t0 # save -b

        mults t1, s1, s1 # b^2
        mults t2, s0, s2 # a*c
        mults t2, t2, 4 # 4ac
        sub a0, t1, t2 # b^2 - 4ac
        lt bn_ret a0, 0 # terminate if sqrt is of a neg number

        call sqrt  # sqrt(a0) = sqrt(b^2 - 4ac)
        mov t1, rv #save sqrt(b^2 - 4ac) into t1
        mults t2, s0, 2 # 2a
        pop t0 #get our beloved -b back
        sub t0, t0, t1 # -b - sqrt(b^2 - 4ac)

        divs t0, t0, t2 # (-b - sqrt(b^2 - 4ac)) / 2a
        mov rv, t0 #return answer
        ret #"
        bn_ret:
            mov rv, -1
            ret

sqrt:
    clr t1 # our lil counter/offsetter
    s_loop:
        lwrom t2, sqrt_table, t1 # <load from rom> t2, *sqrt_table, offset
        eq s_hit, a0, t2
        ult overshot, a0, t2
        add t1, t1, 4 # inc by 2 word
        jmp s_loop
    s_hit:
        mov s7, 7 # debug, lucky # 7, this is just a little marker to indicate a hit!
        add t1, t1, 2
        mov s3, t1 # debug
        lwrom rv, sqrt_table, t1 # <load from rom> t2, *sqrt_table, new_offset (*sqrt(val))
        ret
    overshot:
        sub t3, t1, 2
        lwrom rv, sqrt_table, t3 # <load from rom> t2, *sqrt_table, new_offset (*est_down_sqrt(val))
        ret
```

```asm
# BABY'S FIRST HELLO WORLD PRINTED TO THE FRAME BUFFER; THIS WAS THE FOUNDATION FOR STRING BLITTING CORE UTILITY IN THE BEAR16 OS
.data
fb_data_start:
.byte 0b00010000 0b10111100 0b01111100 0b10111100 0b11111110 0b11111110 0b01111110 0b10000010 0b11111110 0b01111110 0b10000010 0b01000000 0b10000010 0b10000010 0b01111100 0b10111100 0b01111100 0b10111100 0b01111100 0b11111110 0b10000010 0b10000010 0b10000010 0b10000010 0b10000010 0b11111110 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
.byte 0b00101000 0b11000010 0b10000010 0b11000010 0b10000000 0b10000000 0b10000000 0b10000010 0b00010000 0b00001000 0b10000010 0b01000000 0b11000110 0b11000010 0b10000010 0b11000010 0b10000010 0b11000010 0b10000010 0b00010000 0b10000010 0b10000010 0b10000010 0b01000100 0b10000010 0b00000100 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
.byte 0b01000100 0b10000010 0b10000000 0b10000010 0b10000000 0b10000000 0b10000000 0b10000010 0b00010000 0b00001000 0b10000100 0b01000000 0b10101010 0b10100010 0b10000010 0b10000010 0b10000010 0b10000010 0b10000000 0b00010000 0b10000010 0b10000010 0b10000010 0b00101000 0b01000100 0b00001000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
.byte 0b10000010 0b11111100 0b10000000 0b10000010 0b11111110 0b11111110 0b10001110 0b11111110 0b00010000 0b00001000 0b11111100 0b01000000 0b10010010 0b10010010 0b10000010 0b11000010 0b10000010 0b11000010 0b01111100 0b00010000 0b10000010 0b10000010 0b10010010 0b00010000 0b00101000 0b00010000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
.byte 0b11111110 0b10000010 0b10000000 0b10000010 0b10000000 0b10000000 0b10000010 0b10000010 0b00010000 0b00001000 0b10000010 0b01000000 0b10000010 0b10001010 0b10000010 0b10111100 0b10001010 0b10111100 0b00000010 0b00010000 0b10000010 0b01000100 0b10010010 0b00101000 0b00010000 0b00100000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
.byte 0b10000010 0b11000010 0b10000010 0b11000010 0b10000000 0b10000000 0b11000010 0b10000010 0b00010000 0b10001000 0b10000010 0b01000000 0b10000010 0b10000110 0b10000010 0b10000000 0b10000100 0b10000100 0b10000010 0b00010000 0b10000010 0b00101000 0b10101010 0b01000100 0b00010000 0b01000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
.byte 0b10000010 0b10111100 0b01111100 0b10111100 0b11111110 0b10000000 0b10111110 0b10000010 0b11111110 0b01110000 0b10000010 0b01111100 0b10000010 0b10000010 0b01111100 0b10000000 0b01111010 0b10000010 0b01111100 0b00010000 0b01111100 0b00010000 0b01000100 0b10000010 0b00010000 0b11111110 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
.byte 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000

my_str:
    .string " HELLO WORLD"
me_str:
    .string "      MADE BY ZACH MAHAN"
date_str_0:
    .string "      ON JUNE SIXTEENTH"
date_str_1:
    .string "      TWENTY TWENTY FIVE"


.text
.const FB_LOC = 0
.const LINE_SIZE = 256
.const TILE_MAP_SIZE = 256
.const LINE_WIDTH_B = 32
start:
    lea s0, FB_LOC
    romcpy s0, fb_data_start, TILE_MAP_SIZE #print out char tile set
    add s0, s0, LINE_SIZE

    mov a0, 2 # line
    mov a1, 0 # index
    mov a2, my_str
    call blit_strl_rom #blitting a str

    mov a0, 4 # line
    mov a1, 0 # index
    mov a2, me_str
    call blit_strl_rom #blitting a str

    mov a0, 5 # line
    mov a1, 0 # index
    mov a2, date_str_0
    call blit_strl_rom #blitting a str

    mov a0, 6 # line
    mov a1, 0 # index
    mov a2, date_str_1
    call blit_strl_rom #blitting a str
    call inf_loop # stall
    hlt

inf_loop:
    jmp inf_loop
    ret
blit_cl:
    #a0 = line, a1 = index, a2 = desired char
    mult t0, a0, LINE_SIZE # set line
    add t0, t0, a1 # set index
    sub t1, a2, 65 # get char index in rom, also ' ' works essentially by coincidence since that region of ROM is empty
    add t1, t1, fb_data_start
    clr t2 # cnt
    bcl_loop:
        lbrom t3, t1       #load byte from rom in t3
        sb t0, t3          #store byte in t3 into addr @ t0
        add t0, t0, LINE_WIDTH_B     # t0 += 32
        add t1, t1, LINE_WIDTH_B     # t1 += 32
        inc t2             # t2++
        ult bcl_loop, t2, 8 # check cnt
    ret
blit_strl_rom:
    #a0 = line, a1 = index, a2 = char*
    mov t5, a2 # current char ptr
    bstrl_rom_loop:
        lbrom a2, t5 # a2 <- *char
        eq bstrl_ret, a2, 0
        call blit_cl #reuse a0 & a1
        inc a1
        inc t5
        jmp bstrl_rom_loop
    bstrl_ret:
        ret
```
