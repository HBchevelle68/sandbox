#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>

#define ONEKB 1024
//static size_t PAGE_SIZE = sysconf(_SC_PAGESIZE);

/**
 * @brief Searches memory maps for a library in a targeted 
 * processes memory or search for a library within ourself 
 * 
 * @param library - string of library to search for
 * @param pid - pid of target process, if -1, then 
 * 				search within self
 * @return unsigned long long of starting address of library on 
 * 		   success or 0 on failure
 */
unsigned long long findLibrary(const char *library, pid_t pid) {
    char mapFilename[ONEKB];
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
        //printf("%s\n", buffer);
		if (strstr(buffer, library)) {
            
			addr = strtoull(buffer, NULL, 16);
			break;
		}
	}

	fclose(fd);

	return addr;
}
/**
 * @brief Searches and returns the first instance of memory 
 * with execute permisisons within the targeted process
 * 
 * @param pid - pid of target process
 * @return void* 
 */
void *findExeFreeSpaceAddr(pid_t pid) {
    FILE *fp = NULL;
    char filename[50] = {};
    char line[ONEKB] = {};
    char str[20] = {};
    char perms[5] = {};
    void *start_addr = NULL; 
	void *end_addr = NULL;

	sprintf(filename, "/proc/%d/maps", pid);
    	if ((fp = fopen(filename, "r")) == NULL) {
		printf("[!] Error, could not open maps file for process %d\n", pid);
		exit(1);
	}

	while(fgets(line, 850, fp) != NULL) {
        //printf("%s", line);
		sscanf(line, "%p-%p %s %*s %s %*d", &start_addr, &end_addr, perms, str);

		if(strstr(perms, "x") != NULL) {
		    break;
		}
    }
	fclose(fp);
	printf("[+] Found executable memory at 0x%p\n", start_addr);
	return start_addr;
}



/**
 * 
 * 
 */
int main(int argc, char* argv[]) {
    unsigned long long target_libdl_exemem, self_libdl_exemem;
    void *dlopen_sym_addr = NULL;
    void *self_libdl_addr = NULL;

	if(argc != 2){
		printf("[-] Improper number of args...\n");
		exit(EXIT_FAILURE);
	}

	self_libdl_addr = dlopen("libdl.so", RTLD_LAZY);
	if (self_libdl_addr == NULL) {
		printf("[!] Error opening libdl.so\n");
		exit(EXIT_FAILURE);
	}
	printf("[*] libdl.so loaded at address %p\n", self_libdl_addr);

	dlopen_sym_addr = dlsym(self_libdl_addr, "dlopen");
	if (dlopen_sym_addr == NULL) {
		printf("[!] Error locating dlopen() function\n");
		exit(EXIT_FAILURE);
	}
	printf("[*] dlopen() found at address %p\n", dlopen_sym_addr);

	// Find the base address of libdl in our target process
	target_libdl_exemem = findLibrary("libdl", atoi(argv[1]));
	if(!target_libdl_exemem){
		printf("[-] Could not locate libdl in PID: %i", atoi(argv[1]));
		exit(EXIT_FAILURE);
	}
	printf("[*] libdl located in PID %d at address %p\n", atoi(argv[1]), (void*)target_libdl_exemem);

	// Find base of executable lib memory
	self_libdl_exemem = findLibrary("libdl", -1);

	/*
	 * Due to ASLR, we need to calculate the 
	 * address offset in the target process
	 */  
	dlopen_sym_addr = target_libdl_exemem + (dlopen_sym_addr - self_libdl_exemem);
	printf("[*] dlopen() offset in libdl found to be 0x%llx bytes\n", (unsigned long long)(self_libdl_addr - self_libdl_exemem));
	printf("[*] dlopen() in target process at address 0x%llx\n", (unsigned long long)dlopen_sym_addr);

    findExeFreeSpaceAddr(atoi(argv[1]));

    return 0;
}