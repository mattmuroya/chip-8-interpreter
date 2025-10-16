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
    bool should_redraw();
    void cycle_cpu();
    void decrement_timers();
    void redraw();

private:
    Cpu cpu;
    Memory memory;
    Display display;
};
