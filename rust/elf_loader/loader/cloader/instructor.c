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
#include <sys/mman.h>
#include <errno.h>

#include "instructor.h"

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS 0x20 /* Don't use a file.  */
#endif

// Lazy global
// Grab what we need,
static ielf_t elf_to_load;

// TODO
// Keeping this? Or just inline assembly?
// Should this be shared with students??
// Entry point of loaded binary as a func ptr
void (*foo)(void);

// Helps keep track of mmap'd segments
typedef struct MappedAddr
{
    void *addr;
    size_t size;
} maddr_t;

// Lets avoid yet another memory alloc
// Just keep this big enough to not worry
#define MAXADDRS 100
static maddr_t mapped_addrs[MAXADDRS];
static size_t addr_count;

/**
 * TODO document
 */
static int add_mapping(void *addr, size_t size)
{
    int result = 0;
    if (MAXADDRS == addr_count)
    {
        printf("[!] Max mappable segments reached!\n");
        result = -1;
        goto done;
    }

    mapped_addrs[addr_count].addr = addr;
    mapped_addrs[addr_count].size = size;
    addr_count++;

done:
    return result;
}

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
/**
 * @brief Print 64-bit elf header. Parses/supports more options
 * than we need. Taken from "TheCodeArtist" github. Thanks friend.
 */
static void print_elf64_header(Elf64_Ehdr *e64_hdr)
{

    /* Storage capacity class */
    printf("Storage class\t= ");
    switch (e64_hdr->e_ident[EI_CLASS])
    {
    case ELFCLASS32:
        printf("32-bit objects\n");
        break;

    case ELFCLASS64:
        printf("64-bit objects\n");
        break;

    default:
        printf("INVALID CLASS\n");
        break;
    }

    /* Data Format */
    printf("Data format\t= ");
    switch (e64_hdr->e_ident[EI_DATA])
    {
    case ELFDATA2LSB:
        printf("2's complement, little endian\n");
        break;

    case ELFDATA2MSB:
        printf("2's complement, big endian\n");
        break;

    default:
        printf("INVALID Format\n");
        break;
    }

    /* OS ABI */
    // Not needed but it was free
    printf("OS ABI\t\t= ");
    switch (e64_hdr->e_ident[EI_OSABI])
    {
    case ELFOSABI_SYSV:
        printf("UNIX System V ABI\n");
        break;

    case ELFOSABI_HPUX:
        printf("HP-UX\n");
        break;

    case ELFOSABI_NETBSD:
        printf("NetBSD\n");
        break;

    case ELFOSABI_LINUX:
        printf("Linux\n");
        break;

    case ELFOSABI_SOLARIS:
        printf("Sun Solaris\n");
        break;

    case ELFOSABI_AIX:
        printf("IBM AIX\n");
        break;

    case ELFOSABI_IRIX:
        printf("SGI Irix\n");
        break;

    case ELFOSABI_FREEBSD:
        printf("FreeBSD\n");
        break;

    case ELFOSABI_TRU64:
        printf("Compaq TRU64 UNIX\n");
        break;

    case ELFOSABI_MODESTO:
        printf("Novell Modesto\n");
        break;

    case ELFOSABI_OPENBSD:
        printf("OpenBSD\n");
        break;

    case ELFOSABI_ARM_AEABI:
        printf("ARM EABI\n");
        break;

    case ELFOSABI_ARM:
        printf("ARM\n");
        break;

    case ELFOSABI_STANDALONE:
        printf("Standalone (embedded) app\n");
        break;

    default:
        printf("Unknown (0x%x)\n", e64_hdr->e_ident[EI_OSABI]);
        break;
    }

    /* ELF filetype */
    printf("Filetype \t= ");
    switch (e64_hdr->e_type)
    {
    case ET_NONE:
        printf("N/A (0x0)\n");
        break;

    case ET_REL:
        printf("Relocatable\n");
        break;

    case ET_EXEC:
        printf("Executable\n");
        break;

    case ET_DYN:
        printf("Shared Object\n");
        break;
    default:
        printf("Unknown (0x%x)\n", e64_hdr->e_type);
        break;
    }

    /* ELF Machine-id */
    printf("Machine\t\t= ");
    switch (e64_hdr->e_machine)
    {
    case EM_NONE:
        printf("None (0x0)\n");
        break;

    case EM_386:
        printf("INTEL x86 (0x%x)\n", EM_386);
        break;

    case EM_X86_64:
        printf("AMD x86_64 (0x%x)\n", EM_X86_64);
        break;

    case EM_AARCH64:
        printf("AARCH64 (0x%x)\n", EM_AARCH64);
        break;

    default:
        printf(" 0x%x\n", e64_hdr->e_machine);
        break;
    }

    /* Entry point */
    printf("Entry point\t= 0x%08lx\n", e64_hdr->e_entry);

    /* ELF header size in bytes */
    printf("ELF header size\t= 0x%08x\n", e64_hdr->e_ehsize);

    /* Program Header */
    printf("\nProgram Header\t= ");
    printf("0x%08lx\n", e64_hdr->e_phoff);            /* start */
    printf("\t\t  %d entries\n", e64_hdr->e_phnum);   /* num entry */
    printf("\t\t  %d bytes\n", e64_hdr->e_phentsize); /* size/entry */

    /* Section header starts at */
    printf("\nSection Header\t= ");
    printf("0x%08lx\n", e64_hdr->e_shoff);            /* start */
    printf("\t\t  %d entries\n", e64_hdr->e_shnum);   /* num entry */
    printf("\t\t  %d bytes\n", e64_hdr->e_shentsize); /* size/entry */
    printf("\t\t  0x%08x (string table offset)\n", e64_hdr->e_shstrndx);

    /* File flags (Machine specific)*/
    printf("\nFile flags \t= 0x%08x\n", e64_hdr->e_flags);

    /* ELF file flags are machine specific.
     * INTEL implements NO flags.
     * ARM implements a few.
     * Add support below to parse ELF file flags on ARM
     */
    int32_t ef = e64_hdr->e_flags;
    printf("\t\t  ");

    if (ef & EF_ARM_RELEXEC)
        printf(",RELEXEC ");

    if (ef & EF_ARM_HASENTRY)
        printf(",HASENTRY ");

    if (ef & EF_ARM_INTERWORK)
        printf(",INTERWORK ");

    if (ef & EF_ARM_APCS_26)
        printf(",APCS_26 ");

    if (ef & EF_ARM_APCS_FLOAT)
        printf(",APCS_FLOAT ");

    if (ef & EF_ARM_PIC)
        printf(",PIC ");

    if (ef & EF_ARM_ALIGN8)
        printf(",ALIGN8 ");

    if (ef & EF_ARM_NEW_ABI)
        printf(",NEW_ABI ");

    if (ef & EF_ARM_OLD_ABI)
        printf(",OLD_ABI ");

    if (ef & EF_ARM_SOFT_FLOAT)
        printf(",SOFT_FLOAT ");

    if (ef & EF_ARM_VFP_FLOAT)
        printf(",VFP_FLOAT ");

    if (ef & EF_ARM_MAVERICK_FLOAT)
        printf(",MAVERICK_FLOAT ");

    printf("\n");

    /* MSB of flags conatins ARM EABI version */
    printf("ARM EABI\t= Version %d\n", (ef & EF_ARM_EABIMASK) >> 24);

    printf("\n"); /* End of ELF header */
}

