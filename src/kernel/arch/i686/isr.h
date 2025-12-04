#pragma once
#include <stdint.h>

typedef struct {

    // In the reverse order they are pushed
    uint32_t ds;                                           // Data Segment pushed by me
    uint32_t edi, esi, ebp, useless, ebx, edx, ecx, eax;   // push a
    uint32_t interrupt, error;                             // Pushing interrupt, error is pushed automatically (or dummy)
    uint32_t eip, cs, eflags, esp, ss;                     // Pushed automatically by cpu 
} __attribute__((packed)) Registers;

typedef void (*ISRHandler)(Registers* regs);

void i686_ISR_Initialize();
void i686_ISR_RegisterHandler(int interrupt, ISRHandler handler);