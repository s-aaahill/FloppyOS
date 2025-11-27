#include "stdio.h"
#include "stdint.h"

void _cdecl cstart_ (uint16_t bootDrive) {
    puts("Bootloader in C!!\n");
    for(;;);
}