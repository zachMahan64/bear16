cmake_minimum_required(VERSION 3.31)
project(cpu_emulator_16-bit_custom_isa)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

set(CMAKE_FRAMEWORK_PATH "${CMAKE_SOURCE_DIR}")

add_executable(bear16
        main.cpp
        parts.cpp
        core.cpp
        assembly.cpp
        emu.cpp
        preprocess.cpp
        expr_resolver.cpp
)
if(APPLE)
    target_include_directories(bear16 PRIVATE "${CMAKE_SOURCE_DIR}/libs/SDL2.framework")

    target_link_directories(bear16 PRIVATE "${CMAKE_SOURCE_DIR}/libs")

    target_link_libraries(bear16 "${CMAKE_SOURCE_DIR}/libs/SDL2.framework/Versions/A/SDL2")
elseif(WIN32)
    # paths
    set(SDL2_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/libs/SDL2-2.32.8/include")
    set(SDL2_LIB_DIR "${CMAKE_SOURCE_DIR}/libs/SDL2-2.32.8/lib/x86")
    set(SDL2_DLL "${SDL2_LIB_DIR}/SDL2.dll")

    target_compile_definitions(bear16 PRIVATE SDL_MAIN_HANDLED) # as to not override main()

    target_include_directories(bear16 PRIVATE "${SDL2_INCLUDE_DIR}")
    target_link_directories(bear16 PRIVATE "${SDL2_LIB_DIR}")

    target_link_libraries(bear16 PRIVATE SDL2 SDL2main)

    # copy SDL2.dll next to the output binary after build
    add_custom_command(TARGET bear16 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SDL2_DLL}"
            "$<TARGET_FILE_DIR:bear16>")
endif()