#ifndef INSTRUCTOR_H
#define INSTRUCTOR_H
#include <stdint.h>
#include <stdlib.h>

// BEHOLD!!!!!
// MY LAZINESS! :)
#define ADDR_ZERO_THEN_DONE \
    do                      \
    {                       \
        addr = 0;           \
        goto done;          \
    } while (0)

#define FREE_IF_VALID(ptr) \
    do                     \
    {                      \
        if (NULL != ptr)   \
        {                  \
            free(ptr);     \
            ptr = NULL;    \
        }                  \
    } while (0)

#define IF_NULL_ERROR_THEN_DONE(ptr) \
    do                               \
    {                                \
        if (NULL == ptr)             \
        {                            \
            result = 1;              \
            goto done;               \
        }                            \
    } while (0)

/**
 * @brief Instructor struct to make life a bit easier
 * and to remove the need for duplicative parsing.
 *
 */
typedef struct instructor_elf
{
    Elf64_Phdr *phdr_table;
    Elf64_Shdr *shdr_table;
} ielf_t;

uint64_t instructor_load(uint8_t *fdata, size_t size);
int instructor_jump();
void instructor_clean();

#endif
