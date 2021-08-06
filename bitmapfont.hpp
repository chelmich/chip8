#pragma once

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>

class BitmapFont {
public:
    BitmapFont(SDL_Renderer* renderer, char const* filename, int width, int height):
        char_width(width), char_height(height), m_renderer(renderer)
    {
        m_texture = IMG_LoadTexture(renderer, filename);
        if (m_texture == nullptr) {
            // TODO: throw if loading fails
        }

        SDL_QueryTexture(m_texture, NULL, NULL, &tex_width, &tex_height);
    }

    ~BitmapFont() {
        SDL_DestroyTexture(m_texture);
    }

    // Font pixel dimensions
    const int char_width, char_height;

    void drawChar(char c, int x, int y, int scale);
    void drawStr(char const* str, int x, int y, int scale);

private:
    SDL_Renderer* m_renderer;

    SDL_Texture* m_texture;
    int tex_width, tex_height;
};
