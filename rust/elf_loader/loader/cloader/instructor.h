#ifndef INSTRUCTOR_H
#define INSTRUCTOR_H
#include <stdint.h>
#include <stdlib.h>

#define ADDR_0_THEN_DONE \
    do                   \
    {                    \
        addr = 0;        \
        goto done;       \
    } while (0)

uint64_t instructor_load(uint8_t *fdata, size_t size);
int instructor_jump(uint64_t);

#endif
