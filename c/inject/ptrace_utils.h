#ifndef PTRACE_UTILS_H
#define PTRACE_UTILS_H

#include <stdint.h>
#include <stdlib.h>
#include <sys/user.h>

typedef struct user_regs_struct registers_t;

// See ptrace(2) man pages for details

int ptraceAttach(pid_t pid, int* out_status);
int ptraceDetach(pid_t pid);
int ptraceRead(pid_t pid, unsigned long addr, void *out_ptr, uint32_t len);
int ptraceWrite(pid_t pid, unsigned long addr, void *buff_ptr, uint32_t len);
int ptraceGetRegs(pid_t pid, registers_t* regs);
int ptraceSetRegs(pid_t pid, registers_t* regs);
int ptraceCont(pid_t pid, int verifySIGTRAP, int* status);

#endif