#include "keyboard.h"
#include "vga.h"
#include <arch/i686/io.h>
#include <arch/i686/irq.h>
#include <arch/i686/idt.h>
#include <arch/i686/pic.h>
#include <shell/shell.h>
#include <stdbool.h>

#define KEYBOARD_DATA_PORT 0x60

static char g_LineBuffer[256];
static int g_LineIndex = 0;

// Simple US QWERTY scancode map (incomplete, just basics)
// 0x00 - 0x39
static const char g_ScancodeToChar[] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

void keyboard_process_char(char c)
{
    if (c == '\n' || c == '\r')
    {
        vga_putc('\n');
        g_LineBuffer[g_LineIndex] = '\0';
        handle_line(g_LineBuffer);
        g_LineIndex = 0;
        vga_puts("FloppyOS> "); 
    }
    else if (c == '\b' || c == 127) // Handle backspace and delete
    {
        if (g_LineIndex > 0)
        {
            vga_putc('\b');
            g_LineIndex--;
        }
    }
    else
    {
        if (g_LineIndex < 255)
        {
            vga_putc(c);
            g_LineBuffer[g_LineIndex++] = c;
        }
    }
}

void keyboard_handler_c(Registers* regs)
{
    // EOI handled by IRQ handler wrapper usually, but here we are called from ISR stub.
    // We need to send EOI manually.
    // i686_outb(0x20, 0x20); // This is what SendEndofInterrupt does for master PIC.

    uint8_t scancode = i686_inb(KEYBOARD_DATA_PORT);

    if (scancode & 0x80)
    {
        // Key release, ignore
    }
    else
    {
        char c = 0;
        if (scancode < sizeof(g_ScancodeToChar))
        {
            c = g_ScancodeToChar[scancode];
        }

        if (c)
        {
            keyboard_process_char(c);
        }
    }

    // Send EOI to PIC (IRQ1 = 1)
    i686_PIC_SendEndOfInterrupt(1);
}

extern void isr_keyboard_stub();

void keyboard_init()
{
    // IRQ1 is mapped to interrupt 33 (0x21)
    i686_IDT_SetGate(33, isr_keyboard_stub, 0x08, IDT_FLAG_PRESENT | IDT_FLAG_RING0 | IDT_FLAG_GATE_32BIT_INT);
    i686_PIC_Unmask(1);
}
