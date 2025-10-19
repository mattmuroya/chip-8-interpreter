#pragma once

#include <cstdint>
#include <map>

#include <SDL3/SDL_scancode.h>

class Keypad
{
public:
    void keydown(uint8_t key);
    void keyup(uint8_t key);
    bool is_pressed(uint8_t key);

private:
    std::map<uint8_t, bool> pressed_keys;
};
