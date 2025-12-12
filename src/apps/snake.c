#include <gui/wm.h>
#include <gui/wm.h>
#include <drivers/font.h>
#include <drivers/fb.h>
#include <string.h>
#include <stdio.h>
#include <hal/hal.h> /
#define GRID_W 28
#define GRID_H 20
#define TILE_SIZE 8
#define MAX_SNAKE_LEN 1024

typedef struct {
    int x, y;
} Point;

typedef enum { UP, RIGHT, DOWN, LEFT } Direction;

typedef struct {
    Point body[MAX_SNAKE_LEN];
    int length;
    Direction dir;
    Direction next_dir; // Prevents 180 turn in same tick
    bool alive;
    bool paused;
    Point food;
    int score;
    uint32_t speed_ms; // tick interval
    uint32_t last_tick; // simple counter
    uint32_t seed;
} SnakeGame;

static SnakeGame game;

// Simple random generator
static int rand_next() {
    game.seed = game.seed * 1103515245 + 12345;
    return (unsigned int)(game.seed / 65536) % 32768;
}

static void spawn_food() {
    bool on_snake = true;
    while (on_snake) {
        game.food.x = rand_next() % GRID_W;
        game.food.y = rand_next() % GRID_H;
        on_snake = false;
        for (int i = 0; i < game.length; i++) {
            if (game.body[i].x == game.food.x && game.body[i].y == game.food.y) {
                on_snake = true;
                break;
            }
        }
    }
}

static void game_reset() {
    game.length = 3;
    game.body[0].x = GRID_W / 2;
    game.body[0].y = GRID_H / 2;
    game.body[1].x = GRID_W / 2;
    game.body[1].y = GRID_H / 2 + 1; // Tail down
    game.body[2].x = GRID_W / 2;
    game.body[2].y = GRID_H / 2 + 2; 
    
    game.dir = UP;
    game.next_dir = UP;
    game.alive = true;
    game.paused = false;
    game.score = 0;
    game.speed_ms = 200; // Lower is faster. If main loop is fast, this might need tuning.
    // If we don't have real MS time, we treat this as "ticks to skip"
    game.last_tick = 0;
    
    spawn_food();
}

void snake_on_paint(Window* win) {
    // 1. Clear Background
    for (int i = 0; i < win->w * win->h; i++) win->backbuffer[i] = 0xFF202020; // Dark grey bg

    int offset_x = (win->w - (GRID_W * TILE_SIZE)) / 2;
    int offset_y = (win->h - (GRID_H * TILE_SIZE)) / 2 + 10; // Shift down for score

    // 2. Draw Score
    char score_str[32];
    int score_val = game.score;
    // Simple int to str
    int idx = 0;
    if (score_val == 0) score_str[idx++] = '0';
    else {
        char temp[10];
        int t_i = 0;
        while(score_val > 0) { temp[t_i++] = (score_val % 10) + '0'; score_val /= 10; }
        while(t_i > 0) score_str[idx++] = temp[--t_i];
    }
    score_str[idx] = '\0';
    
    draw_text(win->backbuffer, win->w, 5, 5, "Score: ", 0xFFFFFFFF, 0xFF202020);
    draw_text(win->backbuffer, win->w, 55, 5, score_str, 0xFFFFFFFF, 0xFF202020);
    
    if (game.paused) {
         draw_text(win->backbuffer, win->w, 120, 5, "[PAUSED]", 0xFFFFFF00, 0xFF202020);
    }
    if (!game.alive) {
         draw_text(win->backbuffer, win->w, 120, 5, "[GAME OVER - R to Restart]", 0xFFFF0000, 0xFF202020);
    }

    // 3. Draw Grid Boundary (Optional, but nice)
    // Draw rectangle around grid
    int bx = offset_x - 1;
    int by = offset_y - 1;
    int bw = GRID_W * TILE_SIZE + 2;
    int bh = GRID_H * TILE_SIZE + 2;
    // Top
    for(int x=bx; x<bx+bw; x++) if(by>=0 && by<win->h && x>=0 && x<win->w) win->backbuffer[by*win->w+x] = 0xFF808080;
    // Bottom
    for(int x=bx; x<bx+bw; x++) if((by+bh)<win->h && x>=0 && x<win->w) win->backbuffer[(by+bh)*win->w+x] = 0xFF808080;
    // Left
    for(int y=by; y<by+bh; y++) if(y>=0 && y<win->h && bx>=0 && bx<win->w) win->backbuffer[y*win->w+bx] = 0xFF808080;
    // Right
    for(int y=by; y<by+bh; y++) if(y>=0 && y<win->h && (bx+bw)<win->w) win->backbuffer[y*win->w+(bx+bw)] = 0xFF808080;

    // 4. Draw Food
    int fx = offset_x + game.food.x * TILE_SIZE;
    int fy = offset_y + game.food.y * TILE_SIZE;
    for(int dy = 1; dy < TILE_SIZE-1; dy++) {
        for(int dx = 1; dx < TILE_SIZE-1; dx++) {
            win->backbuffer[(fy+dy)*win->w + (fx+dx)] = 0xFFFF0000; // Red Food
        }
    }

    // 5. Draw Snake
    for (int i = 0; i < game.length; i++) {
        int sx = offset_x + game.body[i].x * TILE_SIZE;
        int sy = offset_y + game.body[i].y * TILE_SIZE;
        uint32_t color = (i == 0) ? 0xFF00FF00 : 0xFF008000; // Bright green head, dark body
        
        for(int dy = 0; dy < TILE_SIZE; dy++) {
            for(int dx = 0; dx < TILE_SIZE; dx++) {
                if (sx + dx >= 0 && sx + dx < win->w && sy + dy >= 0 && sy + dy < win->h)
                   win->backbuffer[(sy+dy)*win->w + (sx+dx)] = color;
            }
        }
    }
}

