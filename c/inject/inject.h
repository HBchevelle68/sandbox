#ifndef INJECT_H
#define INJECT_H
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

int inject_shellcode(pid_t pid, void* shellcode, size_t shellcode_len, uint8_t options);


#endif
