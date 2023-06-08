/**
 * @file This is where instructor code should go. This solution
 * is not intended to be perfect or usable for all situations.
 * Merely it should be written well enough to assist future
 * instructors  in teaching as well as students to understand
 * the material that is WITHIN the scope of the class.
 *
 * To prevent any leakage, lets keep to some guidlines:
 * - Functions follow the format "instructor_<your function>"
 * - Functions with the exception of instructor_load and instructor_jump
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

static int is_elf64(Elf64_Ehdr *hdr)
{
    int result = 0;
    // Check its an ELF
    if (!strncmp((char *)hdr->e_ident, "\177ELF", 4))
    {
        printf("[+] ELF Magic Match\n");
        // Check its also 64-bit
        if (ELFCLASS64 == hdr->e_ident[EI_CLASS])
        {
            printf("[+] ELF is 64-bit\n");
            result = 1;
        }
        else
        {
            printf("[-] 32-bit ELF's not supported");
        }
    }
    else
    {
        printf("[-] ELFMAGIC mismatch!\n");
    }

    return result;
}

static int parse_elf_hdr() {}

uint64_t instructor_load(uint8_t *fdata, size_t size)
{
    uint64_t addr = 0;
    Elf64_Ehdr *hdr = (Elf64_Ehdr *)fdata;

    printf("** Begin Loading Elf **\n");

    if (0 == is_elf64((Elf64_Ehdr *)fdata))
    {
        ADDR_ZERO_THEN_DONE;
    }

    printf("[+] Entry point: 0x%X\n", hdr->e_entry);

done:
    printf("** End Loading Elf **\n");
    return addr;
}

int instructor_jump(uint64_t)
{

    return 0;
}