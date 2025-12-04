#include "font.h"
#include "font8x8.h"

void draw_char(uint32_t* buffer, int buf_w, int x, int y, char c, uint32_t fg, uint32_t bg)
{
    const uint8_t* glyph = font8x8_basic[(int)c];
    for (int cy = 0; cy < 8; cy++)
    {
        for (int cx = 0; cx < 8; cx++)
        {
            if (glyph[cy] & (1 << (7 - cx)))
            {
                buffer[(y + cy) * buf_w + (x + cx)] = fg;
            }
            else
            {
                buffer[(y + cy) * buf_w + (x + cx)] = bg;
            }
        }
    }
}

void draw_text(uint32_t* buffer, int buf_w, int x, int y, const char* str, uint32_t fg, uint32_t bg)
{
    while (*str)
    {
        draw_char(buffer, buf_w, x, y, *str, fg, bg);
        x += 8;
        str++;
    }
}
