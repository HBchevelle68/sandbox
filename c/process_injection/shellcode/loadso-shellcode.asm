BITS 64

section .text
global _start

_start:
; On 64-bit linux systems ptrace read/writes
; are word aligned. Word size on these systems
; is 8 bytes. Keeping code at 8 byte alignment
; will make life much easier and safer :) 
    nop
    nop
    nop
    nop
    call rax ; dlopen()
    int 0x03 ; SIGTRAP