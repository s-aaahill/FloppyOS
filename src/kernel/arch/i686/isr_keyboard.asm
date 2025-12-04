[bits 32]
global isr_keyboard_stub
extern keyboard_handler_c

isr_keyboard_stub:
    pushad
    call keyboard_handler_c
    popad
    iretd
