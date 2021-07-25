#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "process_utils.h"


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