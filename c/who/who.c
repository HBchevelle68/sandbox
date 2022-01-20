#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <utmp.h>

#define NMEMB 100
#define UTMPSZ (sizeof(struct utmp))

int main(int argc, char **argv) {
    
    struct utmp *utmp_log = NULL;  
    FILE *fp = NULL;
    int n_entries = 0;
    int n_alloc = 0;

    // Open the log file
    fp = fopen(UTMP_FILE, "r");
    if(NULL == fp){
        printf("Error opening %s. errno = %d...\n", UTMP_FILE, errno);
        goto done;
    }

    // Alloc the array
    utmp_log = calloc(NMEMB, UTMPSZ);
    if(NULL == utmp_log){
        printf("Error allocating initial utmp_log. errno = %d...\n", errno);
        goto done;
    }
    n_alloc = NMEMB;

    // Loop reading through log
    for (;;)
    {
        // Check if we have room too read a single entry
        if(n_entries == n_alloc){

            // Nope...array full
            utmp_log = realloc(utmp_log, ((n_alloc+NMEMB)*UTMPSZ));
            if(NULL == utmp_log){
                // Failure
                printf("reallocation to %lub failed with errno %d\n", 
                    ((n_alloc+NMEMB)*UTMPSZ), errno);
                goto done;
            }
            // Success
            // Update numer of members allocated
            n_alloc += NMEMB;
        }
        // Actually read
        if(1 != fread(&utmp_log[n_entries], UTMPSZ, 1, fp)) {
            break;
        }
        
    }

    if(0 != ferror(fp)){
        // Error
        printf("Error occured during fread with errno %d\n", errno);
        goto done;
    }   
    // No error, EOF



done:
    if(NULL != fp){
        fclose(fp);
    }

    if(NULL != utmp_log){
        free(utmp_log);
    }

    return 0;
}