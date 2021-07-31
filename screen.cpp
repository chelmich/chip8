#include "screen.hpp"

void Screen::createTexture(SDL_Renderer* renderer) {
    m_texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGB888,
        SDL_TEXTUREACCESS_TARGET,
        64, 32);
}

void Screen::destroyTexture() {
    SDL_DestroyTexture(m_texture);
}

void Screen::update(SDL_Renderer* renderer, Chip8 const& chip) const {
    SDL_SetRenderTarget(renderer, m_texture);

    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, fg.r, fg.g, fg.b, fg.a);

    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 32; y++) {
            int index = y * 64 + x;
            int byte = index / 8;
            int bit = index % 8;

            if (chip.screen[byte] & (1 << bit)) {
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }

    SDL_SetRenderTarget(renderer, NULL);
}

void Screen::draw(SDL_Renderer* renderer, int x, int y, int scale) const {
    SDL_Rect rect = {x, y, 64 * scale, 32 * scale};
    SDL_RenderCopy(renderer, m_texture, NULL, &rect);
}
