BITS 64

section .text
global _start
_start:
    mov rax, 60            ; sys_exit
    mov rdi, 0             ; 0
    syscall