// PROGRAM HEADERS
static void print_elf64_progheader_flags(Elf64_Word flag)
{
    char flgstr[3] = {'.', '.', '.'};
    if (flag & PF_R)
    {
        flgstr[0] = 'R';
    }
    if (flag & PF_W)
    {
        flgstr[1] = 'W';
    }
    if (flag & PF_X)
    {
        flgstr[2] = 'X';
    }
    printf("%s ", flgstr);
}
static void print_elf64_progheaders(Elf64_Ehdr *e64_hdr)
{
    // ptr cause im lazy
    Elf64_Phdr *phdr_table = elf_to_load.phdr_table;

    printf("Program Headers:\n");
    printf("========================================");
    printf("========================================\n");
    printf(" Type Offset     VirtAddr   PhysAddr   FileSz"
           "     MemSz      Flg Align\n");
    printf("========================================");
    printf("========================================\n");
    for (int i = 0; i < e64_hdr->e_phnum; i++)
    {
        printf("  %u   ", phdr_table[i].p_type);
        printf("0x%08lx ", phdr_table[i].p_offset);
        printf("0x%08lx ", phdr_table[i].p_vaddr);
        printf("0x%08lx ", phdr_table[i].p_paddr);
        printf("0x%08lx ", phdr_table[i].p_filesz);
        printf("0x%08x ", phdr_table[i].p_memsz);
        print_elf64_progheader_flags(phdr_table[i].p_flags);
        printf("0x%08x\t", phdr_table[i].p_align);
        printf("\n");
    }
}
static int parse_elf64_progheadrs(Elf64_Ehdr *e64hdr, uint8_t *fdata)
{
    // Helper ptr
    Elf64_Phdr *phdr_table = NULL;
    int result = 0;

    // Allocate enough space for all the program headers
    // The ELF64 header tells us all we need
    elf_to_load.phdr_table = malloc(e64hdr->e_phnum * e64hdr->e_phentsize);
    IF_NULL_ERROR_THEN_DONE(elf_to_load.phdr_table);

    // Set helper pointer to the Program Header table offset
    phdr_table = (Elf64_Phdr *)(fdata + e64hdr->e_phoff);

    // Walk the Program Header table copying out the headers
    // We'll want easy access to these later
    for (int i = 0; i < e64hdr->e_phnum; i++)
    {
        memcpy((void *)&elf_to_load.phdr_table[i], &phdr_table[i], e64hdr->e_phentsize);
    }
done:
    return result;
}
// END PROGRAM HEADERS
// SECTION HEADERS
/**
 * @brief Bulk is borrowed from "TheCodeArtist" github. Shanks
 *
 */
