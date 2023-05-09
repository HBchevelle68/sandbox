/**
 * @file This is where instructor code should go. This solution
 * is not intended to be perfect or usable for all situations.
 * Merely it should be written well enough to assist future
 * instructors  in teaching as well as students to understand
 * the material that is WITHIN the scope of the class.
 *
 * To prevent any leakage, lets keep to some guidlines:
 * - ALL functions must follow the formate "instructor_<your function>"
 * - ALL functions with the exception of instructor_load and instructor_jump
 * should be static functions. This prevents accidental usage outside
 * their intended bounds
 *
 */
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <elf.h>
#include <string.h>

#include "instructor.h"

static int is_elf(Elf64_Ehdr *hdr)
{
    // Found online, slightly modified
    if (!strncmp((char *)hdr->e_ident, "\177ELF", 4))
    {
        // Is ELF
        printf("[+] ELFMAGIC Match!\n");
        return 1;
    }
    printf("[-] ELFMAGIC mismatch!\n");
    return 0;
}

uint64_t instructor_load(uint8_t *fdata, size_t size)
{
    uint64_t addr = 0;

    printf("** Begin Loading Elf **\n");

    if (0 == is_elf((Elf64_Ehdr *)fdata))
    {
        ADDR_0_THEN_DONE;
    }

done:
    return addr;
}

int instructor_jump(uint64_t)
{

    return 0;
}