#ifndef INJECT_H
#define INJECT_H
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

#include "process_utils.h"

#define DETACHED 0
#define ATTACHED 1



int inject_shellcode(pid_t pid, void* shellcode, size_t shellcode_len, uint8_t options);
int inject_so(pid_t pid, void* shellcode, size_t shellcode_len, char* soPath, size_t soPath_len, uint8_t options);



#endif
