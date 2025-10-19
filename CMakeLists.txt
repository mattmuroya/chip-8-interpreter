cmake_minimum_required(VERSION 3.10)
project(chip8)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Include CMake's FetchContent module
include(FetchContent)

# Fetch SDL3
FetchContent_Declare(
    SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG a8589a8
)
FetchContent_MakeAvailable(SDL)

# Define target executable
add_executable(chip8)

# Locate header files
target_include_directories(chip8
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# Locate C/C++ source files
target_sources(chip8
    PRIVATE
        src/main.cpp
        src/chip8.cpp
        src/cpu.cpp
        src/memory.cpp
        src/display.cpp
        src/keypad.cpp
)

# Link external library binaries
target_link_libraries(chip8 PRIVATE SDL3::SDL3)

# Copy ROMs folder into to the build directory
add_custom_target(copy_files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/roms/
        ${CMAKE_BINARY_DIR}/roms/
)
