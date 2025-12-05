#include "keyboard.h"
#include "vga.h"
#include <arch/i686/io.h>
#include <arch/i686/pic.h>
#include <gui/wm.h>
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

static bool g_ShiftDown = false;
static bool g_CapsLock = false;

void keyboard_process_char(char c)
{
    wm_handle_key_event(c);
}

void keyboard_handler_c(Registers* regs)
{
    // EOI handled by IRQ handler wrapper usually, but here we are called from ISR stub.
    // We need to send EOI manually.

    uint8_t scancode = i686_inb(KEYBOARD_DATA_PORT);
    
    // Handle modifiers
    if (scancode == 0x2A || scancode == 0x36) g_ShiftDown = true;
    else if (scancode == 0xAA || scancode == 0xB6) g_ShiftDown = false;
    else if (scancode == 0x3A) g_CapsLock = !g_CapsLock;

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
            // Apply Shift/Caps logic
            if (c >= 'a' && c <= 'z')
            {
                if (g_ShiftDown ^ g_CapsLock)
                {
                    c = c - 32; // To uppercase
                }
            }
            // Basic symbol support
            else if (g_ShiftDown)
            {
                if (c == '1') c = '!';
                else if (c == '9') c = '(';
                else if (c == '0') c = ')';
                else if (c == '-') c = '_';
                else if (c == '=') c = '+';
                else if (c == '[') c = '{';
                else if (c == ']') c = '}';
                else if (c == ';') c = ':';
                else if (c == '\'') c = '"';
                else if (c == ',') c = '<';
                else if (c == '.') c = '>';
                else if (c == '/') c = '?';
            }

            wm_handle_key_event(c);
        }
    }

    // Send EOI to PIC (IRQ1 = 1)
    i686_PIC_SendEndOfInterrupt(1);
}

extern void isr_keyboard_stub();

void keyboard_init()
{
    printf("Keyboard Init...\n");
    // IRQ1 is mapped to interrupt 33 (0x21)
    i686_IDT_SetGate(33, isr_keyboard_stub, 0x08, IDT_FLAG_PRESENT | IDT_FLAG_RING0 | IDT_FLAG_GATE_32BIT_INT);
    i686_PIC_Unmask(1);
}
