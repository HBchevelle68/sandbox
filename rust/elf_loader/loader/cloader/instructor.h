#ifndef INSTRUCTOR_H
#define INSTRUCTOR_H
#include <stdint.h>
#include <stdlib.h>

#define ADDR_ZERO_THEN_DONE \
    do                      \
    {                       \
        addr = 0;           \
        goto done;          \
    } while (0)

//
typedef struct instructor_elf
{
    uint64_t entry_point;
} ielf_t;

uint64_t instructor_load(uint8_t *fdata, size_t size);
int instructor_jump(uint64_t);

#endif
