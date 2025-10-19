#include <cstdint>

#include "memory.h"

uint8_t Memory::read(uint16_t address)
{
    return data[address];
}

void Memory::write(uint16_t address, uint8_t value)
{
    data[address] = value;
}
