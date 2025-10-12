#include "cpu.h"
#include "memory.h"
#include "display.h"
#include "constants.h"

Cpu::Cpu() :
    registers{},
    program_counter{ START_ADDRESS },
    index_register{ 0 },
    stack{},
    stack_pointer{ 0 },
    delay_timer{ 0 },
    sound_timer{ 0 },
    opcode{}
{
}

void Cpu::tick(Memory& memory, Display& display)
{
    fetch_instruction(memory);
    decode_and_execute(memory, display);
}

void Cpu::fetch_instruction(Memory& memory)
{
    // Get the next 16-bit instruction from memory (two bytes)
    // Left-shift hi-byte by 8 bits; fills space to right with 0s
    // Bitwise OR overwrites 0s to the right with the lo-byte's values
    opcode = memory.read(program_counter) << 8 | memory.read(program_counter + 1);

    // Increment PC to next instruction
    program_counter += 2;
}

// Decode
void Cpu::decode_and_execute(Memory& memory, Display& display)
{
    const uint16_t op = (opcode & 0xF000);
    const uint8_t X = (opcode & 0x0F00) >> 8;
    const uint8_t Y = (opcode & 0x00F0) >> 4;
    const uint8_t N = (opcode & 0x000F);
    const uint8_t NN = (opcode & 0x00FF);
    const uint16_t NNN = (opcode & 0x0FFF);

    if (op == 0x0000)
    {
        if (NNN == 0x0E0)
        {
            display.clear();
        }
    }
    else if (op == 0x1000)
    {
        program_counter = NNN;
    }
    else if (op == 0x6000)
    {
        registers[X] = NN;
    }
    else if (op == 0x7000)
    {
        registers[X] += NN; // Note: Does not set carry flag (VF) on overflow
    }
    else if (op == 0xA000)
    {
        index_register = NNN;
    }
    else if (op == 0xD000)
    {
        // Draw

        // Index register contains pointer to memory location for sprite to be drawn
        // This pointer has been set by some previous instruction

        // Reset flag register
        registers[0x0F] = 0;

        // VX and VY indicate initial x- and y-coordinates for drawing the sprite
        // Initialize y-coordinate and convert absolute (wrapped) value
        uint8_t y_coord = registers[Y] & DISPLAY_HEIGHT - 1; // Same as VY % 32

        // N indicates the number of bytes to draw (i.e. the sprite's pixel height)
        for (int row = 0; row < N; ++row)
        {
            // Sprites should not wrap; end drawing current sprite
            if (y_coord >= DISPLAY_HEIGHT)
            {
                break;
            }

            // Initialize x-coordinate and convert absolute (wrapped) value
            uint8_t x_coord = registers[X] & DISPLAY_WIDTH - 1; // Same as VX % 64

            // Each byte represents a row of pixels (1 bit = 1 pixel)
            const uint8_t sprite_data = memory.read(index_register + row);

            for (int col = 0; col < 8; ++col)
            {
                // Sprites should not wrap; end drawing current row
                if (x_coord >= DISPLAY_WIDTH)
                {
                    break;
                }

                // Shift current bit into the 1s place and mask to get the pixel value
                const bool sprite_pixel = (sprite_data >> (7 - col)) & 0x0001;

                if (sprite_pixel)
                {
                    display.set_pixel(x_coord, y_coord);
                }

                x_coord += 1;
            }

            y_coord += 1;
        }
    }
}

void Cpu::decrement_timers()
{
    if (this->delay_timer > 0)
    {
        --this->delay_timer;
    }
    if (this->sound_timer > 0)
    {
        --this->sound_timer;
    }
}
