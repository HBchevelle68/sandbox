#include <stdio.h>
#include <sys/ptrace.h> // See ptrace(2) man pages for details
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#include "ptrace_utils.h"


static 
void _ptrace_error_parse(){
    switch(errno) {
        case EBUSY:
            // i386 only
            printf("[-] (EBUSY) Error with allocating or freeing a debug register\n");
            break;

        case EFAULT:
            printf("[-] (EFAULT) attempt to read from or write to an invalid area "
                   "in the tracer's or the tracee's memory, probably "
                   "because the area wasn't mapped or accessible\n");
            break;

        case EINVAL:
            printf("[-] (EINVAL) Attempt was made to set an invalid option\n");
            break;

        case EIO:
            printf("[-] (EIO) request is invalid, or an attempt was made to read "
                   "from or write to an invalid area in the tracer's or the "
                   "tracee's memory, or there was a word-alignment violation, or an "
                   "invalid signal was specified during a restart request\n");
            break;

        case EPERM:
            printf("[-] (EPERM) Specified process cannot be traced");
            break;

        case ESRCH:
            printf("[-] (ESRCH) The specified process does not exist, or "
                   "is not currently being traced by the caller, or is "
                   "not stopped (for requests that require a stopped tracee)\n");
            break;

        default:
            printf("[!] Unknown error value from ptrace... errno: %d",errno);
            break;
    }
}

/**
 * @brief Attach to specified process nad verify process recv'd SIGSTOP
 * 
 * @param pid: (int) pid of the target process
 * @param out_status: (int*) out ptr of status of target pid from waitpid 
 * @return 0 on success 1 on failure 
 */
int ptraceAttach(int pid, int* out_status){
    int result = EXIT_SUCCESS;

    // Attach to the target process
	if(-1 == ptrace(PTRACE_ATTACH, pid, NULL, NULL)) {
        printf("ptraceAttach failed");
        _ptrace_error_parse();
        result = EXIT_FAILURE;
    }
    else if(-1 == waitpid(pid, out_status, WUNTRACED)) {
		printf("[-] Failure to catch SIGSTOP on target process\n");
        result = EXIT_FAILURE;
	}
    else {
	    printf("[+] Attached to PID: %d\n", pid);
    }
    return result;
}

/**
 * @brief Detach from program entirely
 * 
 * @param pid: (int) pid of the target process
 * @return 0 on success 1 on failure 
 */
int ptraceDetach(pid_t pid){
    int result = EXIT_SUCCESS;
    
	if(-1 == ptrace(PTRACE_DETACH, pid, NULL, NULL)){
        printf("[-] ptraceDetach failed\n");
        _ptrace_error_parse();
        result = EXIT_FAILURE;
    }
    return result;
}

/**
 * @brief Read copy of memory at specified address for 
 * specified length of bytes
 *
 * @param pid: (int) pid of the target process
 * @param addr: (unsigned long) The address to start reading from
 * @param void *out_ptr: aA pointer to a buffer to read data into
 * @param len: int tThe amount of data to write to the targe
 *
 * @return 0 on success 1 on failure
 */
int ptraceRead(int pid, unsigned long addr, void *out_ptr, uint32_t len) {
	int bytesRead = 0;
	int i = 0;
	long word = 0;
	long *ptr = (long *)out_ptr;
    int result = EXIT_SUCCESS;

	while (bytesRead < len) {
		word = ptrace(PTRACE_PEEKTEXT, pid, (addr + bytesRead), NULL);
		if(-1 == word) {
			printf("[-] ptraceRead failed\n");
            _ptrace_error_parse();
			result = EXIT_FAILURE;
            break;
		}
        else {
            bytesRead += sizeof(word);
            ptr[i++] = word;
        }
	}
    return result;
}

/**
 * @brief Write (inject) bytes at specified address from buffer
 * of passed in length. 
 *
 * @param pid: (int) pid of the target process
 * @param addr: (unsigned long) The address to start reading from
 * @param buff_ptr: (void*) A pointer to a buffer containing the data 
 *              to be written to the target's address space
 * @param len: int tThe amount of data to write to the target
 *
 * @return 0 on success 1 on failure
 */
int ptraceWrite(int pid, unsigned long addr, void *buff_ptr, uint32_t len) {
	int byteCount = 0;
	long word = 0;
    int result = EXIT_SUCCESS;

	while (byteCount < len) {
		memcpy(&word, (buff_ptr + byteCount), sizeof(word));
		word = ptrace(PTRACE_POKETEXT, pid, (addr + byteCount), word);
		if(-1 == word) {
			printf("[-] ptraceWrite failed\n");
            _ptrace_error_parse();
            result = EXIT_FAILURE;
			break;
		}
        else {
		    byteCount += sizeof(word);
        }
	}
    return result;
}

/**
 * @brief Get a copy of processes current registers
 * 
 * @param pid: (int) pid of the target process
 * @param regs: (struct user_regs_struct) out ptr to regs struct
 * used to store copy of regs
 * @return 0 on success 1 on failure 
 */
int ptraceGetRegs(pid_t pid, registers_t* regs){
    int result = EXIT_SUCCESS;

    // Store the current register values for later
	if(-1 == ptrace(PTRACE_GETREGS, pid, NULL, regs)){
        printf("[-] ptraceGetRegs failed\n");
        _ptrace_error_parse();
        result = EXIT_FAILURE;
    }
    return result;
}

/**
 * @brief Set target processes registers with given regs
 * 
 * @param pid: (int) pid of the target process
 * @param regs: (struct user_regs_struct) ptr to regs struct with
 * regs values you wish to write to target process
 * @return 0 on success 1 on failure 
 */
int ptraceSetRegs(pid_t pid, registers_t* regs){
    int result = EXIT_SUCCESS;
    
    // Set registers to regs in supplied struct
	if(-1 == ptrace(PTRACE_SETREGS, pid, NULL, regs)){
        printf("[-] ptraceSetRegs failed\n");
        _ptrace_error_parse();
        result = EXIT_FAILURE;
    }
    return result;
}

/**
 * @brief Continue program execution while still attached
 * 
 * @param pid: (int) pid of the target process
 * @param verifySIGTRAP: (int) if set, will verify SIGTRAP,
 * else will skip check
 * @param status: (int*) ptr to returned status variable from waitpid
 * @return 0 on success 1 on failure 
 */
int ptraceCont(pid_t pid, int verifySIGTRAP, int* status){
    int result = EXIT_SUCCESS;
    
    // Continue program execution while still attached
	if(-1 == ptrace(PTRACE_CONT, pid, NULL, NULL)){
        printf("[-] ptraceCont failed\n");
        _ptrace_error_parse();
        result = EXIT_FAILURE;
        goto done;
    }

    if(verifySIGTRAP) {
        // check status
    }

    
done:
    return result;
}

