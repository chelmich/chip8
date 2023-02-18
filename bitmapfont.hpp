#pragma once

#include <SDL2/SDL_render.h>

class BitmapFont {
public:
    BitmapFont(SDL_Renderer *renderer, char const *filename, int char_width, int char_height);

    ~BitmapFont();

    // Font pixel dimensions
    const int char_width, char_height;

    void draw_char(char c, int x, int y, int scale) const;
    void draw_str(char const* str, int x, int y, int scale) const;

private:
    SDL_Renderer* m_renderer;

    SDL_Texture* m_texture;
    int m_tex_width, m_tex_height;
};
