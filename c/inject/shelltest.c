#include <stdio.h>
#include <sys/mman.h>
#include <string.h>

/* equiv to MAP_ANONYMOUS but 
 * on Ubunut 20 that requires the use
 * of -std=gnu11. Which is less portable
 * This lame work around allows use of 
 * -std=c11 which is more portable
 */
#define ANONYMOUS_MAP 0x20



unsigned char shellcode[] = "\x48\x31\xc0\x50\x48\x31\xf6\x48\x31\xd2\x48\xbb\x2f\x62\x69\x6e\x2f\x73\x68\x00\x53\x48\x89\xe7\xb8\x3b\x00\x00\x00\x0f\x05";



int main(int argc, char** argv) {
    char *buf;
    int prot = PROT_READ | PROT_WRITE | PROT_EXEC;
    int flags = MAP_PRIVATE | ANONYMOUS_MAP;

    buf = mmap(0, sizeof(shellcode), prot, flags, -1, 0);
    memcpy(buf, shellcode, sizeof(shellcode));     
    printf("Shellcode size: %lu\n\n", sizeof(shellcode));
    ((void (*)(void))buf)();
}