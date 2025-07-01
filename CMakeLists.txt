cmake_minimum_required(VERSION 3.31)
project(cpu_emulator_16-bit_custom_isa)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

set(CMAKE_FRAMEWORK_PATH "${CMAKE_SOURCE_DIR}")

add_executable(cpu_emulator_take_1
        main.cpp
        ISA.h
        parts.cpp
        parts.h
        core.cpp
        core.h
        assembly.cpp
        assembly.h
        emu.cpp
        emu.h
        preprocess.cpp
        preprocess.h
        fixpt_16.h
)

# Correctly includes SDL headers (e.g., <SDL2/SDL.h> will now work)
target_include_directories(cpu_emulator_take_1 PRIVATE "${CMAKE_SOURCE_DIR}/SDL2.framework")

# This explicitly tells the linker where to find frameworks (the directory containing SDL2.framework).
target_link_directories(cpu_emulator_take_1 PRIVATE "${CMAKE_SOURCE_DIR}")

target_link_libraries(cpu_emulator_take_1 "${CMAKE_SOURCE_DIR}/SDL2.framework/Versions/A/SDL2")
