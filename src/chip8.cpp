#include <string>
#include <fstream>
#include <iostream>

#include "chip8.h"
#include "utils.h"

Chip8::Chip8()
{
    load_font_set();
}

void Chip8::load_font_set()
{
    for (int i = 0; i < sizeof(FONT_SET); ++i)
    {
        memory.write(FONT_SET_START_ADDRESS + i, FONT_SET[i]);
    }
}

void Chip8::load_rom(std::string filename)
{
    // Load ROM as binary and position pointer at end of file
    std::ifstream file_in(filename, std::ios::binary | std::ios::ate);

    if (!file_in.is_open())
    {
        std::cerr << "Failed to load ROM" << std::endl;
        return;
    }

    // tellg() gets current pointer pointer
    const int file_length = file_in.tellg();

    // Return pointer to beginning of file
    file_in.seekg(0, file_in.beg);

    // Dynamic (heap) allocation since length not known at compile time
    char* buffer = new char[file_length];

    // Read file contents into buffer
    file_in.read(buffer, file_length);
    file_in.close();

    // Load buffer contents into memory
    for (int i = 0; i < file_length; ++i)
    {
        memory.write(START_ADDRESS + i, buffer[i]);
    }

    delete[] buffer;
}

void Chip8::run()
{
    double prev_cycle_time, prev_frame_time, prev_timer_time;
    prev_cycle_time = prev_frame_time = prev_timer_time = get_time_in_ms();

    while (true)
    {
        const double now = get_time_in_ms();

        if (now - prev_cycle_time >= CYCLE_DURATION_MS)
        {
            cpu.tick(memory, display);
            prev_cycle_time += CYCLE_DURATION_MS;
        }

        if (now - prev_timer_time >= TIMER_DURATION_MS)
        {
            cpu.decrement_timers();
            prev_timer_time += TIMER_DURATION_MS;
        }

        if (now - prev_frame_time >= FRAME_DURATION_MS && display.should_redraw())
        {
            display.redraw();
            prev_frame_time += FRAME_DURATION_MS;
        }
    }
}
