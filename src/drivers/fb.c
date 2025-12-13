#include "fb.h"
#include <arch/i686/io.h>
#include <stdio.h>
#include <mm/memstats.h>



// Bochs Graphics Adapter (BGA) ports
#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA  0x01CF

#define VBE_DISPI_INDEX_ID          0
#define VBE_DISPI_INDEX_XRES        1
#define VBE_DISPI_INDEX_YRES        2
#define VBE_DISPI_INDEX_BPP         3
#define VBE_DISPI_INDEX_ENABLE      4
#define VBE_DISPI_INDEX_BANK        5
#define VBE_DISPI_INDEX_VIRT_WIDTH  6
#define VBE_DISPI_INDEX_VIRT_HEIGHT 7
#define VBE_DISPI_INDEX_X_OFFSET    8
#define VBE_DISPI_INDEX_Y_OFFSET    9

#define VBE_DISPI_DISABLED          0x00
#define VBE_DISPI_ENABLED           0x01
#define VBE_DISPI_LFB_ENABLED       0x40

#define FB_WIDTH  800
#define FB_HEIGHT 600
#define FB_BPP    32

static uint32_t* g_FrameBuffer = (uint32_t*)0xFD000000; // VRAM
static uint32_t* g_BackBuffer = (uint32_t*)0x00A00000;  // RAM Backbuffer (at 10MB)

void bga_write_register(uint16_t index, uint16_t value)
{
    i686_outw(VBE_DISPI_IOPORT_INDEX, index);
    i686_outw(VBE_DISPI_IOPORT_DATA, value);
}

uint16_t bga_read_register(uint16_t index)
{
    i686_outw(VBE_DISPI_IOPORT_INDEX, index);
    return i686_inw(VBE_DISPI_IOPORT_DATA);
}

void fb_init()
{
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_write_register(VBE_DISPI_INDEX_XRES, FB_WIDTH);
    bga_write_register(VBE_DISPI_INDEX_YRES, FB_HEIGHT);
    bga_write_register(VBE_DISPI_INDEX_BPP, FB_BPP);
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
    
    uint16_t version = bga_read_register(VBE_DISPI_INDEX_ID);
    printf("BGA Version: 0x%x\n", version);

    // Track memory
    // Default 800x600x32bpp = 1,920,000 bytes.
    // Plus backbuffer at 10MB (static in fb.c but it consumes RAM).
    uint32_t fb_size = FB_WIDTH * FB_HEIGHT * 4;
    // We have VRAM (hardware) and Backbuffer (RAM).
    // The prompt says: "Calculate framebuffer_bytes = width x height x bytes_per_pixel. Add to used_ram_bytes..."
    // "If a desktop background buffer exists, count it as framebuffer memory as well."
    // In fb.c, g_BackBuffer is at 0x00A00000 (10MB mark). This is part of system RAM.
    
    // We count the system RAM backbuffer.
    memstats.framebuffer_bytes += fb_size;
    memstats.used_ram_bytes += fb_size;
    memstats.free_ram_bytes -= fb_size;
}

int fb_width() { return FB_WIDTH; }
int fb_height() { return FB_HEIGHT; }

void fb_set_pixel(int x, int y, uint32_t color)
{
    if (x >= 0 && x < FB_WIDTH && y >= 0 && y < FB_HEIGHT)
    {
        g_BackBuffer[y * FB_WIDTH + x] = color;
    }
}

void fb_fill_rect(int x, int y, int w, int h, uint32_t color)
{
    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            fb_set_pixel(x + i, y + j, color);
        }
    }
}

void fb_blit(uint32_t* buffer, int x, int y, int w, int h)
{
    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            if (x + i >= 0 && x + i < FB_WIDTH && y + j >= 0 && y + j < FB_HEIGHT)
            {
                g_BackBuffer[(y + j) * FB_WIDTH + (x + i)] = buffer[j * w + i];
            }
        }
    }
}

void fb_swap_buffers()
{

    uint32_t* src = g_BackBuffer;
    uint32_t* dst = g_FrameBuffer;
    int count = FB_WIDTH * FB_HEIGHT;
    
    while (count--)
    {
        *dst++ = *src++;
    }
}

uint32_t* fb_get_backbuffer()
{
    return g_BackBuffer;
}
