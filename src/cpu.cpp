#include <cstdlib>
#include <ctime>

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
    opcode{ 0 }
{
    srand(time(NULL));
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
        if (NNN == 0x00E0)
        {
            display.clear();
        }
        else if (NNN == 0x00EE)
        {
            // Return from subroutine
            stack_pointer -= 1;
            program_counter = stack[stack_pointer];
        }
    }
    else if (op == 0x1000)
    {
        program_counter = NNN;
    }
    else if (op == 0x2000)
    {
        // Call subroutine
        stack[stack_pointer] = program_counter;
        stack_pointer += 1;
        program_counter = NNN;
    }
    else if (op == 0x3000)
    {
        if (registers[X] == NN)
        {
            program_counter += 2;
        }
    }
    else if (op == 0x4000)
    {
        if (registers[X] != NN)
        {
            program_counter += 2;
        }
    }
    else if (op == 0x5000)
    {
        if (registers[X] == registers[Y])
        {
            program_counter += 2;
        }
    }
    else if (op == 0x6000)
    {
        registers[X] = NN;
    }
    else if (op == 0x7000)
    {
        // Note: Do NOT set carry flag (VF) on overflow
        registers[X] += NN;
    }
    else if (op == 0x8000)
    {
        if (N == 0x0)
        {
            registers[X] = registers[Y];
        }
        else if (N == 0x1)
        {
            registers[X] |= registers[Y];
        }
        else if (N == 0x2)
        {
            registers[X] &= registers[Y];
        }
        else if (N == 0x3)
        {
            registers[X] ^= registers[Y];
        }
        else if (N == 0x4)
        {
            // Check if operation will result in overflow (sum greater than max 8-bit integer)
            int overflow = registers[X] > 0xFF - registers[Y] ? 1 : 0;

            // Perform operation
            registers[X] += registers[Y];

            // Set carry flag
            registers[0xF] = overflow;
        }
        else if (N == 0x5)
        {
            // Check if operation will result in "underflow" (1 if no carry required; 0 if carry required)
            int overflow = registers[X] >= registers[Y] ? 1 : 0;

            // Perform operation
            registers[X] -= registers[Y];

            // Set carry flag
            registers[0xF] = overflow;
        }
        else if (N == 0x6)
        {
            if (CLASSIC_MODE)
            {
                registers[X] = registers[Y];
            }

            // Store bit that will be lost in shift
            uint8_t lost_bit = registers[X] & 0b00000001;
            registers[X] >>= 1;
            registers[0xF] = lost_bit;
        }
        else if (N == 0x7)
        {
            // Check if operation will result in "underflow" (1 if no carry required; 0 if carry required)
            int overflow = registers[Y] >= registers[X] ? 1 : 0;

            // Perform operation
            registers[X] = registers[Y] - registers[X];

            // Set carry flag
            registers[0xF] = overflow;
        }
        else if (N == 0xE)
        {
            if (CLASSIC_MODE)
            {
                registers[X] = registers[Y];
            }

            // Store bit that will be lost in shift
            uint8_t lost_bit = (registers[X] & 0b10000000) >> 7;
            registers[X] <<= 1;
            registers[0xF] = lost_bit;
        }
    }
    else if (op == 0x9000)
    {
        if (registers[X] != registers[Y])
        {
            program_counter += 2;
        }
    }
    else if (op == 0xA000)
    {
        index_register = NNN;
    }
    else if (op == 0xB000)
    {
        program_counter = NNN + registers[CLASSIC_MODE ? 0x0 : X];
    }
    else if (op == 0xC000)
    {
        registers[X] = rand() & NN;
    }
    else if (op == 0xD000)
    {
        // Draw

        // Index register contains pointer to memory location for sprite to be drawn
        // This pointer has been set by some previous instruction

        // Reset flag register
        registers[0xF] = 0;

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
                const bool sprite_pixel = (sprite_data >> (7 - col)) & 1;

                if (sprite_pixel)
                {
                    display.set_pixel(x_coord, y_coord);
                }

                x_coord += 1;
            }

            y_coord += 1;
        }
    }
    else if (op == 0xE000)
    {
        // TODO: Skip if key (keypad implementation)
    }
    else if (op == 0xF000)
    {
        if (NN == 0x07)
        {
            registers[X] = delay_timer;
        }
        else if (NN == 0x15)
        {
            delay_timer = registers[X];
        }
        else if (NN == 0x18)
        {
            sound_timer = registers[X];
        }
        else if (NN == 0x0A)
        {
            // TODO: Get key (keypad implementation)
        }
        else if (NN == 0x1E)
        {
            index_register += registers[X];

            // Set carry flag if index register overflows the typical 12-bit addressing range
            // The original COSMAC VIP interpreter doesn't do this but some others (e.g. Amiga) do
            if (index_register > 0x0FFF)
            {
                registers[0xF] = 1;
            }
        }
        else if (NN == 0x29)
        {
            index_register = FONT_SET_START_ADDRESS + (registers[X] * BYTES_PER_FONT_SPRITE);
        }
        else if (NN == 0x33)
        {
            // Binary-coded decimal conversion
            uint8_t decimal_value = registers[X];

            // Place each digit of decimal value in X into memory starting at index register
            // Iterate backwards beginning with least significant digit
            for (int i = 2; i >= 0; --i)
            {
                memory.write(index_register + i, decimal_value % 10);
                decimal_value /= 10;
            }
        }
        else if (NN == 0x55)
        {
            for (int i = 0; i <= X; ++i)
            {
                memory.write(index_register + i, registers[i]);
            }

            if (CLASSIC_MODE)
            {
                index_register += (X + 1);
            }
        }
        else if (NN == 0x65)
        {
            for (int i = 0; i <= X; ++i)
            {
                registers[i] = memory.read(index_register + i);
            }

            if (CLASSIC_MODE)
            {
                index_register += (X + 1);
            }
        }
    }
}

void Cpu::decrement_timers()
{
    if (delay_timer > 0)
    {
        --delay_timer;
    }
    if (sound_timer > 0)
    {
        --sound_timer;
    }
}
