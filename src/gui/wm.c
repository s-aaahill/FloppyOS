#include "wm.h"
#include "wallpaper.h"
#include <drivers/fb.h>
#include <drivers/font.h>
#include <drivers/ps2mouse.h>
#include <drivers/keyboard.h> // For keyboard_process_char hook
#include <mm/memstats.h>
#include <kernel/memory.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

static Window g_Windows[MAX_WINDOWS];
static int g_WindowCount = 0;
static Window* g_FocusedWindow = NULL;

// Dynamic buffers
// static uint32_t g_WindowBuffers[MAX_WINDOWS][MAX_WINDOW_PIXELS]; // Removed static


#define MAX_TASKBAR_ITEMS 10
struct Taskbar {
    Window* items[MAX_TASKBAR_ITEMS];
    int count;
    int height;
};

static struct Taskbar g_Taskbar = { .count = 0, .height = 24 };

void wm_init()
{
    g_WindowCount = 0;
    g_FocusedWindow = NULL;
}

Window* wm_create_window(int x, int y, int w, int h, const char* title)
{
    if (g_WindowCount >= MAX_WINDOWS) return NULL;
    
    Window* win = &g_Windows[g_WindowCount];
    win->x = x;
    win->y = y;
    win->w = w;
    win->h = h;
    strncpy(win->title, title, 31);
    
    // Allocate backbuffer
    uint32_t size = w * h * 4;
    win->backbuffer = (uint32_t*)kalloc_raw(size);
    
    // Track Stats
    memstats.backbuffer_bytes += size;
    memstats.used_ram_bytes += size;
    memstats.free_ram_bytes -= size;

    win->has_focus = false;
    win->on_paint = NULL;
    win->on_key = NULL;
    win->on_mouse = NULL;
    win->on_tick = NULL;
    
    // Clear backbuffer
    if (win->backbuffer) {
         for (int i = 0; i < w * h; i++) win->backbuffer[i] = 0xFFFFFFFF; // White background
    }

    win->minimized = false;
    win->taskbar_index = -1;

    g_WindowCount++;
    wm_set_focus(win);
    return win;
}

void wm_minimize(Window* win)
{
    if (!win || win->minimized) return;
    
    if (g_Taskbar.count < MAX_TASKBAR_ITEMS)
    {
        printf("WM: Minimizing Window '%s'\n", win->title);
        win->minimized = true;
        win->taskbar_index = g_Taskbar.count;
        g_Taskbar.items[g_Taskbar.count++] = win;
        
        // If the minimized window had focus, focus something else (or nothing)
        if (win->has_focus)
        {
             win->has_focus = false;
             g_FocusedWindow = NULL;
             // Try to focus top-most non-minimized window
             for (int i = g_WindowCount - 1; i >= 0; i--) {
                 if (!g_Windows[i].minimized) {
                     wm_set_focus(&g_Windows[i]);
                     break;
                 }
             }
        }
    }
}

void wm_restore(Window* win)
{
    if (!win || !win->minimized) return;
    
    printf("WM: Restoring Window '%s'\n", win->title);
    
    // Remove from taskbar
    int idx = win->taskbar_index;
    if (idx != -1) {
        for (int i = idx; i < g_Taskbar.count - 1; i++) {
            g_Taskbar.items[i] = g_Taskbar.items[i+1];
            g_Taskbar.items[i]->taskbar_index = i;
        }
        g_Taskbar.count--;
    }
    
    win->minimized = false;
    win->taskbar_index = -1;
    wm_set_focus(win);
}

void wm_set_focus(Window* win)
{
    if (g_FocusedWindow) g_FocusedWindow->has_focus = false;
    g_FocusedWindow = win;
    if (g_FocusedWindow) g_FocusedWindow->has_focus = true;
    
    // Move to top of stack (simple approach: just redraw last)
    // For proper z-ordering we would reorder the array, but for MVP we just keep track of focus.
}

void wm_draw_window_frame(Window* win)
{
    // Draw title bar
    uint32_t title_color = win->has_focus ? 0xFF000080 : 0xFF808080;
    fb_fill_rect(win->x, win->y - TITLE_BAR_HEIGHT, win->w, TITLE_BAR_HEIGHT, title_color);
    
    // Draw minimize button (small square on right)
    int btn_size = 12;
    int btn_x = win->x + win->w - btn_size - 4;
    int btn_y = win->y - TITLE_BAR_HEIGHT + 4;
    fb_fill_rect(btn_x, btn_y, btn_size, btn_size, 0xFFC0C0C0);
    // Draw a small line inside to look like minimize symbol
    fb_fill_rect(btn_x + 2, btn_y + btn_size - 4, btn_size - 4, 2, 0xFF000000);

    int tx = win->x + 4;
    int ty = win->y - TITLE_BAR_HEIGHT + 4;
    draw_text(fb_get_backbuffer(), fb_width(), tx, ty, win->title, 0xFFFFFFFF, title_color);
}


#define DESKTOP_BG_COLOR 0xFF000080

