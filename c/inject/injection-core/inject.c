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

int inject_shellcode(pid_t pid, void* shellcode, size_t shellcode_len, uint8_t options){

	unsigned char *oldcode = NULL;
	struct user_regs_struct oldregs = {0};
	struct user_regs_struct regs = {0};
	long addr = 0;
	int target_status = 0;
	int attach_status = DETACHED;
	int result = EXIT_SUCCESS;

	// Find a good address to copy code to
	addr = findExecAddr(pid);
	if(!addr){
		result = EXIT_FAILURE;
		goto done;
	}

	// Attach to the target process
	result = ptraceAttach(pid, &target_status);
	IF_FAILURE_THEN_DONE(result);

	attach_status = ATTACHED;

	// Store the current register values for later
	result = ptraceGetRegs(pid, &oldregs);
	IF_FAILURE_THEN_DONE(result);
	memcpy(&regs, &oldregs, sizeof(struct user_regs_struct));

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
	attach_status = DETACHED;	

done:
	if(NULL != oldcode){
		free(oldcode);
	}
	if(ATTACHED == attach_status){
		ptraceDetach(pid);
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