#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <utmp.h>
#include <time.h>

// Some helpers
#define NMEMB 100
#define UTMPSZ (sizeof(struct utmp))
#define MAXTIMESZ 32

// From gnulib filename.h
// pre-fixing with MY_ to prevent possible conflict
#define MY_ISSLASH(C) ((C) == '/')
#define MY_HAS_DEVICE(Filename) ((void) (Filename), 0)
#define MY_FILE_SYSTEM_PREFIX_LEN(Filename) ((void) (Filename), 0)
#define MY_IS_ABSOLUTE_FILE_NAME(Filename) ISSLASH ((Filename)[0])
#define MY_IS_RELATIVE_FILE_NAME(Filename) (! ISSLASH ((Filename)[0]))
#define MY_IS_FILE_NAME_WITH_DIR(Filename) (strchr ((Filename), '/') != NULL)

// From gnulib who.c
// pre-fixing with MY_ to prevent possible conflict
#define MY_DEV_DIR_WITH_TRAILING_SLASH "/dev/"
#define MY_DEV_DIR_LEN (sizeof (MY_DEV_DIR_WITH_TRAILING_SLASH) - 1)

/**
 * @brief Resolves the ut_line member of a struct utmp.
 * decent portions of this have been take from GNU who.c
 * but simplified since I don't need to support windows
 * 
 * @param[in] utmp_ut_line The struct utmp* you wish to resolve the ut_line member of
 * @param[out] resolved_line result of resolution of utmp_ut_line param
 */
void resolve_line_idle_str(struct utmp* entry, char* resolved_line, char* idle){
    
    char* rlptr = resolved_line;
    
    /* Copy ut_line into LINE, prepending '/dev/' if ut_line is not
     * already an absolute file name.  Some systems may put the full,
     * absolute file name in ut_line.
     */
    if(!MY_IS_ABSOLUTE_FILE_NAME(entry->ut_line)){
        rlptr = stpcpy(resolved_line, entry->ut_line);
    }
    /**
     * If the above conditional is take, this line behaves sematically more 
     * like a strncat. 
     * If the above conditional is not taken, then this will copy
     * the ut_line into the beginning of the array rlptr is pointing to 
     */
    strncpy(rlptr, entry->ut_line, sizeof(entry->ut_line))


    return;
}

/**
 * @brief Convert struct ut_tv.sec to a human-readable timestring
 * 
 * @param[in] tv_sec struct ut_tv.sec from entry cast as a time_t for conversion
 * @param[out] out_time pointer to out buffer for time string, min 32-bytes
 * 
 * @return outtime will contain the converted time string on success
 * or no change at all on error
 */
size_t get_time_str(time_t tv_sec, char *out_time){

    struct tm * timeinfo = localtime(&tv_sec);

    return strftime(out_time, MAXTIMESZ, "%F %T", timeinfo);
}

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
            /*
             * If fread doesn't return 1, either an error occured 
             * or we reached EOF. regardless, time to break the 
             * loop
             */
            break;
        }
        n_entries++;
    }

    // Did we break due to an error?
    if(0 != ferror(fp)){
        // Error
        printf("Error occured during fread with errno %d\n", errno);
        goto done;
    }
    // No error, EOF

    printf("%10s %10s %-20s %-16s %8s %-16s %-16s\n",
        "NAME", "LINE", "TIME", "IDLE", "PID", "COMMENT", "EXIT");
    // Roll through and print the data
    for(int i = 0; i < n_entries; i++){
        
        // Grab Time string
        char time_str[MAXTIMESZ]; 
        get_time_str((time_t)utmp_log[i].ut_tv.tv_sec, time_str);

        //Resolve line
        char line_str[sizeof(UT_LINESIZE) + MY_DEV_DIR_LEN + 1];
        resolve_line_str(utmp_log[i].line_str, line_str);


        printf("%10s %10s %-20s %-16s\n", 
            utmp_log[i].ut_user,
            utmp_log[i].ut_line,
            time_str,
            utmp_log[i].);
        
        memset(&time_str, 0, MAXTIMESZ);
    }



done:
    if(NULL != fp){
        fclose(fp);
    }

    if(NULL != utmp_log){
        free(utmp_log);
    }

    return 0;
}