#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

int curr_depth = 1;
int max_depth = 0;

const char SLASH = '/';
const char SINGLE_DOT[] = ".";
const char DOUBLE_DOT[] = "..";

#define INC_DEPTH(d) ++(d)
#define RESET_DEPTH(d) d=1

int walk_dir_recursive(char* dir_to_walk, int flags){
    
    DIR *dir_ptr;
    struct dirent *dir_entry_ptr;
    struct stat file_stat = {};
    char dir_path[FILENAME_MAX] = {};
	int dir_path_len = strlen(dir_to_walk);
	
    if (dir_path_len >= FILENAME_MAX - 1) {
        printf("File name too long!\n");
        return -1;
    }
    
    // Need to keep the path
	strcpy(dir_path, dir_to_walk);
    if(dir_path[dir_path_len-1] != SLASH) {
	    dir_path[dir_path_len++] = SLASH;
    }

    // Open directory
    dir_ptr = opendir(dir_to_walk);
    if(!dir_ptr){
        printf("failure to open %s --- ernno %d \n", dir_to_walk, errno);
        perror("Error: ");
        printf("\n");
        return 1;
    }
    
    // Walk
    while ((dir_entry_ptr = readdir(dir_ptr))) {

        // Ignore '.' and '..'
        if (!strcmp(dir_entry_ptr->d_name, SINGLE_DOT) || !strcmp(dir_entry_ptr->d_name, DOUBLE_DOT))
			continue;


        strncpy(dir_path + dir_path_len, dir_entry_ptr->d_name, (FILENAME_MAX-dir_path_len));
        if(lstat(dir_path, &file_stat) == -1) {
			printf("Can't stat %s", dir_path);
			continue;
		}
        
        printf("%s\n", dir_path);
        if(S_ISLNK(file_stat.st_mode)){
            /* 
             * For now Don't follow links
             * can be compared against a flag
             * on whether or not to ignore
             */ 
            continue;
        }

        /* 
         * If it is a directory follow 
         * it to max depth
         */
        if(S_ISDIR(file_stat.st_mode)){
            // Depth first!
            if(curr_depth == max_depth){
                continue;
            }
            INC_DEPTH(curr_depth);
            walk_dir_recursive(dir_path, 0);
        }          
    }
    /* REQUIRED!!
     * If directory is not closed program 
     * will run out of fd's
     */
    closedir(dir_ptr); 

    RESET_DEPTH(curr_depth);
    return 0;
}

int main(int argc, char** argv){
    
    char* resolved_path = NULL;

    if( argc != 2){
        walk_dir_recursive((char*)"/", 0);
    }
    else{
        resolved_path = realpath(argv[1], NULL);
        if(resolved_path){
            printf("%s\n",argv[1]);
            walk_dir_recursive(resolved_path, 0);
            free(resolved_path);
        }
        else{
            printf("Invalid path: %s", argv[1]);
        }
    }

    return 0;
}