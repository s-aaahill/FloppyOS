#include "shell.h"
#include <drivers/vga.h>
#include <arch/i686/io.h>
#include <stdbool.h>

// Simple string functions
int strcmp(const char* s1, const char* s2)
{
    while(*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, int n)
{
    while(n > 0 && *s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

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
        // Keyboard controller reset
        uint8_t good = 0x02;
        while (good & 0x02)
            good = i686_inb(0x64);
        i686_outb(0x64, 0xFE);
        // Halt if failed
        __asm__ volatile("hlt");
    }
    else
    {
        vga_puts("Unknown command. Type 'help'.\n");
    }
}
