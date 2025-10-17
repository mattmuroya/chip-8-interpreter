#pragma once

#include <string>

#include "cpu.h"
#include "memory.h"
#include "display.h"

class Chip8
{
public:
    Chip8();

    void load_font_set();
    void load_rom(std::string filename);
    void cycle_cpu();
    void decrement_timers();
    const uint32_t* get_display_buffer() const;

private:
    Cpu cpu;
    Memory memory;
    Display display;
};
