#include <cstdint>
#include <iostream>

#include "display.h"
#include "constants.h"

uint32_t Display::set_pixel(int x, int y)
{
    const int display_index = x + (y * DISPLAY_WIDTH);
    const uint32_t new_pixel_value = buffer[display_index] ^= PIXEL_ON; // Bitwise OR

    return new_pixel_value;
}

void Display::clear()
{
    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; ++i)
    {
        buffer[i] = PIXEL_OFF;
    }
}

const uint32_t* Display::get_display_buffer() const
{
    return buffer;
}