void wm_draw_desktop()
{
    // Draw background
    // Draw background
    // fb_fill_rect(0, 0, fb_width(), fb_height(), DESKTOP_BG_COLOR); // Soft pastel background
    wallpaper_render(fb_width(), fb_height());

    // Draw windows
    for (int i = 0; i < g_WindowCount; i++)
    {
        Window* win = &g_Windows[i];
        if (win->minimized) continue;
        
        // Draw title bar
        wm_draw_window_frame(win);

        // Draw window content
        if (win->on_paint) win->on_paint(win);
        fb_blit(win->backbuffer, win->x, win->y, win->w, win->h);
        
    }
    
    // Draw Taskbar
    int tb_y = fb_height() - g_Taskbar.height;
    fb_fill_rect(0, tb_y, fb_width(), g_Taskbar.height, 0xFFC0C0C0); // Gray background
    
    // Draw taskbar items
    int item_w = 100;
    int item_margin = 2;
    for (int i = 0; i < g_Taskbar.count; i++) {
        Window* win = g_Taskbar.items[i];
        int item_x = i * (item_w + item_margin) + item_margin;
        
        fb_fill_rect(item_x, tb_y + item_margin, item_w, g_Taskbar.height - 2*item_margin, 0xFFFFFFFF);
        // Draw title (clipped if necessary, but for now just draw)
        // Clip text length for display
        char display_title[16];
        strncpy(display_title, win->title, 15);
        display_title[15] = '\0';
        
        draw_text(fb_get_backbuffer(), fb_width(), item_x + 4, tb_y + 8, display_title, 0xFF000000, 0xFFFFFFFF);
    }
    
    // Draw mouse cursor
    fb_fill_rect(g_MouseX, g_MouseY, 5, 5, 0xFFFFFFFF);
    fb_set_pixel(g_MouseX, g_MouseY, 0xFF000000);
    
    fb_swap_buffers();
}

static int g_DragOffsetX, g_DragOffsetY;
static bool g_IsDragging = false;

void wm_handle_mouse_event(int x, int y, uint8_t buttons)
{
    static uint8_t last_buttons = 0;
    
    if ((buttons & 1) && !(last_buttons & 1)) // Left click
    {
        // 1. Check Taskbar Click
        int tb_y = fb_height() - g_Taskbar.height;
        if (y >= tb_y)
        {
             // Check which item
             int item_w = 100;
             int item_margin = 2;
             for (int i = 0; i < g_Taskbar.count; i++) {
                int item_x = i * (item_w + item_margin) + item_margin;
                if (x >= item_x && x < item_x + item_w)
                {
                    // Restore this window
                    wm_restore(g_Taskbar.items[i]);
                    return; // Handled
                }
             }
        }
    
        // 2. Check for window click (reverse order for z-order)
        bool handled = false;
        for (int i = g_WindowCount - 1; i >= 0; i--)
        {
            Window* win = &g_Windows[i];
            if (win->minimized) continue; // Skip minimized windows
            
            // Check title bar
            if (x >= win->x && x < win->x + win->w &&
                y >= win->y - TITLE_BAR_HEIGHT && y < win->y)
            {
                // Check Minimize Button
                int btn_size = 12;
                int btn_x = win->x + win->w - btn_size - 4;
                int btn_y = win->y - TITLE_BAR_HEIGHT + 4;
                
                if (x >= btn_x && x < btn_x + btn_size &&
                    y >= btn_y && y < btn_y + btn_size)
                {
                    wm_minimize(win);
                    handled = true;
                    break;
                }
                
                wm_set_focus(win);
                g_IsDragging = true;
                g_DragOffsetX = x - win->x;
                g_DragOffsetY = y - win->y;
                handled = true;
                break;
            }
            // Check content
            if (x >= win->x && x < win->x + win->w &&
                y >= win->y && y < win->y + win->h)
            {
                wm_set_focus(win);
                if (win->on_mouse) win->on_mouse(win, x - win->x, y - win->y, buttons);
                handled = true;
                break;
            }
        }
        
        // If clicked on desktop background (not handled by any window), unfocus
        if (!handled && y < tb_y)
        {
             if (g_FocusedWindow) g_FocusedWindow->has_focus = false;
             g_FocusedWindow = NULL;
        }
    }
    
    if (!(buttons & 1))
    {
        g_IsDragging = false;
    }
    
    if (g_IsDragging && g_FocusedWindow)
    {
        g_FocusedWindow->x = x - g_DragOffsetX;
        g_FocusedWindow->y = y - g_DragOffsetY;
    }
    
    last_buttons = buttons;
}

void wm_handle_key_event(char c)
{
    if (g_FocusedWindow && g_FocusedWindow->on_key)
    {
        g_FocusedWindow->on_key(g_FocusedWindow, c);
    }
}

void wm_tick()
{
    for (int i = 0; i < g_WindowCount; i++)
    {
        Window* win = &g_Windows[i];
        if (!win->minimized && win->on_tick)
        {
            win->on_tick(win);
        }
    }
}

void gui_show_splash(int ms)
{
    //printf("Showing Splash Screen for %d ms...\n", ms);
    // Fill background
    fb_fill_rect(0, 0, fb_width(), fb_height(), DESKTOP_BG_COLOR);
    
    // Draw centered text
    const char* title = "FloppyOS";
    const char* subtitle = "Loading...";
    
    // Simple centering math (assuming 8x8 font)
    int title_x = (fb_width() - (strlen(title) * 8)) / 2;
    int title_y = (fb_height() / 2) - 20;
    
    int sub_x = (fb_width() - (strlen(subtitle) * 8)) / 2;
    int sub_y = (fb_height() / 2) + 10;
    
    // Use black text for title, dark gray for subtitle
    draw_text(fb_get_backbuffer(), fb_width(), title_x, title_y, title, 0xFF000000, DESKTOP_BG_COLOR);
    draw_text(fb_get_backbuffer(), fb_width(), sub_x, sub_y, subtitle, 0xFF404040, DESKTOP_BG_COLOR);
    
    fb_swap_buffers();
    
    // Fallback busy-wait loop
    // Calibrate roughly for QEMU/real hardware. 
    // A simple volatile loop prevents optimization.
    volatile int count = 0;
    // 2000ms * some constant. 
    // Let's assume a fairly large number of iterations per ms.
    int iterations = ms * 100000; 
    for (int i = 0; i < iterations; i++)
    {
        count++;
    }
    //printf("Booting...\n");
}
