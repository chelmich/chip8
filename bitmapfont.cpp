#include "bitmapfont.hpp"

#include <SDL2/SDL_image.h>

BitmapFont::BitmapFont(SDL_Renderer* renderer, char const* filename, int char_width, int char_height)
    : char_width(char_width), char_height(char_height), m_renderer(renderer)
{
    m_texture = IMG_LoadTexture(renderer, filename);
    if (m_texture == nullptr) {
        // TODO: throw if loading fails
    }

    SDL_QueryTexture(m_texture, NULL, NULL, &m_tex_width, &m_tex_height);
}

BitmapFont::~BitmapFont() {
    SDL_DestroyTexture(m_texture);
}

void BitmapFont::draw_char(char c, int x, int y, int scale) const {
    if (c < '!' || c > '~') return; // skip non-printing characters

    int cols = m_tex_width / char_width;
    int rows = m_tex_height / char_height;

    int index = c - 32; // font bitmap skips first 32 ascii chars
    int col = index % cols;
    int row = index / cols;
    if (row > rows) return; // TODO: signal an error?

    SDL_Rect src_rect = {char_width * col, char_height * row, char_width, char_height};
    SDL_Rect dst_rect = {x, y, char_width * scale, char_height * scale};
    SDL_RenderCopy(m_renderer, m_texture, &src_rect, &dst_rect);
}

void BitmapFont::draw_str(char const* str, int x, int y, int scale) const {
    size_t len = strlen(str);
    int advance = (char_width + char_spacing) * scale;
    for (int i = 0; i < len; i++) {
        draw_char(str[i], x + advance * i, y, scale);
    }
}