// Since we may not have a reliable ms timer in this simple environment, 
// we will increment a counter every call. 
// Assuming main loop spins reasonably fast or has a delay, we tune strictness.
// For now, let's treat 'speed' as number of 'wm_tick' calls to skip.
// If main loop has "volatile check", it might be slow.
void snake_tick(Window* win) {
    if (!game.alive || game.paused) return;

    game.last_tick++;
    // Arbitrary tuning: If main loop is fast, 200 ticks might is ok. 
    // If it's slow (delay loop 100000), then maybe 5-10 ticks is enough.
    // The main loop in main.c has a 100,000 iteration delay.
    // That suggests the tick rate is not super high (maybe 10-50Hz?).
    // Let's try update every 5 ticks initially.
    if (game.last_tick < 5) return; 
    game.last_tick = 0;

    // Logic
    game.dir = game.next_dir;
    
    // Debug log for verification
    // printf("Snake Tick: Head (%d,%d) Food (%d,%d) Len %d\n", game.body[0].x, game.body[0].y, game.food.x, game.food.y, game.length);

    Point next_head = game.body[0];
    switch (game.dir) {
        case UP: next_head.y--; break;
        case DOWN: next_head.y++; break;
        case LEFT: next_head.x--; break;
        case RIGHT: next_head.x++; break;
    }

    // Collision Wall
    if (next_head.x < 0 || next_head.x >= GRID_W || next_head.y < 0 || next_head.y >= GRID_H) {
        game.alive = false;
        // Trigger repaint to show game over
        // But we don't have invalidate(), so just rely on WM redrawing everything if it does?
        // WM draws desktop every frame, calling on_paint. So state change is sufficient.
        return;
    }

    // Collision Self
    for (int i = 0; i < game.length; i++) {
        if (game.body[i].x == next_head.x && game.body[i].y == next_head.y) {
            // Note: Tail moves, so colliding with tail tip is fine unless we grow.
            // But simple logic: don't hit any part.
             game.alive = false;
             return;
        }
    }

    // Move
    // Shift body
    for (int i = game.length; i > 0; i--) {
        game.body[i] = game.body[i-1];
    }
    game.body[0] = next_head;

    // Eat Food
    if (next_head.x == game.food.x && next_head.y == game.food.y) {
        if (game.length < MAX_SNAKE_LEN) game.length++;
        game.score += 10;
        spawn_food();
    } else {
        // If not ate, tail was already shifted. Wait, if we didn't grow, the last segment (at old length) 
        // is now garbage index? No, we shifted up to length. 
        // Example: len 3. indices 0,1,2.
        // Shift: 3=2, 2=1, 1=0. 0=new.
        // If not grow, we define length as 3. We disregard index 3.
        // Correct.
    }
}

void snake_on_key(Window* win, char c) {
    if (c == 'r' || c == 'R') {
        game_reset();
        return;
    }
    if (c == 'p' || c == 'P') {
        game.paused = !game.paused;
        return;
    }
    if (!game.alive) return;

    // Input
    // 'w' = 119, 'a' = 97, 's' = 115, 'd' = 100
    // Uppercase too?
    // Arrows might be mapped to specific chars or escape codes. 
    // WM doesn't seem to pass scan codes, just chars.
    // Assuming WASD for now as per request.
    
    if (c == 'w' || c == 'W') { if (game.dir != DOWN) game.next_dir = UP; }
    if (c == 's' || c == 'S') { if (game.dir != UP) game.next_dir = DOWN; }
    if (c == 'a' || c == 'A') { if (game.dir != RIGHT) game.next_dir = LEFT; }
    if (c == 'd' || c == 'D') { if (game.dir != LEFT) game.next_dir = RIGHT; }
}

void snake_init() {
    game.seed = 12345; 
    game_reset();
    
    // 28*8 = 224, 20*8 = 160.
    // Window need margins. 240x190 seems safe.
    Window* win = wm_create_window(60, 60, 240, 190, "Snake");
    if (win) {
        win->on_paint = snake_on_paint;
        win->on_key = snake_on_key;
        win->on_tick = snake_tick;
    }
}
