#define _XOPEN_SOURCE 1			 /* Required under GLIBC for nftw() */
#define _XOPEN_SOURCE_EXTENDED 1 /* Same */

#include <stdio.h>
#include <ftw.h>
#include <unistd.h>
#include <limits.h> 
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <glob.h>
#include <pwd.h>
#include <grp.h>
#include <fnmatch.h>

#define RECUR_CONTINUE    0
#define MAX_DEPTH_REACHED 1

const long int AUGUST012021 = 1627790400;
const long int AUGUST312021 = 1630468799;

typedef struct{
    int max_depth;
    char* glob_pattern;
     


} filter_t;

typedef struct {
    char path[FILENAME_MAX];
    unsigned int type;
    unsigned int perms;

} entry_t;

static filter_t filter;

static void build_perm_str(mode_t file_mode, char* permission_str){
    
    permission_str[0] = (file_mode & S_IRUSR) ? 'r' : '-';
    permission_str[1] = (file_mode & S_IWUSR) ? 'w' : '-';
    permission_str[2] = (file_mode & S_IXUSR) ? 'x' : '-';
    permission_str[3] = (file_mode & S_IRGRP) ? 'r' : '-';
    permission_str[4] = (file_mode & S_IWGRP) ? 'w' : '-';
    permission_str[5] = (file_mode & S_IXGRP) ? 'x' : '-';
    permission_str[6] = (file_mode & S_IROTH) ? 'r' : '-';
    permission_str[7] = (file_mode & S_IWOTH) ? 'w' : '-';
    permission_str[8] = (file_mode & S_IXOTH) ? 'x' : '-';
    permission_str[9] = '\0';
}

static void build_ftype_str(mode_t file_mode, char* type) {

    if(S_ISDIR(file_mode)){
        *type = 'd';
    }
    else if(S_ISLNK(file_mode)){
        *type = 'l';
    }
    else if(S_ISBLK(file_mode)){
        *type = 'b';
    }
    else if(S_ISCHR(file_mode)){
        *type = 'c';
    }
    else{
        *type = '-';
    }
    
}

int nftw_callback(const char *file, const struct stat *file_stat, int flag, struct FTW *ftw_meta){

    char file_type;
    char permissions[10];
    struct passwd* usrinfo;
    struct group* grpinfo;
    
    if(0 != filter.max_depth && ftw_meta->level > filter.max_depth){
        //printf("continuing\n");
        return RECUR_CONTINUE;
    }
    

    switch (flag) {
        case FTW_F:
            // Normal File

            // Match on glob filter
            if(FNM_NOMATCH == fnmatch(filter.glob_pattern, file, 0)){
                // No match
                break;
            }
            printf("Is %lu < %lu ?\n", file_stat->st_ctime, AUGUST012021);
            printf("Is %lu > %lu ?\n", file_stat->st_ctime, AUGUST312021);
            // Match on timestamp
            if(file_stat->st_ctime < AUGUST012021 && file_stat->st_ctime > AUGUST312021){
                // Outside time bound
                printf("OUTSIDE TIME BOUND!\n");
                break;
            }


            build_perm_str(file_stat->st_mode, permissions);
            build_ftype_str(file_stat->st_mode, &file_type);
            usrinfo = getpwuid(file_stat->st_uid);
            grpinfo = getgrgid(file_stat->st_gid);
            struct tm * timeinfo = localtime(&file_stat->st_mtime);
            char* stime = asctime(timeinfo);
            
            stime[strlen(stime)-1] = '\0';

            printf("%c%s %3lu %-16s %-16s %15lu %s %-s\n", 
            file_type, permissions, file_stat->st_nlink, usrinfo->pw_name, grpinfo->gr_name, file_stat->st_size, stime, (file));

            break;
        case FTW_D:
            // Directory
            break;
        case FTW_SL:
            // Symlink
            break;
        case FTW_DP:

            break;

        default:
            break;
    }

    return EXIT_SUCCESS;
}


int find(char* path, char* pattern) {

    filter.max_depth = 3;
    filter.glob_pattern = pattern;

    return nftw(path, nftw_callback, 100, FTW_MOUNT);
}


int main(int argc, char** argv) {
    
    char *resolved_path = NULL;
    char cwd[PATH_MAX] = {};
    char *pattern = "*";

    if( argc < 2){
        getcwd(cwd, PATH_MAX);
        find(cwd, pattern);
    }
    else {
        resolved_path = realpath(argv[1], NULL);
        if(resolved_path) {
            if(argc != 3) {
                printf("No pattern given... grabing everything\n");
            }
            else {
                pattern = argv[2];
            }
            find(resolved_path, pattern);
            free(resolved_path);
        }
        else {
            printf("Invalid path: %s", argv[1]);
        }
    }

    return 0;
}