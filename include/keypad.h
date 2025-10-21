#pragma once

#include <cstdint>

#include <SDL3/SDL_scancode.h>

class Keypad
{
public:
    Keypad();

    // Emit key events
    void keydown(uint8_t key);
    void keyup(uint8_t key);
    void clear_key_events();

    // Get key state
    bool is_pressed(uint8_t key);
    bool is_pressed_this_loop(uint8_t key);
    bool is_released_this_loop(uint8_t key);

private:
    bool pressed[16];
    bool pressed_this_loop[16];
    bool released_this_loop[16];
};
