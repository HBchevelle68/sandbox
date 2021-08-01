#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>


void myfunc(void){

    for(int i = 0; i < 10; i++){
        printf(".");
        fflush(NULL);
        sleep(1);
    }
    printf("\n");
}


unsigned long putils_findLibrary(const char *library, pid_t pid) {
    char mapFilename[1024];
    char buffer[9076];
    FILE *fd;
    unsigned long long addr = 0;

	if (pid == -1) {
		snprintf(mapFilename, sizeof(mapFilename), "/proc/self/maps");
	} 
    else {
		snprintf(mapFilename, sizeof(mapFilename), "/proc/%d/maps", pid);
	}

	fd = fopen(mapFilename, "r");

	while(fgets(buffer, sizeof(buffer), fd)) {
		if (strstr(buffer, library)) {
			addr = strtoull(buffer, NULL, 16);
			break;
		}
	}
	fclose(fd);

	return addr;
}

void* getFunctionAddress(char* funcName)
{
	void* self = dlopen("libc.so.6", RTLD_LAZY);
	void* funcAddr = dlsym(self, funcName);
	return funcAddr;
}

int main(int argc, char* argv[]){
    
    void* libc_dlopen_sym_addr;
    unsigned long  libdlAddr;
    pid_t pid = getpid();

    //libdlAddr = dlopen("libc.so.6", RTLD_LAZY);
    //if (libdlAddr == NULL) {
    //    printf("[!] Error opening libc.so.6\n");
    //}
    libdlAddr = putils_findLibrary("libc", -1);

    libc_dlopen_sym_addr = getFunctionAddress("__libc_dlopen_mode");
    if (libc_dlopen_sym_addr == NULL) {
        printf("[!] Error locating __libc_dlopen_mode() function\n");
    }
    

    while(1) {
        
        printf("[*] pid: %u\n", pid);
        printf("[*] libc.so loaded at address %lx\n", libdlAddr);  
        printf("[*] __libc_dlopen_mode() found at address %p\n", libc_dlopen_sym_addr);
        printf("[*] main() at address %p\n", main);
        printf("[*] myfunc() at address %p\n", myfunc);
        printf("[*] Stack variable at %p\n", &pid);
        myfunc();
    }

    return 0;
}