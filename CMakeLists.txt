cmake_minimum_required(VERSION 3.10)
project(chip8)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

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
        src/display.cpp
        src/memory.cpp
)

# Copy ROMs folder into to the build directory
add_custom_target(copy_files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/roms/
        ${CMAKE_BINARY_DIR}/roms/
)
