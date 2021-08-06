#include "bitmapfont.hpp"

void BitmapFont::drawChar(char c, int x, int y, int scale) {
    int cols = tex_width / char_width;
    int rows = tex_height / char_height;

    int index = c - 32; // font bitmap skips first 32 ascii chars
    SDL_Rect src_rect = {
        char_width * (index % cols),
        char_height * (index / cols),
        char_width, char_height};

    SDL_Rect dst_rect = {x, y, char_width * scale, char_height * scale};

    SDL_RenderCopy(m_renderer, m_texture, &src_rect, &dst_rect);
}

void BitmapFont::drawStr(char const* str, int x, int y, int scale) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        drawChar(str[i], x + (char_width * scale * i), y, scale);
    }
}
