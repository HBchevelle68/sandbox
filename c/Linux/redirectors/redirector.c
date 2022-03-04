#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h> 
#include <stdint.h> 
#include <stdlib.h>
#include <pthread.h>
#include <sys/poll.h>
#include <errno.h>

#define BUFMAX 1500

#define RDR_FAILURE -1
#define RDR_SUCCESS  0

#define S1FLAG 1
#define S2FLAG 2

#define BYTE sizeof(uint8_t)
#define FLAGFLIP(x) (x^0x3)


// Leave these for now...
#define S2_IP   "192.168.56.140" 
#define S2_PORT 12345

typedef struct {
	/* Server Listener FD */
	uint16_t listen_sock;
	struct sockaddr_in listen_addr;

	/* Side 1 -- sock from accept */
	uint16_t s1_sock;
	pthread_t s1_thread; // Side 1 thread
	
	/* Side 2 -- initiate connection */
	uint16_t s2_sock;
	struct sockaddr_in connect_addr;
	pthread_t s2_thread; // Side 2 thread 

	/* Status flag */
	uint8_t status;
} redir_t;

/* Global redirection struct */
static redir_t redirector;
/*
 * Struct for threads
 * 
 * @read_sock -> socket to perform ONLY reads on
 * @write_sock -> socket to perform ONLY write on
 * @opposing_flag -> flag representing other side of redirector
 * @side_of_conn -> flag representing "my" side of redirector
 */
typedef struct {
	uint16_t* read_sock;
	uint16_t* write_sock;
	uint8_t	  opposing_flag;
	uint8_t   side_of_conn;
} loop_t;

/*
 * Generic function
 * returns a status code for now incase expansion wanted later
 * 
 * ufd -> struct pollfd of your choice
 * sock_to_read -> sock reading from
 * sock_to_write -> sock to write to
 * flag -> opposing sides flag to check 
 */
int poll_and_read(uint16_t sock_to_read, uint16_t sock_to_write, uint8_t flag) {
	int readret = 0;
	int pollret = 0;
	char buffer[BUFMAX];

	/* 
	 * Set up polling struct
	 */ 
	struct pollfd ufd;
	memset(&ufd, 0, sizeof(ufd));
	ufd.fd = sock_to_read;
	ufd.events = POLLIN | POLLPRI;

	while(1) {
		/*
		 * Poll for 2 sec
		 * then check for teardown flag
		 */
		pollret = poll(&ufd, 1, 2000);
		if(pollret > 0) {
			// Data ready to be read
			memset(buffer, 0, BUFMAX);
			readret = read(sock_to_read, &buffer, BUFMAX);
			if( readret > 0) {
				/* Successful read. Write buffer to other side */ 
				if((write(sock_to_write, &buffer, readret)) < 0) {
					printf("Error writing from side %d\n", FLAGFLIP(flag));
					return RDR_FAILURE;
				}

			} 
			else if( readret == 0 ) {
				/* Connection closed time to clean up */
				printf("Tearing down side %d\n", FLAGFLIP(flag));
				return RDR_SUCCESS;

			} 
			else {
				/* Error on read */
				printf("Error reading from side %d\n", FLAGFLIP(flag));
				return RDR_FAILURE;
			}
		}
		else if (pollret == 0){
			/* Timeout, check flag */
			if(!(redirector.status & flag)){
				printf("Detected side %d tore down, tearing down\n", FLAGFLIP(flag));
				return RDR_SUCCESS;
			}
		}
		else{
			/* Polling error */ 
			printf("Polling error side %d -> errno:%d\n", FLAGFLIP(flag), errno);
			return RDR_FAILURE;
		}
	}
}

/*
 * I really dislike that these are the exact same function....
 */
void* network_loop(void *args){
	
	loop_t* side_thrd_args = (loop_t*)args;
	
	/* side_thrd_args 1 LOOP */
	poll_and_read(*(side_thrd_args->read_sock), *(side_thrd_args->write_sock), side_thrd_args->opposing_flag);

	/*
	 * Clean up
	 * ptr's to global static struct members 
	 */
	close(*(side_thrd_args->read_sock));
	*(side_thrd_args->read_sock) = 0;
	redirector.status &= (uint8_t)(~(side_thrd_args->side_of_conn));

	return RDR_SUCCESS;
}


/*
 * Build side 2, verify connect and kick off s2 loop thread 
 */
