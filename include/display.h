#pragma once

#include <cstdint>

#include "constants.h"

class Display
{
public:
    uint32_t set_pixel(int x, int y);
    void clear();
    const uint32_t* get_display_buffer() const;

private:
    uint32_t buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT];
    bool draw_flag;
};
