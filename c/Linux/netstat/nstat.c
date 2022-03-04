#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define MAXBUFLEN 

void read_tcp(){

    struct stat tcp_fstat = {};
    char* tcp_line;
    char * file_buff = NULL;

    // Create a buffer for file size
    file_buff = malloc(40960);

    // Open file, read, close
    int tcp_file_fd = open("/proc/net/tcp", O_RDONLY);
    read(tcp_file_fd, file_buff, 40960);
    close(tcp_file_fd);

    //printf("%s\n", file_buff);

    tcp_line = strtok(file_buff, "\n");

    while(NULL != tcp_line){
        printf("%s\n", tcp_line);
        tcp_line = strtok(NULL, "\n");
    }
    
    free(file_buff);



}

int main(int argc, char** argv){

    read_tcp();

    return 0;
}