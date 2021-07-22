#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dlfcn.h>


void myfunc(void){

    for(int i = 0; i < 10; i++){
        printf(".");
        fflush(NULL);
        sleep(1);
    }
    printf("\n");
}

int main(int argc, char* argv[]){
    
    void* dlopen_sym_addr;
    void* libdlAddr;

    libdlAddr = dlopen("libdl.so", RTLD_LAZY);
    if (libdlAddr == NULL) {
        printf("[!] Error opening libdl.so\n");
    }

    dlopen_sym_addr = dlsym(libdlAddr, "dlopen");
    if (dlopen_sym_addr == NULL) {
        printf("[!] Error locating dlopen() function\n");
    }
    pid_t pid = getpid();
    ;

    while(1) {
        
        printf("[*] pid: %u\n", pid);
        printf("[*] libdl.so loaded at address %p\n", libdlAddr);  
        printf("[*] dlopen() found at address %p\n", dlopen_sym_addr);
        printf("[*] main() at address %p\n", main);
        printf("[*] myfunc() at address %p\n", myfunc);
        printf("[*] Stack variable at %p\n", &pid);
        myfunc();
    }

    return 0;
}