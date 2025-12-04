#include "ps2mouse.h"
#include <arch/i686/io.h>
#include <arch/i686/pic.h>
#include <arch/i686/idt.h>
#include "fb.h"

#define MOUSE_PORT_DATA 0x60
#define MOUSE_PORT_STATUS 0x64
#define MOUSE_PORT_CMD 0x64

int g_MouseX = 0;
int g_MouseY = 0;
uint8_t g_MouseButtons = 0;

static uint8_t g_MouseCycle = 0;
static int8_t g_MouseByte[3];

void mouse_wait(uint8_t type)
{
    uint32_t timeout = 100000;
    if (type == 0)
    {
        while (timeout--) // Data
        {
            if ((i686_inb(MOUSE_PORT_STATUS) & 1) == 1)
                return;
        }
        return;
    }
    else
    {
        while (timeout--) // Signal
        {
            if ((i686_inb(MOUSE_PORT_STATUS) & 2) == 0)
                return;
        }
        return;
    }
}

void mouse_write(uint8_t write)
{
    mouse_wait(1);
    i686_outb(MOUSE_PORT_CMD, 0xD4);
    mouse_wait(1);
    i686_outb(MOUSE_PORT_DATA, write);
}

uint8_t mouse_read()
{
    mouse_wait(0);
    return i686_inb(MOUSE_PORT_DATA);
}

extern void isr_mouse_stub();

void mouse_init()
{
    uint8_t status;

    // Enable auxiliary device
    mouse_wait(1);
    i686_outb(MOUSE_PORT_CMD, 0xA8);

    // Enable interrupts
    mouse_wait(1);
    i686_outb(MOUSE_PORT_CMD, 0x20);
    mouse_wait(0);
    status = (i686_inb(MOUSE_PORT_DATA) | 2);
    mouse_wait(1);
    i686_outb(MOUSE_PORT_CMD, 0x60);
    mouse_wait(1);
    i686_outb(MOUSE_PORT_DATA, status);

    // Use default settings
    mouse_write(0xF6);
    mouse_read();  // Acknowledge

    // Enable streaming
    mouse_write(0xF4);
    mouse_read();  // Acknowledge

    // Register ISR
    // IRQ12 is mapped to interrupt 44 (0x2C)
    i686_IDT_SetGate(44, isr_mouse_stub, 0x08, IDT_FLAG_PRESENT | IDT_FLAG_RING0 | IDT_FLAG_GATE_32BIT_INT);
    i686_PIC_Unmask(12);
    
    g_MouseX = fb_width() / 2;
    g_MouseY = fb_height() / 2;
}

void mouse_handler(Registers* regs)
{
    // printf("Mouse IRQ\n");
    uint8_t status = i686_inb(MOUSE_PORT_STATUS);
    if (!(status & 0x20)) {
        // Not mouse data
        i686_PIC_SendEndOfInterrupt(12);
        return;
    }

    uint8_t mouse_in = i686_inb(MOUSE_PORT_DATA);

    switch (g_MouseCycle)
    {
        case 0:
            if ((mouse_in & 0x08) == 0x08) // Check bit 3
            {
                g_MouseByte[0] = mouse_in;
                g_MouseCycle++;
            }
            break;
        case 1:
            g_MouseByte[1] = mouse_in;
            g_MouseCycle++;
            break;
        case 2:
            g_MouseByte[2] = mouse_in;
            g_MouseCycle = 0;

            g_MouseButtons = g_MouseByte[0] & 0x07;
            g_MouseX += g_MouseByte[1];
            g_MouseY -= g_MouseByte[2];

            if (g_MouseX < 0) g_MouseX = 0;
            if (g_MouseY < 0) g_MouseY = 0;
            if (g_MouseX >= fb_width()) g_MouseX = fb_width() - 1;
            if (g_MouseY >= fb_height()) g_MouseY = fb_height() - 1;
            
            // Notify WM
            wm_handle_mouse_event(g_MouseX, g_MouseY, g_MouseButtons);
            break;
    }
    
    i686_PIC_SendEndOfInterrupt(12);
}