static void print_elf64_secheaders(Elf64_Ehdr *e64_hdr, uint8_t *fdata)
{
    // ptr cause im lazy
    Elf64_Shdr *shdr_table = elf_to_load.shdr_table;
    char *shdr_str_table = NULL;

    // This index tells us which Section Header contains
    // the Section Header String Table (also a section).
    // This table contains the names of each of the section
    // headers present in the binary
    // printf("e64_hdr->e_shstrndx = 0x%x\n", e64_hdr->e_shstrndx);
    shdr_str_table = (char *)fdata + shdr_table[e64_hdr->e_shstrndx].sh_offset;

    // printf("Shdr String Table offset: 0x%X\n", shdr_str_table);

    printf("Section Headers:\n");
    printf("========================================");
    printf("========================================\n");
    printf(" idx offset     load-addr  size       algn"
           " flags      type       section\n");
    printf("========================================");
    printf("========================================\n");

    for (int i = 0; i < e64_hdr->e_shnum; i++)
    {
        printf("%3d  ", i);
        printf("0x%08lx ", shdr_table[i].sh_offset);
        printf("0x%08lx ", shdr_table[i].sh_addr);
        printf("0x%08lx ", shdr_table[i].sh_size);
        printf("%4ld ", shdr_table[i].sh_addralign);
        printf("0x%08lx ", shdr_table[i].sh_flags);
        printf("0x%08x ", shdr_table[i].sh_type);
        printf("%s\t", (shdr_str_table + shdr_table[i].sh_name));
        printf("\n");
    }
    printf("========================================");
    printf("========================================\n");
    printf("\n"); /* end of section header table */
}

static int parse_elf64_secheaders(Elf64_Ehdr *e64hdr, uint8_t *fdata)
{
    // Helper ptr
    Elf64_Shdr *shdr_table = NULL;
    int result = 0;

    // Allocate enough space for all the section headers
    // The ELF64 header tells us all we need
    elf_to_load.shdr_table = malloc(e64hdr->e_shnum * e64hdr->e_shentsize);
    IF_NULL_ERROR_THEN_DONE(elf_to_load.shdr_table);

    // Set helper pointer to the Section Header table offset
    shdr_table = (Elf64_Shdr *)(fdata + e64hdr->e_shoff);

    // Walk the Section Header table copying out the headers
    // We'll want easy access to these later
    for (int i = 0; i < e64hdr->e_shnum; i++)
    {
        memcpy((void *)&elf_to_load.shdr_table[i], &shdr_table[i], e64hdr->e_shentsize);
    }
done:
    return result;
}
// END SECTION HEADERS

// Yes this technically can be different on
// other systems but not a concern for us
#define PGSIZE 4096
/**
 * @brief Truncates a size_t value to the left-adjacent (low) 4KiB boundary.
 *
 * @return size_t
 */
size_t align_lo(size_t x)
{
    return (x & ~0xFFF);
}

/**
 * @brief Loads all segments marked with PT_LOAD && has a segment
 * size > 0. Lastly sets all newly mapped regions to the proper
 * permissions
 *
 * @param[in] e64_hdr: ptr to elf header of target bin
 * @param[in] fdata; raw target bin data
 * @param[out] entry_point: ptr to entry point out var
 *
 * @return
 */
