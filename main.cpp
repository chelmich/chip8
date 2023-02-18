#include "chip8.hpp"
#include "screen.hpp"
#include "bitmapfont.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

void draw_registers(BitmapFont const& font, Chip8 const& chip, int x, int y, int scale) {
    const size_t line_buf_len = 20;
    char line_buf[line_buf_len];

    font.drawStr("Registers:", x, y, scale);
    y += font.char_height * scale;

    for (int i = 0; i < 8; i++) {
        snprintf(line_buf, line_buf_len, "V%X %02x   V%X %02x",
            i, chip.regs[i],
            i+8, chip.regs[i+8]);

        font.drawStr(line_buf, x, y, scale);
        y += font.char_height * scale;
    }

    snprintf(line_buf, line_buf_len, "I  %04x", chip.ir);
    font.drawStr(line_buf, x, y, scale);
}

void print_usage() {
    printf("Usage: ./chip8 [FILE]\n");
}

void log_SDL_error(char const* function) {
    fprintf(stderr, "ERROR: %s: %s\n", function, SDL_GetError());
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        print_usage();
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        log_SDL_error("SDL_Init");
        return 1;
    }

    int img_flags = IMG_INIT_PNG;
    if (IMG_Init(img_flags) != img_flags) {
        log_SDL_error("IMG_Init");
        SDL_Quit();
        return 1;
    }

    srand(0);

    Chip8 chip;
    chip.loadFont();
    chip.loadROM(argv[1]);

    chip.quirk_shift = true;
    chip.quirk_regs_load_store = true;

    const int screen_scale = 8;

    Screen screen;
    screen.bg = {26, 42, 61, 255};
    screen.fg = {202, 217, 235, 255};

    SDL_Window* window = SDL_CreateWindow("chip-8 emulator",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        chip.screen_width * screen_scale + 200, chip.screen_height * screen_scale,
        SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        log_SDL_error("SDL_CreateWindow");
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        log_SDL_error("SDL_CreateRenderer");
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    screen.createTexture(renderer);
    screen.update(renderer, chip);

    const int font_scale = 2;
    BitmapFont gui_font(renderer,"res/pixel_font_basic_latin_ascii.png", 7, 9);

    // Main loop
    bool shouldClose = false;
    while (!shouldClose) {
        SDL_Event event;
        SDL_WaitEvent(&event);

        switch (event.type) {
        case SDL_QUIT:
            shouldClose = true;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                shouldClose = true;
                break;
            case SDLK_s:
                chip.update();
                screen.update(renderer, chip);
                break;
            }
            break;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        screen.draw(renderer, 0, 0, screen_scale);
        draw_registers(gui_font, chip, chip.screen_width * screen_scale, 0, font_scale);

        SDL_RenderPresent(renderer);
    }

    // Clean up SDL
    screen.destroyTexture();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
