#include "screen.hpp"

void Screen::draw(SDL_Renderer* renderer, Chip8 const& chip) const {
    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, fg.r, fg.g, fg.b, fg.a);

    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 32; y++) {
            int index = y * 64 + x;
            int byte = index / 8;
            int bit = index % 8;

            if (chip.screen[byte] & (1 << bit)) {
                SDL_Rect rect = {x * scale, y * scale, scale, scale};
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}
