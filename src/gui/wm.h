#pragma once
#include <stdint.h>
#include <stdbool.h>

#define MAX_WINDOWS 10
#define TITLE_BAR_HEIGHT 20

typedef struct Window Window;

typedef void (*PaintCallback)(Window* win);
typedef void (*KeyCallback)(Window* win, char c);
typedef void (*MouseCallback)(Window* win, int x, int y, uint8_t buttons);

struct Window {
    int x, y, w, h;
    char title[32];
    uint32_t* backbuffer;
    bool has_focus;
    
    PaintCallback on_paint;
    KeyCallback on_key;
    MouseCallback on_mouse;
};

void wm_init();
Window* wm_create_window(int x, int y, int w, int h, const char* title);
void wm_draw_desktop();
void wm_handle_mouse_event(int x, int y, uint8_t buttons);
void wm_handle_key_event(char c);
void wm_set_focus(Window* win);
void gui_show_splash(int ms);
