#include "vga.h"
#include <arch/i686/io.h>

const unsigned VGA_WIDTH = 80;
const unsigned VGA_HEIGHT = 25;
const uint8_t VGA_DEFAULT_COLOR = 0x07;

uint8_t* g_VgaBuffer = (uint8_t*)0xB8000;
int g_VgaX = 0, g_VgaY = 0;

void vga_putentry(int x, int y, char c, uint8_t color)
{
    const int index = 2 * (y * VGA_WIDTH + x);
    g_VgaBuffer[index] = c;
    g_VgaBuffer[index + 1] = color;
}

void vga_set_cursor(int x, int y)
{
    int pos = y * VGA_WIDTH + x;
    i686_outb(0x3D4, 0x0F);
    i686_outb(0x3D5, (uint8_t)(pos & 0xFF));
    i686_outb(0x3D4, 0x0E);
    i686_outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void vga_clrscr()
{
    for (int y = 0; y < VGA_HEIGHT; y++)
        for (int x = 0; x < VGA_WIDTH; x++)
            vga_putentry(x, y, '\0', VGA_DEFAULT_COLOR);

    g_VgaX = 0;
    g_VgaY = 0;
    vga_set_cursor(g_VgaX, g_VgaY);
}

void vga_scroll(int lines)
{
    for (int y = lines; y < VGA_HEIGHT; y++)
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            const int src_index = 2 * (y * VGA_WIDTH + x);
            const int dst_index = 2 * ((y - lines) * VGA_WIDTH + x);
            g_VgaBuffer[dst_index] = g_VgaBuffer[src_index];
            g_VgaBuffer[dst_index + 1] = g_VgaBuffer[src_index + 1];
        }

    for (int y = VGA_HEIGHT - lines; y < VGA_HEIGHT; y++)
        for (int x = 0; x < VGA_WIDTH; x++)
            vga_putentry(x, y, '\0', VGA_DEFAULT_COLOR);

    g_VgaY -= lines;
}

void vga_putc(char c)
{
    // Mirror to serial port 0x3F8 for headless testing
    while ((i686_inb(0x3F8 + 5) & 0x20) == 0);
    i686_outb(0x3F8, c);

    switch (c)
    {
        case '\n':
            g_VgaX = 0;
            g_VgaY++;
            // Serial also needs \r
            while ((i686_inb(0x3F8 + 5) & 0x20) == 0);
            i686_outb(0x3F8, '\r');
            break;
    
        case '\t':
            for (int i = 0; i < 4 - (g_VgaX % 4); i++)
                vga_putc(' ');
            break;

        case '\r':
            g_VgaX = 0;
            break;

        case '\b':
            if (g_VgaX > 0)
            {
                g_VgaX--;
            }
            else if (g_VgaY > 0)
            {
                g_VgaY--;
                g_VgaX = VGA_WIDTH - 1;
            }
            vga_putentry(g_VgaX, g_VgaY, '\0', VGA_DEFAULT_COLOR);
            break;

        default:
            vga_putentry(g_VgaX, g_VgaY, c, VGA_DEFAULT_COLOR);
            g_VgaX++;
            break;
    }

    if (g_VgaX >= VGA_WIDTH)
    {
        g_VgaX = 0;
        g_VgaY++;
    }

    if (g_VgaY >= VGA_HEIGHT)
    {
        vga_scroll(1);
    }

    vga_set_cursor(g_VgaX, g_VgaY);
}

void vga_puts(const char* str)
{
    while(*str)
    {
        vga_putc(*str);
        str++;
    }
}
