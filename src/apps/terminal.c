#include <gui/wm.h>
#include <drivers/font.h>
#include <drivers/fb.h>
#include <string.h>
#include <drivers/keyboard.h> // For handle_line

// We need to hook into the shell output.
// For MVP, we can just maintain a buffer here and have the shell write to it?
// Or we can just simulate a terminal that sends commands to shell and displays output.
// But the shell writes to VGA/Serial.
// We should modify the shell/kernel to write to a "console" abstraction.
// For this task, "When shell prints output, render it inside the terminal window’s backbuffer."
// This implies we need a way to capture stdout.

#define TERM_ROWS 25
#define TERM_COLS 40
static char term_buffer[TERM_ROWS][TERM_COLS + 1];
static int term_row = 0;
static int term_col = 0;

// Current command buffer
static char cmd_buffer[256];
static int cmd_index = 0;

void terminal_write_char(char c)
{
    if (c == '\n')
    {
        term_row++;
        term_col = 0;
    }
    else if (c == '\b')
    {
        if (term_col > 0) term_col--;
    }
    else if (c >= 32 && c <= 126)
    {
        if (term_col < TERM_COLS)
        {
            term_buffer[term_row][term_col++] = c;
            term_buffer[term_row][term_col] = '\0'; // Null terminate for safety
        }
    }
    
    if (term_row >= TERM_ROWS)
    {
        // Scroll
        for (int i = 0; i < TERM_ROWS - 1; i++)
        {
            strcpy(term_buffer[i], term_buffer[i+1]);
        }
        memset(term_buffer[TERM_ROWS-1], 0, TERM_COLS + 1);
        term_row = TERM_ROWS - 1;
    }
}

void terminal_write_string(const char* str)
{
    while (*str) terminal_write_char(*str++);
}

void terminal_paint(Window* win)
{
    // Fill background (Black)
    for (int i = 0; i < win->w * win->h; i++) win->backbuffer[i] = 0xFF000000;
    
    // Draw text
    for (int i = 0; i < TERM_ROWS; i++)
    {
        draw_text(win->backbuffer, win->w, 2, 2 + (i * 10), term_buffer[i], 0xFF00FF00, 0xFF000000);
    }
    
    // Draw prompt/cursor line
    char line[TERM_COLS + 5];
    strcpy(line, "> ");
    strcat(line, cmd_buffer);
    draw_text(win->backbuffer, win->w, 2, 2 + (term_row * 10), line, 0xFF00FF00, 0xFF000000);
}

// Forward declaration
void handle_line(const char* line);

void terminal_key(Window* win, char c)
{
    if (c == '\n')
    {
        terminal_write_char('\n');
        terminal_write_string("> ");
        terminal_write_string(cmd_buffer);
        terminal_write_char('\n');
        
        // Execute command
        // We need to capture output of handle_line.
        // This is tricky without redirecting stdout.
        // For MVP, let's just execute it and hope it prints to serial/VGA, 
        // and maybe we can hook vga_putc to also call terminal_write_char?
        
        handle_line(cmd_buffer);
        
        cmd_index = 0;
        memset(cmd_buffer, 0, 256);
    }
    else if (c == '\b')
    {
        if (cmd_index > 0)
        {
            cmd_index--;
            cmd_buffer[cmd_index] = '\0';
        }
    }
    else if (c >= 32 && c <= 126)
    {
        if (cmd_index < 255)
        {
            cmd_buffer[cmd_index++] = c;
            cmd_buffer[cmd_index] = '\0';
        }
    }
}

void terminal_init()
{
    for (int i = 0; i < TERM_ROWS; i++) memset(term_buffer[i], 0, TERM_COLS + 1);
    
    Window* win = wm_create_window(400, 50, 320, 260, "Terminal");
    if (win)
    {
        win->on_paint = terminal_paint;
        win->on_key = terminal_key;
    }
}
