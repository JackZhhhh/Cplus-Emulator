// main.cpp (SDL3 version)
#include "chip8.h"
#include <SDL3/SDL.h>
#include <iostream>
#include <chrono>
#include <thread>

const int VIDEO_SCALE = 10;
const int VIDEO_WIDTH = 64;
const int VIDEO_HEIGHT = 32;

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: chip8 <ROM file>\n";
        return 1;
    }

    Chip8 chip8;
    chip8.loadROM(argv[1]);


    SDL_Window* window = SDL_CreateWindow("CHIP-8 Emulator", VIDEO_WIDTH * VIDEO_SCALE, VIDEO_HEIGHT * VIDEO_SCALE, 0);
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING, VIDEO_WIDTH, VIDEO_HEIGHT);

    bool quit = false;
    SDL_Event event;

    const SDL_Scancode keymap[16] = {
        SDL_SCANCODE_X, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
        SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_A,
        SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Z, SDL_SCANCODE_C,
        SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V
    };

    while (!quit) {
        // Handle input
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) quit = true;
            if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
                bool pressed = (event.type == SDL_EVENT_KEY_DOWN);
                SDL_Scancode scancode = event.key.scancode;
                for (int i = 0; i < 16; ++i) {
                    if (scancode == keymap[i])
                        chip8.key[i] = pressed;
                }
            }
        }

        chip8.emulateCycle();

        // Draw
        if (chip8.drawFlag) {
            chip8.drawFlag = false;

            uint32_t pixels[VIDEO_WIDTH * VIDEO_HEIGHT];
            for (int i = 0; i < VIDEO_WIDTH * VIDEO_HEIGHT; ++i) {
                uint8_t pixel = chip8.gfx[i];
                pixels[i] = pixel ? 0xFFFFFFFF : 0xFF000000;
            }

            SDL_UpdateTexture(texture, nullptr, pixels, VIDEO_WIDTH * sizeof(uint32_t));
            SDL_RenderTexture(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);
        }

        std::this_thread::sleep_for(std::chrono::microseconds(16667)); // ~60Hz
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
