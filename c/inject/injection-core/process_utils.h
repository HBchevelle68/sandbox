#ifndef PROCESS_UTILS_H
#define PROCESS_UTILS_H
#include <sys/types.h>

unsigned long putils_findLibrary(const char *library, pid_t pid);
unsigned long putils_findExecAddr(pid_t pid);

#endif
