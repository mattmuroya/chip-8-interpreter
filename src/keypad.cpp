#include <cstdint>
#include <map>

#include "keypad.h"

Keypad::Keypad() :
    pressed{}
{
}

void Keypad::keydown(uint8_t key)
{
    pressed[key] = true;
    pressed_this_loop[key] = true;
}

void Keypad::keyup(uint8_t key)
{
    pressed[key] = false;
    released_this_loop[key] = true;
}

void Keypad::clear_key_events()
{
    for (int i = 0; i < 16; ++i)
    {
        pressed_this_loop[i] = false;
        released_this_loop[i] = false;
    }
}

bool Keypad::is_pressed(uint8_t key)
{
    // Zero-initializes if does not exist
    return pressed[key];
}

bool Keypad::is_pressed_this_loop(uint8_t key)
{
    return pressed_this_loop[key];
}

bool Keypad::is_released_this_loop(uint8_t key)
{
    return released_this_loop[key];
}
