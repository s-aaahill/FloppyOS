#include "wallpaper.h"
#include "wallpaper_data.h"
#include <drivers/fb.h>

void wallpaper_render(int screen_width, int screen_height)
{
    BMPHeader* header = (BMPHeader*)_binary_courtyard_fixed_bmp_start;
    BMPInfoHeader* info_header = (BMPInfoHeader*)(_binary_courtyard_fixed_bmp_start + sizeof(BMPHeader));

    uint32_t* pixel_data = (uint32_t*)(_binary_courtyard_fixed_bmp_start + header->offset);
    int width = info_header->width;
    int height = info_header->height;

    // Draw wallpaper, tiling if necessary
    for (int y = 0; y < screen_height; y++)
    {
        for (int x = 0; x < screen_width; x++)
        {
            // Calculate texture coordinates with wrapping
            int tex_x = x % width;
            int tex_y = y % height;

            // BMPs are stored bottom-up
            int bmp_y = height - 1 - tex_y;
            
            // 24-bit BMP usually, or 32-bit. Assuming 32-bit XRGB based on earlier commands (TrueColor) or need to check bits.
            // The user command used `objcopy ... courtyard_fixed.bmp wallpaper.o`
            // and `convert ... -type TrueColor ...` which usually means 24-bit or 32-bit.
            // Let's check info_header->bits during runtime if possible, but here we assume a specific format for simplicity first.
            // Accessing byte-wise to be safe if it's 24-bit packed.
            
            uint8_t* pixel = ((uint8_t*)pixel_data) + (bmp_y * ((width * info_header->bits / 8 + 3) & ~3)) + (tex_x * info_header->bits / 8);
            
            uint32_t color = 0;
            if (info_header->bits == 24) {
                 // BGR
                 color = 0xFF000000 | (pixel[2] << 16) | (pixel[1] << 8) | pixel[0];
            } else if (info_header->bits == 32) {
                 // BGRA or BGRX
                 color = 0xFF000000 | (pixel[2] << 16) | (pixel[1] << 8) | pixel[0];
            }

            fb_set_pixel(x, y, color);
        }
    }
}


