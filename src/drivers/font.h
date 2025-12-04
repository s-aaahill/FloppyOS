#pragma once
#include <stdint.h>

void draw_char(uint32_t* buffer, int buf_w, int x, int y, char c, uint32_t fg, uint32_t bg);
void draw_text(uint32_t* buffer, int buf_w, int x, int y, const char* str, uint32_t fg, uint32_t bg);
