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
    
    // Draw text with manual newline handling
    int x = 5;
    int y = 5;
    int cursor_x = 5;
    int cursor_y = 5;

    for (int i = 0; i < strlen(notepad_buffer); i++) {
        // Capture cursor position if this is the current cursor index
        if (i == notepad_cursor) {
            cursor_x = x;
            cursor_y = y;
        }

        char c = notepad_buffer[i];
        if (c == '\n') {
            x = 5;
            y += 10; // Move down 10 pixels for new line
        } else {
            draw_char(win->backbuffer, win->w, x, y, c, 0xFF000000, 0xFFFFFFFF);
            x += 8;
        }
    }

    // If cursor is at the end of the buffer
    if (notepad_cursor == strlen(notepad_buffer)) {
        cursor_x = x;
        cursor_y = y;
    }
    
    // Draw cursor
    for (int i = 0; i < 10; i++) {
        int idx = (cursor_y + i) * win->w + cursor_x;
        if (idx >= 0 && idx < win->w * win->h) {
             win->backbuffer[idx] = 0xFF000000;
        }
    }
}

void notepad_key(Window* win, char c)
{
    if (c == '\b')
    {
        if (notepad_cursor > 0)
        {
            // Simple backspace: move everything back one char
            // This is O(N) but fine for a small notepad
            for (int i = notepad_cursor - 1; i < strlen(notepad_buffer); i++) {
                notepad_buffer[i] = notepad_buffer[i+1];
            }
            notepad_cursor--;
        }
    }
    else if (c == '\n' || c == '\r') // Handle Enter Key
    {
        if (notepad_cursor < NOTEPAD_BUF_SIZE - 1)
        {
            // Insert newline
            // Move existing text forward to make space
            for (int i = strlen(notepad_buffer); i >= notepad_cursor; i--) {
                notepad_buffer[i+1] = notepad_buffer[i];
            }
            notepad_buffer[notepad_cursor++] = '\n';
        }
    }
    else if (c >= 32 && c <= 126)
    {
        if (notepad_cursor < NOTEPAD_BUF_SIZE - 1)
        {
            // Insert character
             for (int i = strlen(notepad_buffer); i >= notepad_cursor; i--) {
                notepad_buffer[i+1] = notepad_buffer[i];
            }
            notepad_buffer[notepad_cursor++] = c;
        }
    }
}

void notepad_init()
{
    memset(notepad_buffer, 0, NOTEPAD_BUF_SIZE);
    strcpy(notepad_buffer, "Start Typing on Your NotePad!");
    notepad_cursor = strlen(notepad_buffer);
    
    Window* win = wm_create_window(50, 50, 300, 200, "Notepad");
    if (win)
    {
        win->on_paint = notepad_paint;
        win->on_key = notepad_key;
    }
}
