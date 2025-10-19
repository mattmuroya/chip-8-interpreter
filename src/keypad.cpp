#include <cstdint>

#include "keypad.h"

void Keypad::keydown(uint8_t key)
{
    pressed_keys[key] = true;
}

void Keypad::keyup(uint8_t key)
{
    pressed_keys[key] = false;
}

bool Keypad::is_pressed(uint8_t key)
{
    // Zero-initializes if does not exist
    return pressed_keys[key];
}
