#pragma once

#include <cstdint>

#include "constants.h"

class Display
{
public:
    uint32_t set_pixel(int x, int y);
    void clear();
    bool should_redraw();
    void redraw();

private:
    uint32_t buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT];
    bool draw_flag;
};
