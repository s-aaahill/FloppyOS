#pragma once
#include <stdint.h>
#include <arch/i686/irq.h>

void mouse_init();
void mouse_handler(Registers* regs);

extern int g_MouseX;
extern int g_MouseY;
extern uint8_t g_MouseButtons;
