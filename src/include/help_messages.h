#ifndef HELP_MESSAGES_H
#define HELP_MESSAGES_H

constexpr const char* helpMessageCli =
    R"HELP(options:
  -a, --assemble, assemble       assemble source file(s)
  -r, --run, run                 run the specified disk image
  -h, --help, help               show help message
  -v, --version                  show version information
  -u, --ui, --tui                start terminal user interface
  -ar, -ra                       assemble and run
  -sd, --set-disk                set the active disk image
  -cd, --check-disk              check the path of the current disk
  -rd, --reset-disk-disk         reset/zero the contents of the current disk
  doctor, --doctor               restore ".b16.json" config file and rebuild
                                 disks/projects directories if missing
  <no-args>                      start termnial user interface
                                 *note: all args and flags are order-agnostic

usage: b16 [--help | -h]
       b16 [--assemble | assemble | -a] <assembly_file> <target_binary_file>
       b16 [--run | run | -r] <binary_file>
       b16 [-ar | -ra] <assembly_file> <binary_file>
       b16 [--ui | --tui]
       b16 -sd
       b16 -cd
       b16 -rd
       b16 [--version | -v]
       b16 [doctor | --doctor]
       b16 <no-args>

example flows: ~ project set-up ~
               cd ~/bear16/projects_b16/
               mkdir my_project
               cd my_project
               touch main.asm
               touch file_to_be_included.asm  # make sure to include this in your entry file!
               nvim main.asm                  # edit in neovim (or your editor of choice)!
               mkdir build

               ~ assemble and run ~
               b16 assemble main.asm build/my_executable.bin
               b16 run my_executable.bin
               ~~~~~~~~ or ~~~~~~~~
               b16 --assemble main.asm --run build/my_executable.bin
               ~~~~~~~~ or ~~~~~~~~
               b16 -ar my_project_entry.asm my_executable.bin
)HELP";

constexpr const char* helpMessageTui =
    R"HELP(                                               THANK YOU FOR USING BEAR16
========================================================================================================================
WHAT TO DO IN THE BEAR16 TERMINAL USER INTERFACE
------------------------------------------------
- You can use the assembler and emulator to run:
    - Standalone programs
    - Apps for the Bear16 OS (you must static include any bear16 os files in your project)
    - Write your own OS for the system (you could reuse my util library to streamline the process!)

MAKING A NEW PROJECT
--------------------
    1.) Create a new directory inside `projects_b16` or another location if you prefer and set it as your open project
    2.) Create at least one `.asm` file inside the root of your project to be used as your entry point and set it as
        such in the project config.
    3.) Create and link as many `.asm` files as you want (use the` @include` directive; see README for details). Write
        all paths relative to project root.
    4.) Make sure to follow the opcode syntax in the ISA spreadsheet and the directive syntax section in the README.
    5.) The assembler catches all syntax mistakes, but it cannot catch logical errors.

MORE INFO
---------
- See the README and GitHub repo for:
    - ISA reference
    - Assembly syntax
    - Memory-mapped IO implementation details
    - Emulator & assembler source code
    - Bear16 OS breakdown
    - Example assembly programs and the Bear16 OS source code
    → Docs on: https://github.com/zachMahan64/bear16
)HELP";

#endif // HELP_MESSAGES_H
