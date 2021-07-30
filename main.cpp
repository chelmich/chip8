#include <stdio.h>
#include <stdint.h>

#include <SDL2/SDL.h>

#include "chip8.hpp"

void print_registers(uint8_t regs[16]) {
    for (int i = 0; i < 16; i++) {
        printf("V%X  ", i);
    }
    printf("\n");

    for (int i = 0; i < 16; i++) {
        printf("%02X  ", regs[i]);
    }
    printf("\n");
}

void print_memory(uint8_t mem[4096]) {
    int addr = 0;
    while (addr < 4096) {
        printf("0x%03x: ", addr);
        for (int i = 0; i < 8; i++) {
            printf("%02x%02x", mem[addr], mem[addr+1]);
            putchar(i == 7 ? '\n' : ' ');
            addr += 2;
        }
    }
}

void print_usage() {
    printf("Usage: ./chip8 [FILE]\n");
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        print_usage();
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0)  {
        fprintf(stderr, "ERROR: SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    Chip8 chip;
    chip.loadROM(argv[1]);

    int scale = 8;

    SDL_Window* window = SDL_CreateWindow("chip-8 emulator",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        64 * scale, 32 * scale,
        SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED);

    SDL_Color bg_color = {26, 42, 61, 255};
    SDL_Color fg_color = {202, 217, 235, 255};

    bool shouldClose = false;
    while (!shouldClose) {
        SDL_Event event;
        SDL_WaitEvent(&event);

        switch(event.type) {
        case SDL_QUIT:
            shouldClose = true;
            break;
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym) {
            case SDLK_ESCAPE:
                shouldClose = true;
                break;
            case SDLK_s:
                chip.update();
                break;
            }
            break;
        }

        SDL_SetRenderDrawColor(renderer,
            bg_color.r, bg_color.g, bg_color.b, bg_color.a);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer,
            fg_color.r, fg_color.g, fg_color.b, fg_color.a);
        chip.draw(renderer, scale);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
