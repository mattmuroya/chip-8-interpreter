#include <SDL3/SDL.h>

#include "chip8.h"
#include "utils.h"
#include "constants.h"

bool poll_input();
bool init_sdl();
void quit_sdl();

SDL_Window* g_window{ nullptr };
SDL_Renderer* g_renderer{ nullptr };
SDL_Texture* g_texture{ nullptr };

Chip8 chip8;

int main()
{
    init_sdl();

    chip8 = Chip8();
    chip8.load_rom("roms/Pong 2 (Pong hack) [David Winter, 1997].ch8");

    bool running = true;

    double prev_cycle_time, prev_frame_time, prev_timer_time;
    prev_cycle_time = prev_frame_time = prev_timer_time = get_time_in_ms();

    while (running)
    {
        if (!poll_input())
        {
            running = false;
            continue;
        }

        const double now = get_time_in_ms();

        if (now - prev_cycle_time >= CYCLE_DURATION_MS)
        {
            chip8.cycle_cpu();
            chip8.clear_key_events();
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

bool poll_input()
{
    SDL_Event event;

    if (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            return false;
        }
        else if (event.type == SDL_EVENT_KEY_DOWN)
        {
            // Have to do it this way because map::operator[] has no const overload
            // Use map::find to get an iterator that returns the k-v pair
            auto it = KEYMAP.find(event.key.scancode);

            // Check that the key was found in map
            if (it != KEYMAP.end())
            {
                // Get second item (value) from k-v pair
                chip8.keydown(it->second);
            }
        }
        else if (event.type == SDL_EVENT_KEY_UP)
        {
            if (event.key.key == SDLK_ESCAPE)
            {
                return false;
            }

            auto it = KEYMAP.find(event.key.scancode);
            if (it != KEYMAP.end())
            {
                chip8.keyup(it->second);
            }
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