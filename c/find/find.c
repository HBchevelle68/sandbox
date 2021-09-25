#define _XOPEN_SOURCE 1			/* Required under GLIBC for nftw() */
#define _XOPEN_SOURCE_EXTENDED 1	/* Same */

#include <stdio.h>
#include <ftw.h>
#include <unistd.h>
#include <limits.h> 
#include <errno.h>
#include <stdlib.h>


typedef struct {

} entry_t;

int nftw_callback(const char *file, const struct stat *file_stat, int flag, struct FTW *ftw_meta){


    printf("File: %s\n", file);
    printf("Base: %s\n", (file+ftw_meta->base));
    printf("Depth: %d\n", ftw_meta->level);

    switch (flag) {
        case FTW_F:
            // Normal File
            break;
        case FTW_D:
            // Directory
        
        case FTW_SL:
            // Symlink

        default:
            break;
    }

    return EXIT_SUCCESS;
}


int find(char* path, char* pattern) {

    return nftw(path, nftw_callback, 100, 0);
}


int main(int argc, char** argv) {
    
    char *resolved_path = NULL;
    char cwd[PATH_MAX] = {};
    char *pattern = "*";

    if( argc != 2){
        getcwd(cwd, PATH_MAX);
        find(cwd, pattern);
    }
    else {
        resolved_path = realpath(argv[1], NULL);
        if(resolved_path) {
            if(argc != 3) {
                printf("No pattern given... grabing everything\n");

            }
            else{
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