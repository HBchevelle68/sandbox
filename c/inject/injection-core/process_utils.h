#ifndef PROCESS_UTILS_H
#define PROCESS_UTILS_H
#include <sys/types.h>

unsigned long findLibrary(const char *library, pid_t pid);
unsigned long findExecAddr(pid_t pid);

#endif
