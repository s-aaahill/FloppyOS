#include <gui/wm.h>
#include <drivers/font.h>
#include <mm/memstats.h>
#include <stdio.h>
#include <string.h>

static Window* g_TaskMgrWin = NULL;

void taskmgr_paint(Window* win)
{
    // Background color (e.g., standard dialog gray/white)
    int w = win->w;
    int h = win->h;
    uint32_t* buf = win->backbuffer;
    
    // Fill background
    for(int i=0; i<w*h; i++) buf[i] = 0xFFF0F0F0;
    
    // Helper to draw info
    int y = 5;
    int x = 5;
    int line_h = 14;
    
    char line[64];
    
    // Title Section
    draw_text(buf, w, x, y, "System Memory Stats", 0xFF000000, 0xFFF0F0F0);
    y += line_h * 2;
    
    // Total RAM
    sprintf(line, "Total RAM: %d KB", memstats.total_ram_bytes / 1024);
    draw_text(buf, w, x, y, line, 0xFF000000, 0xFFF0F0F0);
    y += line_h;
    
    // Used RAM
    sprintf(line, "Used RAM:  %d KB", memstats.used_ram_bytes / 1024);
    draw_text(buf, w, x, y, line, 0xFF000000, 0xFFF0F0F0);
    y += line_h;
    
    // Free RAM
    sprintf(line, "Free RAM:  %d KB", memstats.free_ram_bytes / 1024);
    draw_text(buf, w, x, y, line, 0xFF000000, 0xFFF0F0F0);
    y += line_h * 2;
    
    // Breakdown Section
    draw_text(buf, w, x, y, "Breakdown", 0xFF000000, 0xFFF0F0F0);
    y += line_h;
    
    sprintf(line, "Kernel:      %d KB", memstats.kernel_static_bytes / 1024);
    draw_text(buf, w, x, y, line, 0xFF000000, 0xFFF0F0F0);
    y += line_h;
    
    sprintf(line, "Framebuffer: %d KB", memstats.framebuffer_bytes / 1024);
    draw_text(buf, w, x, y, line, 0xFF000000, 0xFFF0F0F0);
    y += line_h;
    
    sprintf(line, "Backbuffers: %d KB", memstats.backbuffer_bytes / 1024);
    draw_text(buf, w, x, y, line, 0xFF000000, 0xFFF0F0F0);
    y += line_h;
    
    sprintf(line, "Heap Alloc:  %d KB", memstats.heap_bytes / 1024);
    draw_text(buf, w, x, y, line, 0xFF000000, 0xFFF0F0F0);
    y += line_h;
    
    // Simple bar graph for usage
    y += 10;
    int bar_w = w - 20;
    int bar_h = 10;
    // Outline
    for(int bj=0; bj<bar_h; bj++) {
        for(int bi=0; bi<bar_w; bi++) {
             // Border
             if(bj==0 || bj==bar_h-1 || bi==0 || bi==bar_w-1) {
                 win->backbuffer[(y+bj)*w + (x+bi)] = 0xFF000000;
             } else {
                 win->backbuffer[(y+bj)*w + (x+bi)] = 0xFFFFFFFF;
             }
        }
    }
    
    // Fill
    if (memstats.total_ram_bytes > 0) {
        float pct = (float)memstats.used_ram_bytes / (float)memstats.total_ram_bytes;
        int fill_w = (int)((bar_w - 2) * pct);
        for(int bj=1; bj<bar_h-1; bj++) {
            for(int bi=1; bi<=fill_w; bi++) {
                win->backbuffer[(y+bj)*w + (x+bi)] = 0xFF000080; // Blue
            }
        }
    }
}

void taskmgr_tick(Window* win)
{
    // Update periodically
    static int tick_count = 0;
    tick_count++;
    if (tick_count > 10) { // Approx 500ms if tick is fast?
         // wm_tick -> 500ms constraint.
         // We assume wm_tick call rate. If main loop has "volatile loop", it's slow.
         // Let's just force repaint every tick for "live" feel, or every few depending on loop speed.
         // In main.c loop, we have a delay.
         // Let's rely on main loop calling wm_tick.
         tick_count = 0;
         // Invalidate isn't implemented in WM yet for this MVP, but wm_draw_desktop 
         // calls on_paint every frame if we want OR it blits every frame.
         // Current wm_draw_desktop calls on_paint EVERY frame (see line 159 of wm.c).
         // So we don't strictly need invalidate. The paint function is called every loop iteration.
    }
}

void taskmgr_init()
{
    g_TaskMgrWin = wm_create_window(100, 100, 200, 300, "Task Manager");
    if (g_TaskMgrWin)
    {
        g_TaskMgrWin->on_paint = taskmgr_paint;
        g_TaskMgrWin->on_tick = taskmgr_tick;
    }
}
