cmake_minimum_required(VERSION 3.31)
project(cpu_emulator_16-bit_custom_isa)

set(CMAKE_CXX_STANDARD 20)

add_executable(cpu_emulator_take_1
        main.cpp
        ISA.h
        parts.cpp
        parts.h
        core.cpp
        core.h
        assembler.cpp
        assembler.h
)

