#pragma once

#include <cstdint>
#include <array>

#include "memory.h"
#include "display.h"
#include "constants.h"

class Cpu
{
public:
    Cpu();

    void tick(Memory& memory, Display& display);
    void decrement_timers();

private:
    void fetch_instruction(Memory& memory);
    void decode_and_execute(Memory&, Display& display);

    // General-purpose variable registers V0 to VF
    std::array<uint8_t, NUMBER_OF_REGISTERS> registers;

    // Address of next instruction to load
    // 16 bits accommodates 12-bit memory addresses
    uint16_t program_counter;

    // Index register; general-purpose memory pointer
    uint16_t index_register;

    // Fixed 16-slot call stack with pointer
    std::array<uint16_t, MAX_CALLSTACK> stack;
    uint8_t  stack_pointer;

    // Timers decrement at a rate of 60z until they reach 0
    uint8_t delay_timer;
    uint8_t sound_timer; // Plays tone while not 0

    // Slot to store current instruction
    uint16_t opcode;
};
