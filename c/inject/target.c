#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dlfcn.h>

int main(int argc, char* argv[]){
    
    void* dlopen_sym_addr;
    void* libdlAddr;

    while(1) {
        printf("*********\n");

        pid_t pid = getpid();
        printf("[*] pid: %u\n", pid);

        libdlAddr = dlopen("libdl.so", RTLD_LAZY);
        if (libdlAddr == NULL) {
            printf("[!] Error opening libdl.so\n");
        }
        printf("[*] libdl.so loaded at address %p\n", libdlAddr);

        dlopen_sym_addr = dlsym(libdlAddr, "dlopen");
        if (dlopen_sym_addr == NULL) {
            printf("[!] Error locating dlopen() function\n");
        }
        printf("[*] dlopen() found at address %p\n", dlopen_sym_addr);

        printf("*********\n");
        sleep(5);
    }

    return 0;
}