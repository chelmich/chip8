#pragma once

#include "chip8.hpp"

#include <SDL2/SDL_render.h>

class Screen {
public:
    SDL_Color bg = {0, 0, 0, 255};
    SDL_Color fg = {255, 255, 255, 255};

    void create_texture(SDL_Renderer* renderer);
    void destroy_texture();

    // Update the internal SDL_Texture to reflect the state of the Chip-8 screen
    void update(SDL_Renderer* renderer, Chip8 const& chip) const;

    // Calls SDL_RenderCopy on the internal screen texture
    void draw(SDL_Renderer* renderer, int x, int y, int scale) const;

private:
    SDL_Texture* m_texture;

    const int m_xres = 64;
    const int m_yres = 32;
};
