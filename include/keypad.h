#pragma once

#include <cstdint>

#include <SDL3/SDL_scancode.h>

class Keypad
{
public:
    Keypad();

    void keydown(uint8_t key);
    void keyup(uint8_t key);
    bool is_pressed(uint8_t key);

private:
    bool pressed[16];
};
