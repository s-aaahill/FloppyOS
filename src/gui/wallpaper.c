#include "wallpaper.h"
#include "wallpaper_data.h"
#include <drivers/fb.h>

void wallpaper_render(int width, int height)
{
    // Draw tiles
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int tile_x = x % WALLPAPER_TILE_SIZE;
            int tile_y = y % WALLPAPER_TILE_SIZE;
            uint32_t color = wallpaper_tile[tile_y * WALLPAPER_TILE_SIZE + tile_x];
            fb_set_pixel(x, y, color);
        }
    }
}


