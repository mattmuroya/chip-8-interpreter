#pragma once

#include <cstdint>

// CHIP-8 config
const double CPU_HZ = 700.0; // Clock speed (cycles per second)
const double DISPLAY_HZ = 60.0; // Refresh rate (frames per second)

const double TIMER_HZ = 60.0;

// Calculated cycle/frame durations (ms)
const double CYCLE_DURATION_MS = 1000.0 / CPU_HZ;
const double FRAME_DURATION_MS = 1000.0 / DISPLAY_HZ;
const double TIMER_DURATION_MS = 1000.0 / TIMER_HZ;

// General constants
const unsigned int MEMORY_SIZE = 4096;
const unsigned int DISPLAY_WIDTH = 64;
const unsigned int DISPLAY_HEIGHT = 32;

const uint32_t PIXEL_OFF = 0x00000000;
const uint32_t PIXEL_ON = 0xFFFFFFFF;

const unsigned int NUMBER_OF_REGISTERS = 16;
const unsigned int MAX_CALLSTACK = 16;

// The original CHIP-8 interpreter was loaded into RAM from 0x000 to 0x1FF
// Program ROMS were loaded into the unreserved memory starting at 0x200 
const unsigned int START_ADDRESS = 0x200;

// Built-in fonts are placed inside the block of memory reserved for the
// interpreter, conventionally from 0x050 to 0x09F
const unsigned int FONT_SET_START_ADDRESS = 0x050;

// The built-in font set includes 16 character sprites, each drawn in 4x5
// resolution and represented by four half-bytes

// For example, the letter F is represented like this:

// 0xF0 --> 11110000 --> 1111
// 0x80 --> 10000000 --> 1
// 0xF0 --> 11110000 --> 1111
// 0x80 --> 10000000 --> 1
// 0x80 --> 10000000 --> 1

const uint8_t FONT_SET[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
