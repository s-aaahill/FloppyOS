#include <gui/wm.h>
#include <drivers/font.h>
#include <string.h>
#include <stdio.h>

static char calc_display[32] = "0";
static Window* calc_win = NULL;
static long long current_val = 0;
static long long pending_val = 0;
static char pending_op = 0;
static bool new_entry = true;

// Helper to convert string to int
long long str_to_int(const char* str) {
    long long res = 0;
    int sign = 1;
    int i = 0;
    if (str[0] == '-') { sign = -1; i++; }
    for (; str[i] != '\0'; ++i) {
        if (str[i] >= '0' && str[i] <= '9') {
            res = res * 10 + (str[i] - '0');
        }
    }
    return res * sign;
}

// Helper to convert int to string
void int_to_str(long long n, char* str) {
    if (n == 0) {
        strcpy(str, "0");
        return;
    }
    
    int i = 0;
    int sign = 0;
    if (n < 0) {
        sign = 1;
        n = -n;
    }
    
    char temp[32];
    while (n > 0) {
        temp[i++] = (n % 10) + '0';
        n /= 10;
    }
    if (sign) temp[i++] = '-';
    
    int len = i;
    for (int j = 0; j < len; j++) {
        str[j] = temp[len - 1 - j];
    }
    str[len] = '\0';
}

void calc_paint(Window* win)
{
    // Fill background
    for (int i = 0; i < win->w * win->h; i++) win->backbuffer[i] = 0xFFE0E0E0;
    
    // Draw Display
    for (int i = 10; i < 40; i++) {
        for (int j = 10; j < win->w - 10; j++) {
            win->backbuffer[i * win->w + j] = 0xFFFFFFFF;
        }
    }
    draw_text(win->backbuffer, win->w, 15, 20, calc_display, 0xFF000000, 0xFFFFFFFF);
    
    // Draw Buttons (Grid)
    const char* buttons[] = {
        "7", "8", "9", "+",
        "4", "5", "6", "-",
        "1", "2", "3", "C",
        "=", "0", ".", ""
    };
    
    int start_x = 10;
    int start_y = 50;
    int btn_w = 30;
    int btn_h = 25;
    int gap = 5;
    
    for (int i = 0; i < 16; i++) {
        const char* label = buttons[i];
        if (strlen(label) == 0) continue;
        
        int row = i / 4;
        int col = i % 4;
        
        int x = start_x + col * (btn_w + gap);
        int y = start_y + row * (btn_h + gap);
        
        // Draw button rect
        for (int by = y; by < y + btn_h; by++) {
            for (int bx = x; bx < x + btn_w; bx++) {
                win->backbuffer[by * win->w + bx] = 0xFFC0C0C0;
            }
        }
        // Draw label
        draw_text(win->backbuffer, win->w, x + 10, y + 8, label, 0xFF000000, 0xFFC0C0C0);
    }
}

void calc_mouse(Window* win, int x, int y, uint8_t buttons)
{
    static uint8_t last_buttons = 0;
    if ((buttons & 1) && !(last_buttons & 1)) { // Click
        int start_x = 10;
        int start_y = 50;
        int btn_w = 30;
        int btn_h = 25;
        int gap = 5;
        
        const char* labels[] = {
            "7", "8", "9", "+",
            "4", "5", "6", "-",
            "1", "2", "3", "C",
            "=", "0", ".", ""
        };
        
        for (int i = 0; i < 16; i++) {
            const char* label = labels[i];
            if (strlen(label) == 0) continue;
            
            int row = i / 4;
            int col = i % 4;
            
            int bx = start_x + col * (btn_w + gap);
            int by = start_y + row * (btn_h + gap);
            
            if (x >= bx && x < bx + btn_w && y >= by && y < by + btn_h) {
                char ch = label[0];
                
                if (ch >= '0' && ch <= '9') {
                    if (new_entry) {
                        strcpy(calc_display, label);
                        new_entry = false;
                    } else {
                        if (strcmp(calc_display, "0") == 0) {
                            strcpy(calc_display, label);
                        } else if (strlen(calc_display) < 15) {
                            strcat(calc_display, label);
                        }
                    }
                }
                else if (ch == 'C') {
                    strcpy(calc_display, "0");
                    pending_val = 0;
                    pending_op = 0;
                    new_entry = true;
                }
                else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') { // * and / not in grid yet but good to handle
                     // Grid has + and -
                     // Update: The grid definition in `calc_paint` was:
                     // "7", "8", "9", "+",
                     // "4", "5", "6", "-",
                     // "1", "2", "3", "C",
                     // "=", "0", ".", ""
                     
                     pending_val = str_to_int(calc_display);
                     pending_op = ch;
                     new_entry = true;
                }
                else if (ch == '=') {
                    current_val = str_to_int(calc_display);
                    long long res = current_val;
                    if (pending_op == '+') res = pending_val + current_val;
                    else if (pending_op == '-') res = pending_val - current_val;
                    // Add minimal * and / support if we ever add buttons, though defaults only had + -
                    // Let's stick to what's in the UI logic or assumed.
                    
                    int_to_str(res, calc_display);
                    pending_val = 0;
                    pending_op = 0;
                    new_entry = true;
                }
                else if (ch == '.') {
                    // Ignore for integer MVP
                }
            }
        }
    }
    last_buttons = buttons;
}

void calculator_init()
{
    calc_win = wm_create_window(350, 100, 160, 200, "Calc");
    if (calc_win) {
        calc_win->on_paint = calc_paint;
        calc_win->on_mouse = calc_mouse;
    }
}
