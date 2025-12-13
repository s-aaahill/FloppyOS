#include <stdint.h>
#include "stdio.h"
#include "memory.h"
#include <string.h>
#include <hal/hal.h>
#include <arch/i686/irq.h>
#include <arch/i686/io.h>
#include <drivers/keyboard.h>
#include <drivers/vga.h>
#include <drivers/fb.h>
#include <drivers/ps2mouse.h>
#include <gui/wm.h>

extern uint8_t __bss_start;
extern uint8_t __end;

void crash_me();

void timer(Registers* regs) {
    // Poll serial port for input (headless mode support)
    if (i686_inb(0x3F8 + 5) & 1)
    {
        char c = i686_inb(0x3F8);
        keyboard_process_char(c);
    }
}

void __attribute__((section(".entry"))) start(uint16_t bootDrive)
{
    memset(&__bss_start, 0, (&__end) - (&__bss_start));

    // Direct serial write to verify boot
    i686_outb(0x3F8, 'B'); i686_outb(0x3F8, 'o'); i686_outb(0x3F8, 'o'); i686_outb(0x3F8, 't'); i686_outb(0x3F8, '\n');

    HAL_Initialize();
    i686_IRQ_RegisterHandler(0, timer);

    clrscr();

    // Initialize GUI
    printf("Initializing Memory...\n");
    mem_init();

    printf("Initializing FB...\n");
    fb_init();
    printf("Initializing WM...\n");
    wm_init();
    printf("Initializing Mouse...\n");
    mouse_init();
    printf("Initializing Keyboard...\n");
    keyboard_init();
    
    // Start Apps
    void notepad_init();
    notepad_init();
    
    void terminal_init();
    terminal_init();

    void calculator_init();
    calculator_init();

    void snake_init();
    snake_init();
    
    void taskmgr_init();
    taskmgr_init();

    
    printf("Entering Terminal...\n");
    
    // Show splash screen
    gui_show_splash(5000);
    
    // Main GUI Loop
    int frame = 0;
    while (1)
    {
        wm_draw_desktop();
        wm_tick();
        
        // Simple delay/yield could go here
        // For now, just busy loop
        for (volatile int i = 0; i < 100000; i++);

        // Test Automation for Agent Verification
        frame++;
        if (frame == 50) {
            printf("TESTING: Automating Notepad Minimize...\n");
            // Notepad created at 50,50 with w=300.
            // Minimize btn approx at 50+300-12-4 = 334. Y = 50 - 20 + 4 = 34.
            // Click Down
            wm_handle_mouse_event(335, 35, 1);
            // Click Up
            wm_handle_mouse_event(335, 35, 0);
        }
        if (frame == 150) {
             printf("TESTING: Automating Taskbar Restore...\n");
             // Taskbar at bottom (480). Height 24. Y >= 456.
             // Item 0 (Notepad) at x approx 2+10 = 12.
             // Click Down
             wm_handle_mouse_event(15, 470, 1);
             // Click Up
             wm_handle_mouse_event(15, 470, 0);
        }
        if (frame == 200) {
            printf("TESTING: Complete. Exiting loop for log capture.\n");
            // In a real OS we wouldn't exit, but for this test environment we can loop forever or just print checkmark
        }
    }

end:
    for (;;);
}
