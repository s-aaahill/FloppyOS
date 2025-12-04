#pragma once
#include <stdint.h>

void fb_init();
int fb_width();
int fb_height();
void fb_set_pixel(int x, int y, uint32_t color);
void fb_fill_rect(int x, int y, int w, int h, uint32_t color);
void fb_blit(uint32_t* buffer, int x, int y, int w, int h);
void fb_swap_buffers();
