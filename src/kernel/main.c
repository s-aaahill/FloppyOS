#include <stdint.h>
#include "stdio.h"
#include "memory.h"

extern uint8_t __bss_start;
extern uint8_t __end;

void __attribute__((section(".entry"))) start(uint16_t bootDrive)
{
    memset(&__bss_start, 0, (&__end) - (&__bss_start));

    clrscr();

    printf("Control Flow in Kernel\nFloppyOS under development!!!\nCompleted till here on 3rd December 2025\n");

end:
    for (;;);
}
