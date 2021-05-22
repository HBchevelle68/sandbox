#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>

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

int dirlist(char* dir_to_list){
    
    DIR *dir_ptr;
    struct dirent *dir_entry_ptr;
    struct stat file_stat = {};
    char dir_path[FILENAME_MAX] = {};
	int dir_path_len = strlen(dir_to_list);
    char slash = '/';
    
    char file_type;
    char permissions[10];
    struct passwd* usrinfo;
    struct group* grpinfo;
	
    if (dir_path_len >= FILENAME_MAX - 1) {
        printf("File name too long!\n");
        return -1;
    }
		
    // Need to keep the path
	strcpy(dir_path, dir_to_list);
    if(dir_path[dir_path_len-1] != slash) {
	    dir_path[dir_path_len++] = '/';
    }

    // Open directory
    dir_ptr = opendir(dir_to_list);
    if(!dir_ptr) {
        printf("failure to open %s\n", dir_to_list);
        return 1;
    }

    while ((dir_entry_ptr = readdir(dir_ptr))) {

        strncpy(dir_path + dir_path_len, dir_entry_ptr->d_name, (FILENAME_MAX-dir_path_len));
        if(lstat(dir_path, &file_stat) == -1) {
			printf("Can't stat %s", dir_path);
			continue;
		}


        build_perm_str(file_stat.st_mode, permissions);
        build_ftype_str(file_stat.st_mode, &file_type);
        usrinfo = getpwuid(file_stat.st_uid);
        grpinfo = getgrgid(file_stat.st_gid);
        struct tm * timeinfo = localtime(&file_stat.st_mtim.tv_sec);
        char* stime = asctime(timeinfo);
        
        stime[strlen(stime)-1] = '\0';

        printf("%c%s %3lu %-16s %-16s %15lu %s %-s\n", 
        file_type, permissions, file_stat.st_nlink, usrinfo->pw_name, grpinfo->gr_name, file_stat.st_size, stime, dir_entry_ptr->d_name);
    }
    
    return 0;
}

int main(int argc, char** argv){
    
    char* resolved_path = NULL;

    if( argc != 2){
        dirlist((char*)"/");
    }
    else{
        resolved_path = realpath(argv[1], NULL);
        if(resolved_path){
            dirlist(resolved_path);
            free(resolved_path);
        }
        else{
            printf("Invalid path: %s", argv[1]);
        }
    }

    return 0;
}