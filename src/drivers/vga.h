#pragma once
#include <stdint.h>

void vga_putc(char c);
void vga_puts(const char* str);
void vga_clrscr();
void vga_set_cursor(int x, int y);
void vga_set_putc_hook(void (*hook)(char));
void vga_set_clrscr_hook(void (*hook)());
