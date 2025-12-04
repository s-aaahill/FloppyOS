#include <gui/wm.h>
#include <drivers/font.h>
#include <drivers/fb.h>
#include <string.h>

#define NOTEPAD_BUF_SIZE 1024
static char notepad_buffer[NOTEPAD_BUF_SIZE];
static int notepad_cursor = 0;

void notepad_paint(Window* win)
{
    // Fill background
    for (int i = 0; i < win->w * win->h; i++) win->backbuffer[i] = 0xFFFFFFFF;
    
    // Draw text
    draw_text(win->backbuffer, win->w, 5, 5, notepad_buffer, 0xFF000000, 0xFFFFFFFF);
    
    // Draw cursor
    // Calculate cursor position (very simple, no wrapping for MVP)
    int cx = 5 + (notepad_cursor * 8);
    int cy = 5;
    // ...
}

void notepad_key(Window* win, char c)
{
    if (c == '\b')
    {
        if (notepad_cursor > 0)
        {
            notepad_cursor--;
            notepad_buffer[notepad_cursor] = '\0';
        }
    }
    else if (c >= 32 && c <= 126)
    {
        if (notepad_cursor < NOTEPAD_BUF_SIZE - 1)
        {
            notepad_buffer[notepad_cursor++] = c;
            notepad_buffer[notepad_cursor] = '\0';
        }
    }
}

void notepad_init()
{
    memset(notepad_buffer, 0, NOTEPAD_BUF_SIZE);
    strcpy(notepad_buffer, "Welcome to Notepad!");
    notepad_cursor = strlen(notepad_buffer);
    
    Window* win = wm_create_window(50, 50, 300, 200, "Notepad");
    if (win)
    {
        win->on_paint = notepad_paint;
        win->on_key = notepad_key;
    }
}