static int map_loadable_segments(Elf64_Ehdr *e64_hdr, uint8_t *fdata, uint64_t *entry_point)
{
    int result = 0;
    Elf64_Phdr *phdrs = elf_to_load.phdr_table;
    size_t base_addr = 0x400000;

    for (int i = 0; i < e64_hdr->e_shnum; i++)
    {
        int prot = 0;
        void *res = NULL;

        /**
         * It's possible for a segment to be marked
         * loadable and with a memory size of 0
         * skip those segments
         */
        if (PT_LOAD == phdrs[i].p_type && 0 != phdrs[i].p_memsz)
        {

            /**
             * It is possible to have binaries where the first
             * PT_LOAD segment vaddr is 0, this prevents the kernel
             * from deciding our segments memory mapped location
             */
            size_t addr = base_addr + phdrs[i].p_offset;

            // Need to make sure to adjust for proper page
            // aligned memory addresses when calling mmap
            size_t aligned_addr = align_lo(addr);

            // Get the amount of padding in bytes
            size_t padding = addr - aligned_addr;

            // Adjust our now true length
            size_t segment_len = phdrs[i].p_memsz + padding;

            // TODO
            // make this print cleaner...
            printf("[+] Loading segment @ 0x%08X..0x%08X with perms: ", aligned_addr, segment_len);
            print_elf64_progheader_flags(phdrs[i].p_flags);
            printf("\n");

            // Initially just make it easy and make everything writable
            // We'll need to copy the segment into our mmap'd region
            res = mmap(aligned_addr, segment_len, PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
            if (MAP_FAILED == res)
            {
                perror("[!] mmap() failed!\n");
                printf("Errno: %d", errno);
                result = 1;
                goto done;
            }

            // Again, overkill but lets track our mmap'd regions
            add_mapping(res, segment_len);

            /**
             * Copy segment into memory
             *
             * Copy FROM the elf we are loading using the actual size,
             * not the padded size.
             * NOTE: We must offset into our memory mapped regions based on our
             * padding, this is so any rip relative addressing will still
             * be correct!!...ask me how i know *cries in gdb*
             */
            memcpy(res + padding, fdata + phdrs[i].p_offset, phdrs[i].p_memsz);

            // Build out memory permission flag
            if (phdrs[i].p_flags & PF_R)
            {
                prot |= PROT_READ;
            }
            if (phdrs[i].p_flags & PF_W)
            {
                prot |= PROT_WRITE;
            }
            if (phdrs[i].p_flags & PF_X)
            {
                prot |= PROT_EXEC;
                // Found our executable segment
                // set out variable for entry point
                *entry_point = aligned_addr;
            }

            // Now set proper permissionss
            if (-1 == mprotect(res, segment_len, prot))
            {
                perror("[!] mprotect failed!\n");
                result = 1;
                goto done;
            }
        }
    }
done:
    return result;
}

uint64_t instructor_load(uint8_t *fdata, size_t size)
{
    Elf64_Ehdr *e64_hdr = (Elf64_Ehdr *)fdata;
    uint64_t addr = e64_hdr->e_entry;

    printf("** Begin Loading Elf **\n");

    if (0 == is_elf64((Elf64_Ehdr *)fdata))
    {
        ADDR_ZERO_THEN_DONE;
    }

    // Program Headers
    if (parse_elf64_progheadrs(e64_hdr, fdata))
    {
        ADDR_ZERO_THEN_DONE;
    }

    // No return
    print_elf64_progheaders(e64_hdr);

    // Section Headers
    if (parse_elf64_secheaders(e64_hdr, fdata))
    {
        ADDR_ZERO_THEN_DONE;
    }

    // No return
    print_elf64_secheaders(e64_hdr, fdata);

    if (map_loadable_segments(e64_hdr, fdata, &addr))
    {
        ADDR_ZERO_THEN_DONE;
    }

done:
    printf("** End Loading Elf **\n");
    return addr;
}

/**
 * @brief Jump to our in-memory loaded elf
 * @param[in] entry 64-bit address to entry point
 * @return Technically can return an int, but we shouldn't
 * be returning at all once we jump
 */
int instructor_jump(uint64_t entry)
{
    if (0 == entry)
    {
        printf("[-] ERROR! Provided a bad entry point address!\n");
    }
    else
    {
        /**
         * This is a cheezy way and doesn't account for cmdline args, etc
         * but is plenty good enough for teaching purposes
         */
        foo = (void (*)())entry;
        printf("[+] Jumping to 0x%p\n", foo);
        foo();
        printf("we're back?????\n");
    }
    return 0;
}

/**
 * @brief Clean up time!
 * @warning This should only ever get called if a failure occurs
 * and its just good practice to be cleaning up. Since we'll jump to
 * our loaded program and that will exit, we'll never arrive here post
 * jump
 * @return N/A
 */
void instructor_clean()
{
    FREE_IF_VALID(elf_to_load.phdr_table);
    FREE_IF_VALID(elf_to_load.shdr_table);
    for (int i = 0; i < MAXADDRS; i++)
    {
        if (NULL != mapped_addrs[i].addr)
        {
            // At this point if it fails...oh well
            munmap(mapped_addrs[i].addr, mapped_addrs[i].size);
        }
    }
}