#include <cstdint>
#include <fstream>
#include <vector>
#include <iostream>
#include <chrono>

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
    bool draw_flag = false;

    // Program counter; points to current instruction
    // 16 bits accommodates 12-bit memory addresses
    uint16_t program_counter{ START_ADDRESS };

    // Index register; general-purpose memory pointer
    uint16_t index_register{};

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
        for (int i = 0; i < 80; ++i)
        {
            this->memory[FONT_SET_START_ADDRESS + i] = font_set[i];
        }

        // Load IBM test ROM as binary and position pointer at end of file
        std::ifstream file_in("roms/ibm.ch8", std::ios::binary | std::ios::ate);

        if (!file_in.is_open())
        {
            std::cerr << "Failed to load ROM" << std::endl;
            return;
        }

        // Read file into buffer (adapted from cplusplus.com example code)
        // tellg() returns current pointer position in the input sequence
        // Since the "at end" flag was set, pointer position gives us the file length
        const int file_length = file_in.tellg();

        // Return pointer to beginning of file
        file_in.seekg(0, file_in.beg);

        // Allocate memory dynamically (heap allocation since length not known at compile time)
        // Store pointer to new char array
        char* buffer = new char[file_length];

        // Read N (file_length) characters from file into buffer array
        file_in.read(buffer, file_length);
        file_in.close();

        // Load buffer contents into memory
        for (int i = 0; i < file_length; ++i)
        {
            this->memory[START_ADDRESS + i] = buffer[i];
        }

        // Deallocate memory
        delete[] buffer;
    }

    bool should_redraw()
    {
        return this->draw_flag;
    }

    void redraw()
    {
        std::cout << "\033[2J\033[1;1H";
        std::cout.flush(); // Ensure the output is immediately sent to the terminal

        int col_position = 0;
        for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; ++i)
        {
            std::cout << (this->display[i] ? "██" : "  ");
            if (++col_position >= DISPLAY_WIDTH)
            {
                std::cout << '\n';
                col_position = 0;
            }
        }

        draw_flag = false;
    }

    void decrement_timers()
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

    void cycle()
    {
        this->fetch_instruction();
        this->decode_and_execute();
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
    void decode_and_execute()
    {
        const uint16_t op = (this->opcode & 0xF000);
        const uint8_t X = (this->opcode & 0x0F00) >> 8;
        const uint8_t Y = (this->opcode & 0x00F0) >> 4;
        const uint8_t N = (this->opcode & 0x000F);
        const uint8_t NN = (this->opcode & 0x00FF);
        const uint16_t NNN = (this->opcode & 0x0FFF);

        if (op == 0x0000)
        {
            if (NNN == 0x0E0)
            {
                // Loop over and clear each 32-bit display element (pixel)
                for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; ++i)
                {
                    this->display[i] = 0x0000;
                }
            }
        }
        else if (op == 0x1000)
        {
            // Jump to NNN
            this->program_counter = NNN;
        }
        else if (op == 0x6000)
        {
            // Set register VX to NN
            this->registers[X] = NN;
        }
        else if (op == 0x7000)
        {
            // Add value to register VX
            // Does not set carry flag (VF) on overflow
            this->registers[X] += NN;
        }
        else if (op == 0xA000)
        {
            // Set index register to NNN
            this->index_register = NNN;
        }
        else if (op == 0xD000)
        {
            // Draw

            // Index register contains pointer to memory location for sprite to be drawn
            // This pointer has been set by some previous instruction

            // Reset flag register
            this->registers[0x0F] = 0;

            // VX and VY indicate initial x- and y-coordinates for drawing the sprite
            // Initialize y-coordinate and convert absolute (wrapped) value
            uint8_t y_coord = this->registers[Y] & DISPLAY_HEIGHT - 1; // Same as VY % 32

            // N indicates the number of bytes to draw (i.e. the sprite's pixel height)
            for (int row = 0; row < N; ++row)
            {
                // Sprites should not wrap; end drawing current sprite
                if (y_coord >= DISPLAY_HEIGHT)
                {
                    break;
                }

                // Initialize x-coordinate and convert absolute (wrapped) value
                uint8_t x_coord = this->registers[X] & DISPLAY_WIDTH - 1; // Same as VX % 64

                // Each byte represents a row of pixels (1 bit = 1 pixel)
                const uint8_t sprite_data = this->memory[index_register + row];

                for (int col = 0; col < 8; ++col)
                {
                    // Sprites should not wrap; end drawing current row
                    if (x_coord >= DISPLAY_WIDTH)
                    {
                        break;
                    }

                    // Shift current bit into the 1s place and mask to get the sprite pixel value
                    const bool sprite_pixel = (sprite_data >> (7 - col)) & 0x0001;

                    // Only flip display pixel/set draw flag if sprite pixel is active
                    if (sprite_pixel)
                    {
                        // Get the value of the corresponding display pixel
                        const int display_index = x_coord + (y_coord * DISPLAY_WIDTH);
                        const bool display_pixel = this->display[display_index]; // Truncates 32-bit int

                        if (display_pixel)
                        {
                            this->display[display_index] = PIXEL_OFF;
                            this->registers[0x0F] = 1;
                        }
                        else
                        {
                            this->display[display_index] = PIXEL_ON;
                        }

                        this->draw_flag = true;
                    }

                    x_coord += 1;
                }

                y_coord += 1;
            }
        }
    }
};

double get_time_in_ms()
{
    using namespace std::chrono;
    using double_ms = duration<double, std::chrono::milliseconds::period>;

    auto now = system_clock::now().time_since_epoch();
    double now_ms = duration_cast<double_ms>(now).count();

    return now_ms;
}

int main()
{
    Chip8 chip8;

    double prev_cycle_time, prev_frame_time, prev_timer_time;
    prev_cycle_time = prev_frame_time = get_time_in_ms();

    while (true)
    {
        const double now = get_time_in_ms();

        const double elapsed_cycle_time = now - prev_cycle_time;
        if (elapsed_cycle_time >= CYCLE_DURATION_MS)
        {
            chip8.cycle();
            prev_cycle_time += CYCLE_DURATION_MS;
        }

        const double elapsed_frame_time = now - prev_frame_time;
        if (elapsed_frame_time >= FRAME_DURATION_MS && chip8.should_redraw())
        {
            chip8.redraw();
            prev_frame_time += FRAME_DURATION_MS;
        }

        const double elapsed_timer_time = now - prev_timer_time;
        if (elapsed_timer_time >= TIMER_DURATION_MS)
        {
            chip8.decrement_timers();
            prev_timer_time += TIMER_DURATION_MS;
        }
    }
}
