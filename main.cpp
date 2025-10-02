#include <cstdint>
#include <iostream>

// General constants
const unsigned int MEMORY_SIZE = 4096;
const unsigned int DISPLAY_WIDTH = 64;
const unsigned int DISPLAY_HEIGHT = 32;

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

const uint8_t font_set[80] = {
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

class Chip8
{
    // General-purpose variable registers V0 to VF
    uint8_t registers[16]{};

    // 4KB (4096 bytes) of virtual RAM
    uint8_t memory[MEMORY_SIZE]{};

    // 64x32 monochrome display (32-bit int for SDL compatibility)
    uint32_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT]{};

    // Program counter; points to current instruction
    // 16 bits accommodates 12-bit memory addresses
    uint16_t program_counter{ START_ADDRESS };

    // Index register; general-purpose memory pointer
    uint16_t index{};

    // Fixed 16-slot call stack with pointer
    uint16_t stack[16]{};
    uint8_t  stack_pointer{ 0 };

    // Timers decrement at a rate of 60z until they reach 0
    uint8_t delay_timer{};
    uint8_t sound_timer{}; // Plays tone while not 0

    // Slot to store current instruction
    uint16_t opcode{};

public:
    Chip8()
    {
        // Load font set into memory from 0x050 to 0x09F
        for (unsigned int i = 0; i < 80; ++i)
        {
            this->memory[FONT_SET_START_ADDRESS + i] = font_set[i];
        }
    }

    // Main loop
    void run()
    {
        bool exit = false;

        while (!exit)
        {
            this->fetch_instruction();
        }
    }

    // Fetch
    void fetch_instruction()
    {
        // Get the next 16-bit instruction from memory (two bytes)
        // Left-shift hi-byte by 8 bits; fills space to right with 0s
        // Bitwise OR overwrites 0s to the right with the lo-byte's values
        this->opcode = memory[program_counter] << 8 | memory[program_counter + 1];

        // Increment PC to next instruction
        this->program_counter += 2;
    }

    // Decode

    // Execute
};

int main()
{
    Chip8 chip8;
    chip8.run();
}
