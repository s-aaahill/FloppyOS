#include "shell.h"
#include <drivers/vga.h>
#include <arch/i686/io.h>
#include <string.h>
#include <stdbool.h>


void handle_line(const char* line)
{
    char command[16];
    char args[256];
    int i = 0;
    int j = 0;

    // Skip leading whitespace
    while (line[i] == ' ') i++;

    // Extract command
    while (line[i] != ' ' && line[i] != '\0' && j < 15)
    {
        command[j++] = line[i++];
    }
    command[j] = '\0';

    // Skip whitespace between command and args
    while (line[i] == ' ') i++;

    // Extract args (rest of the line)
    j = 0;
    while (line[i] != '\0' && j < 255)
    {
        args[j++] = line[i++];
    }
    args[j] = '\0';

    if (command[0] == '\0') return;

    if (strcmp(command, "help") == 0)
    {
        vga_puts("Available commands:\n");
        vga_puts("  help    - Show this help\n");
        vga_puts("  clear   - Clear screen\n");
        vga_puts("  echo    - Print arguments\n");
        vga_puts("  reboot  - Reboot system\n");
    }
    else if (strcmp(command, "clear") == 0)
    {
        vga_clrscr();
    }
    else if (strcmp(command, "echo") == 0)
    {
        vga_puts(args);
        vga_puts("\n");
    }
    else if (strcmp(command, "reboot") == 0)
    {
        vga_puts("Rebooting...\n");
        
        // Disable interrupts
        __asm__ volatile("cli");
        
        // Method 1: Keyboard controller pulse
        uint8_t good = 0x02;
        while (good & 0x02)
            good = i686_inb(0x64);
        i686_outb(0x64, 0xFE);
        
        // Method 2: Port 0x92 (Fast A20/Reset)
        // Bit 0: Fast Reset (1=Reset)
        i686_outb(0x92, i686_inb(0x92) | 1);
        
        // Halt if failed
        __asm__ volatile("hlt");
    }
    else
    {
        vga_puts("Unknown command. Type 'help'.\n");
    }
}
