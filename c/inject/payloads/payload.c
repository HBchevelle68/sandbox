#include <stdio.h>
#include <dlfcn.h>

#include "payload.h"

/*
 * hello()
 *
 * Hello world function exported by the sample library.
 *
 */

void hello(void)
{   
	puts("I just got loaded\n");
}

/*
 * loadMsg()
 *
 * This function is automatically called when the sample library is injected
 * into a process. It calls hello() to output a message indicating that the
 * library has been loaded.
 *
 */

__attribute__((constructor))
void loadMsg(void)
{
    //FILE *fp;
    //fp = fopen ("/tmp/file_name", "mode");
    //fclose(fp);
	hello();
}