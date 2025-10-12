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
    void run();

private:
    Cpu cpu;
    Memory memory;
    Display display;
};
