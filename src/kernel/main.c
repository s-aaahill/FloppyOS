#include <stdint.h>
#include "stdio.h"
#include "memory.h"
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
    
    printf("Entering GUI Loop...\n");
    // Main GUI Loop
    while (1)
    {
        wm_draw_desktop();
        
        // Simple delay/yield could go here
        // For now, just busy loop
    }

end:
    for (;;);
}
