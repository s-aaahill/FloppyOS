#pragma once
#include <arch/i686/irq.h>

void keyboard_init();
void keyboard_handler_c(Registers* regs);
void keyboard_process_char(char c);
