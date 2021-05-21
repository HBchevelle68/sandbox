#include <stdio.h>
#include <ctype.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>


#define ONEKB    (1024)
#define PAGESIZE (ONEKB*4)

typedef struct proc_stats{
    int pid;
    char cmd[512];
    char state;
    int ppid;
    int pgrp;
    int sessionid; // unused
    int tty_nr;    // unused
    char* stime;
    struct stat fstat;
    struct passwd* uinfo;

} proc_stats_t;

static void get_process_stats(char* proc_pid, proc_stats_t* pstat){
    
    memset(pstat, 0, sizeof(proc_stats_t));
    
    /* Create multiple file paths:
     * 
     * 1. /proc/<pid> -> used to get user info, time stats on process
     * 2. /proc/<pid>/stat -> core process info exists here
     */ 
    char pid_path[ONEKB];
    char pid_path_stat[ONEKB];
    memset(pid_path, 0, sizeof(pid_path));
    sprintf(pid_path, "/proc/%d", atoi(proc_pid));
    sprintf(pid_path_stat, "/proc/%d/stat", atoi(proc_pid));


    /*
     * Open /proc/<pid>/stat and extract desired 
     * info. Some info not used currently
     */
    // Open up file
    FILE *pid_path_fptr = fopen(pid_path_stat, "r");

    // Read into struct
    fscanf(pid_path_fptr, "%d %s %c %d %d %d %d", 
    &pstat->pid, pstat->cmd, &pstat->state, &pstat->ppid, 
    &pstat->pgrp, &pstat->sessionid, &pstat->tty_nr);
    // Close
    fclose(pid_path_fptr);

    /*
     * Get /proc/<pid> creation time. in other
     * words, get the creation time of the process
     * will resue the results from stat after
     */ 
    stat(pid_path, &pstat->fstat);
    struct tm * timeinfo = localtime(&pstat->fstat.st_ctime);
    pstat->stime = asctime(timeinfo);
    pstat->stime[strlen(pstat->stime)-1] = '\0';

    /*
     * Using results from prior stat call
     * get user/group info
     */
    pstat->uinfo = getpwuid(pstat->fstat.st_uid);    
    
}

int main(int argc, char** argv){

    DIR *dir_ptr;
    struct dirent *dir_entry_ptr;     
    dir_ptr = opendir ("/proc/");
    proc_stats_t pstats = {};

    if (dir_ptr != NULL) {

        // readdir to open next dir in 
        // link list
        printf("%-20s %-10s %-10s %-10s %-28s %s\n","USER", "PID", "PPID", "STATE", "START", "COMMAND");
        while ((dir_entry_ptr = readdir(dir_ptr)) ){
            
            if(isdigit(dir_entry_ptr->d_name[0])){

                // Get stats
                get_process_stats(dir_entry_ptr->d_name, &pstats);

                // read cmdline file
                char cmdline_file_contents[PAGESIZE];
                char cmdline_file[256];
                memset(cmdline_file, 0, PAGESIZE);
                sprintf(cmdline_file, "/proc/%d/cmdline", atoi(&dir_entry_ptr->d_name[0]));

                int fd = open(cmdline_file, O_RDONLY);
                read(fd, cmdline_file_contents, PAGESIZE);
                close(fd);

                // cmdline isn't always populated 
                char* cmdptr = cmdline_file_contents[0] ? cmdline_file_contents : pstats.cmd;
                
                printf("%-20s %-10d %-10d %-10c %-28s %s\n", pstats.uinfo->pw_name, pstats.pid, pstats.ppid, pstats.state, pstats.stime, cmdptr);
            }
        }

        closedir(dir_ptr);
    }
    else {
        perror ("Couldn't open the directory");
    }

    return 0;
}
