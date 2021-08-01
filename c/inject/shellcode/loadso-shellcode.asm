BITS 64

section .text
global _start

_start:
    nop
    nop
    nop
    nop
    ;mov rsi, 1 ; RTLD_LAZY
    ;;int 0x03   ; SIGTRAP
    call rax   ; dlopen()
    int 0x03   ; SIGTRAP