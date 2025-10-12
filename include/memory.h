#pragma once

#include <cstdint>

#include "constants.h"

class Memory
{
public:
    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t value);

private:
    uint8_t data[MEMORY_SIZE];
};
