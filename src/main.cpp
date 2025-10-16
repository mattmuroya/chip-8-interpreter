#include "chip8.h"
#include "utils.h"

int main()
{
    Chip8 chip8;

    chip8.load_rom("roms/tests/4-flags.ch8");

    bool running = true;
    double prev_cycle_time, prev_frame_time, prev_timer_time;
    prev_cycle_time = prev_frame_time = prev_timer_time = get_time_in_ms();

    while (running)
    {
        const double now = get_time_in_ms();

        if (now - prev_cycle_time >= CYCLE_DURATION_MS)
        {
            chip8.cycle_cpu();
            prev_cycle_time += CYCLE_DURATION_MS;
        }

        if (now - prev_timer_time >= TIMER_DURATION_MS)
        {
            chip8.decrement_timers();
            prev_timer_time += TIMER_DURATION_MS;
        }

        if (now - prev_frame_time >= FRAME_DURATION_MS && chip8.should_redraw())
        {
            chip8.redraw();
            prev_frame_time += FRAME_DURATION_MS;
        }
    }
}
