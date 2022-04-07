#include <stdio.h>
#include <unistd.h>


int main(int argc, char** argv){

    char cmd[8] = " id;who";

    char* cmd2 = cmd;

    execl("/bin/sh", "sh", "-c", cmd2, (char*)NULL);
    return 0;
}