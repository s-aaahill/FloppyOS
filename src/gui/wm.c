#include "wm.h"
#include <drivers/fb.h>
#include <drivers/font.h>
#include <drivers/ps2mouse.h>
#include <drivers/keyboard.h> // For keyboard_process_char hook
#include <stddef.h>
#include <string.h>

static Window g_Windows[MAX_WINDOWS];
static int g_WindowCount = 0;
static Window* g_FocusedWindow = NULL;

// Simple static buffers for windows (MVP constraint: no malloc)
#define MAX_WINDOW_PIXELS (640 * 480) 
static uint32_t g_WindowBuffers[MAX_WINDOWS][MAX_WINDOW_PIXELS];

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
    win->backbuffer = g_WindowBuffers[g_WindowCount];
    win->has_focus = false;
    win->on_paint = NULL;
    win->on_key = NULL;
    win->on_mouse = NULL;
    
    // Clear backbuffer
    for (int i = 0; i < w * h; i++) win->backbuffer[i] = 0xFFFFFFFF; // White background

    g_WindowCount++;
    wm_set_focus(win);
    return win;
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
    
    int tx = win->x + 4;
    int ty = win->y - TITLE_BAR_HEIGHT + 4;

}

void wm_draw_desktop()
{
    // Draw background
    fb_fill_rect(0, 0, fb_width(), fb_height(), 0xFF008080); // Teal background

    // Draw windows
    for (int i = 0; i < g_WindowCount; i++)
    {
        Window* win = &g_Windows[i];
        
        // Draw title bar
        uint32_t title_color = win->has_focus ? 0xFF000080 : 0xFF808080;
        fb_fill_rect(win->x, win->y - TITLE_BAR_HEIGHT, win->w, TITLE_BAR_HEIGHT, title_color);
        
        
        // Draw window content
        if (win->on_paint) win->on_paint(win);
        fb_blit(win->backbuffer, win->x, win->y, win->w, win->h);
        
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
        // Check for window click (reverse order for z-order)
        for (int i = g_WindowCount - 1; i >= 0; i--)
        {
            Window* win = &g_Windows[i];
            // Check title bar
            if (x >= win->x && x < win->x + win->w &&
                y >= win->y - TITLE_BAR_HEIGHT && y < win->y)
            {
                wm_set_focus(win);
                g_IsDragging = true;
                g_DragOffsetX = x - win->x;
                g_DragOffsetY = y - win->y;
                break;
            }
            // Check content
            if (x >= win->x && x < win->x + win->w &&
                y >= win->y && y < win->y + win->h)
            {
                wm_set_focus(win);
                if (win->on_mouse) win->on_mouse(win, x - win->x, y - win->y, buttons);
                break;
            }
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
