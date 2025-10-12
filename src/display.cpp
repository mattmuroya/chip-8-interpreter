#include <cstdint>
#include <iostream>

#include "display.h"
#include "constants.h"

uint32_t Display::set_pixel(int x, int y)
{
    const int display_index = x + (y * DISPLAY_WIDTH);
    const uint32_t new_pixel_value = this->buffer[display_index] ^= PIXEL_ON; // Bitwise OR

    this->draw_flag = true;
    return new_pixel_value;
}

void Display::clear()
{
    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; ++i)
    {
        this->buffer[i] = PIXEL_OFF;
    }
}

bool Display::should_redraw()
{
    return this->draw_flag;
}

void Display::redraw()
{

    std::cout << "\033[2J\033[1;1H";
    std::cout.flush();

    int col_position = 0;
    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; ++i)
    {
        std::cout << (this->buffer[i] ? "██" : "  ");
        if (++col_position >= DISPLAY_WIDTH)
        {
            std::cout << '\n';
            col_position = 0;
        }
    }

    this->draw_flag = false;
}
