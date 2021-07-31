#pragma once

#include <SDL2/SDL_render.h>

#include "chip8.hpp"

class Screen {
public:
    SDL_Color bg = {0, 0, 0, 255};
    SDL_Color fg = {255, 255, 255, 255};

    int scale = 1;

    void draw(SDL_Renderer* renderer, Chip8 const& chip) const;
};
