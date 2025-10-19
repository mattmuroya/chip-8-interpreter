#pragma once

#include <string>

#include "cpu.h"
#include "memory.h"
#include "display.h"
#include "keypad.h"

class Chip8
{
public:
    Chip8();

    void load_font_set();
    void load_rom(std::string filename);
    void cycle_cpu();
    void decrement_timers();
    void keyup(uint8_t key);
    void keydown(uint8_t key);
    const uint32_t* get_display_buffer() const;

private:
    Cpu cpu;
    Memory memory;
    Display display;
    Keypad keypad;
};
