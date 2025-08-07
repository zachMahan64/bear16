# BUILD IS COMPATIBLE W/ LINUX, MACOS, & WINDOWS
# SEE THE BEAR16 REPO FOR DETAILS
cmake_minimum_required(VERSION 3.10)
project(b16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# ^ make sure to have a symlink in the project root dir to the
# compile_commands.json file in the build dir

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

set(CMAKE_FRAMEWORK_PATH "${CMAKE_SOURCE_DIR}")

include_directories(${CMAKE_SOURCE_DIR}/src/include)

# grab all .cpp files inside src/
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# for full compiler warnings
target_compile_options(
  ${PROJECT_NAME}
  PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wall
          -Wextra
          -Wpedantic>
          $<$<CXX_COMPILER_ID:Clang>:-Wall
          -Wextra
          -Wpedantic>
          $<$<CXX_COMPILER_ID:MSVC>:/W4>)

if(APPLE)
    target_include_directories(
    ${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/dependencies/SDL2.framework")

    target_link_directories(${PROJECT_NAME} PRIVATE
                          "${CMAKE_SOURCE_DIR}/dependencies")

    target_link_libraries(
    ${PROJECT_NAME}
    "${CMAKE_SOURCE_DIR}/dependencies/SDL2.framework/Versions/A/SDL2")
elseif(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
elseif(WIN32)
    # paths
    set(SDL2_INCLUDE_DIR
      "${CMAKE_SOURCE_DIR}/dependencies/x86_64-w64-mingw32/include")
    set(SDL2_LIB_DIR "${CMAKE_SOURCE_DIR}/dependencies/x86_64-w64-mingw32/lib")
    set(SDL2_DLL "${SDL2_LIB_DIR}/SDL2.dll")

    target_compile_definitions(${PROJECT_NAME} PRIVATE SDL_MAIN_HANDLED)
    # ^ as to not override main()

    target_include_directories(${PROJECT_NAME} PRIVATE "${SDL2_INCLUDE_DIR}")
    target_link_directories(${PROJECT_NAME} PRIVATE "${SDL2_LIB_DIR}")

    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2 SDL2main)

    # copy SDL2.dll next to the output binary after build
    add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL2_DLL}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
endif()
