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
}

void Keypad::keyup(uint8_t key)
{
    pressed[key] = false;
}

bool Keypad::is_pressed(uint8_t key)
{
    // Zero-initializes if does not exist
    return pressed[key];
}
