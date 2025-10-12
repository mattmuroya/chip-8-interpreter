#include "chip8.h"

int main()
{
    Chip8 chip8;

    chip8.load_rom("roms/ibm.ch8");
    chip8.run();
}
