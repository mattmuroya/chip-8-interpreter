#include <SDL3/SDL.h>

#include "chip8.h"
#include "utils.h"
#include "constants.h"

bool poll_events();
bool init_sdl();
void quit_sdl();

SDL_Window* g_window{ nullptr };
SDL_Renderer* g_renderer{ nullptr };
SDL_Texture* g_texture{ nullptr };
SDL_FRect destination_rect{  };

int main()
{
    init_sdl();

    Chip8 chip8 = Chip8();
    chip8.load_rom("roms/tests/4-flags.ch8");

    bool running = true;

    double prev_cycle_time, prev_frame_time, prev_timer_time;
    prev_cycle_time = prev_frame_time = prev_timer_time = get_time_in_ms();

    while (running)
    {

        if (poll_events() == false)
        {
            running = false;
            continue;
        }

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

        if (now - prev_frame_time >= FRAME_DURATION_MS)
        {
            const uint32_t* pixels = chip8.get_display_buffer();

            SDL_RenderClear(g_renderer);
            SDL_UpdateTexture(g_texture, NULL, pixels, DISPLAY_WIDTH * sizeof(uint32_t));
            SDL_RenderTexture(g_renderer, g_texture, NULL, NULL);
            SDL_RenderPresent(g_renderer);

            prev_frame_time += FRAME_DURATION_MS;
        }
    }

    quit_sdl();
}

bool poll_events()
{
    SDL_Event e;
    if (SDL_PollEvent(&e) == true)
    {
        if (e.type == SDL_EVENT_QUIT)
        {
            return false;
        }
        else if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_ESCAPE)
        {
            return false;
        }
    }

    return true;
}

bool init_sdl()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == false)
    {
        SDL_Log("SDL was not initialized. SDL error: %s\n", SDL_GetError());
        return false;
    }

    g_window = SDL_CreateWindow("CHIP-8", DISPLAY_WIDTH * DISPLAY_SCALE, DISPLAY_HEIGHT * DISPLAY_SCALE, 0);
    g_renderer = SDL_CreateRenderer(g_window, NULL);
    g_texture = SDL_CreateTexture(g_renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    SDL_SetTextureScaleMode(g_texture, SDL_SCALEMODE_NEAREST);

    if (!g_window || !g_renderer || !g_texture)
    {
        SDL_Log("Window/renderer/texture not initialized. SDL error: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void quit_sdl()
{
    SDL_DestroyTexture(g_texture);
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);

    SDL_Quit();
}