int build_side2(){

	loop_t* side2 = NULL;

	/*
	* Build out connection back to dest
	* then begin loop
	*/
	if((redirector.s2_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0){
		printf("socket error s2\n");
		return RDR_FAILURE;
	}

	/* Build out side 2 addr struct */
	memset(&redirector.connect_addr, 0, sizeof(redirector.connect_addr));
	redirector.connect_addr.sin_family = AF_INET;
	redirector.connect_addr.sin_addr.s_addr = inet_addr(S2_IP);
	redirector.connect_addr.sin_port = htons(S2_PORT);

	// Connect the dest socket to server socket 
	if (connect(redirector.s2_sock, (struct sockaddr*)&redirector.connect_addr, sizeof(redirector.connect_addr)) != 0) { 
		printf("connection with the server failed...\n"); 
		close(redirector.s2_sock);
		return RDR_FAILURE; 
	} 
	printf("Side 2 connected!\n");
	
	/* Build out side 2 struct */
	side2 = (loop_t*) malloc(sizeof(*side2) * BYTE); 
	side2->read_sock = &(redirector.s2_sock);
	side2->write_sock = &(redirector.s1_sock);
	side2->opposing_flag = S1FLAG;
	side2->side_of_conn = S2FLAG; 

	// Kick off thread
	if((pthread_create(&redirector.s2_thread, NULL, network_loop, side2)) != 0){
		printf("side 2 thread creation failed\n");
	}
	redirector.status |= S2FLAG;
	return RDR_SUCCESS;
}

int build_side1(){
	/*
	 * Build the structure to pass into thread
	 * be weary oh getting sockets flipped
	 */
	loop_t* side1 = NULL;
	side1 = (loop_t*) malloc(sizeof(*side1) * BYTE); 

	side1->read_sock = &(redirector.s1_sock);
	side1->write_sock = &(redirector.s2_sock);
	side1->opposing_flag = S2FLAG;
	side1->side_of_conn = S1FLAG; 

	// Kick off thread
	if((pthread_create(&redirector.s1_thread, NULL, network_loop, side1)) != 0){
		printf("side 1 thread creation failed\n");
	}
	redirector.status |= S1FLAG;
	return RDR_SUCCESS;
}

int build_listener(){
	if((redirector.listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0){
		printf("socket error\n");
		return RDR_FAILURE;
	}

	//Build out server struct
	memset(&redirector.listen_addr, 0, sizeof(redirector.listen_addr));
	redirector.listen_addr.sin_family = AF_INET;
	redirector.listen_addr.sin_addr.s_addr = INADDR_ANY;
	redirector.listen_addr.sin_port = htons(54321);

	/* Setsockopt */
	if(setsockopt(redirector.listen_sock, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &(int){1}, sizeof(uint32_t)) < 0){
		close(redirector.listen_sock);
		printf("setsockopt error\n");
		return RDR_FAILURE; 
	}
	
	/* Bind */
	if(bind(redirector.listen_sock, (struct sockaddr*)&redirector.listen_addr, sizeof(struct sockaddr_in)) < 0){
		close(redirector.listen_sock);
		printf("bind error\n");
		return RDR_FAILURE; 
	}
	
	/* Listen -- only looking for a single connection */
	if(listen(redirector.listen_sock, 1)){
		close(redirector.listen_sock);
		printf("listen error\n");
		return RDR_FAILURE; 
	}

	return RDR_SUCCESS;
}

/*
 * This will listen for a connection, once recv'd 
 * Will ONLY read from client
 * Will ONLY write to endpoint
 */
int listener(){

	/* Connection Variables */
	struct sockaddr_in	cliaddr = {0};
    uint32_t cli_sock = 0;
    socklen_t addrlen = sizeof(cliaddr);

	if(build_listener() < 0){
		printf("Building side 1 error");
		return RDR_FAILURE;
	}

    while(1){
		printf("Listening for connection...\n");
		redirector.s1_sock = accept(redirector.listen_sock, (struct sockaddr*)(&cliaddr), &addrlen);
		printf("Connection recv'd on listener!\n");

		if(build_side1() < 0) {
			close(redirector.listen_sock);
			printf("build_side1 error\n");
			return RDR_FAILURE;
		}
		if(build_side2() < 0) {
			close(redirector.listen_sock);
			printf("build_side2 error\n");
			return RDR_FAILURE;
		}	

		pthread_join(redirector.s1_thread, NULL);
		pthread_join(redirector.s2_thread, NULL);
	}

	return RDR_SUCCESS;
}



int main(){
	memset(&redirector, 0, sizeof(redirector));
	return listener();
}