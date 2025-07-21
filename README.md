# Bear16: A Custom, Fully Featured 16-Bit ISA, Assembler, Emulator, and OS
## Technical Overview
### Background
- Bear16  was built by me (Zach) after hundreds of hours of labor. This was an exploratory, self-directed project made during the summer before my freshman year of college.
    - The OS is effectively a monolithic kernel with fully featured utility libraries, a shell/CLI, file IO, keyboard IO,
      framebuffering, blitting, and memory allocator.
    - The VM and Assembler are written in C++ 23.
    - All programming for the Bear16 system must be done in raw assembly.
    - The Bear16 Architecture is inspired by x86 and RISC-V, although it deviates in its syntax, directives,
      and instruction layout.
    - Bear16 is a pure Harvard architecture in which ROM and RAM occupy separate address spaces. This
      was done as a design choice largely to expand system memory given 16-bit address space constraints without
      the need for banking or wider-than-16-bit register addressing.
    - The VM has been optimized heavily and can run well over 100 MHz real-time, although this is, of course, overkill and
      it has been throttled to 40 MHz. This VM operates at the RTL and is cycle-accurate.
    - Bear16 was primarily an educational endeavor for me, though the entire toolchain is usable by anyone.
### Bear16 ISA and Assembly Language Details
- Full ISA spreadsheet available [here](https://docs.google.com/spreadsheets/d/1skLFHBtt_hR7RHbrW7IGVIHvV-CCc16sBRJoc0tyrCA/edit?usp=sharing)
- Bear16 is a 16-bit little-endian architecture with 16-bit memory addressing and 64-bit fixed-width instructions.
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
        - This can be specified manually in text by {some_op}{imm_suffix} where the suffix can either be absent (00), i1 (10), i2 (01), or i (11).
        - The immediate system allows for full 16-bit immediate inlining which makes the assembly much more concise, at the cost of each instruction being wide (8 whole bytes!). I will discuss this design decision more in the reflection section.
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

### Bear16 Memory Layout: Stack, Heap, and Memory Mapped IO
*(To be added soon.)*

## The Bear16 Design Process and a Source Code Explication

### Implementing a Cycle-Accurate RTL Emulator
*(To be added soon.)*

### Implementing a Preprocessor, Recursive Linker, and a Multipass Assembler
*(To be added soon.)*

### Designing and Implementing a Monolithic OS and Kernel
*(To be added soon.)*

### Unfinished and Unused Features
*(To be added soon.)*

### Bear16: A Reflection
*(To be added soon.)*