[bits 32]
global isr_mouse_stub
extern mouse_handler

isr_mouse_stub:
    pushad
    call mouse_handler
    popad
    iretd
