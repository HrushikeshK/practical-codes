#ifndef SSOCKET_H
#define SSOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h> 		// perror()
#include <stdlib.h> 	// exit()

class ssocket {
private:
	struct sockaddr_in serv_addr, cli_addr;
	int sock_fd;
	void error(char *msg);
public:
	ssocket();
	int bind_server(int port_no);
	int accept_server();
	void listen_server(int queues);
	int get_sockfd();
};

void ssocket::error(char* msg) {
	perror(msg);
  	exit(0);
}


ssocket::ssocket() {
	int optval = 1;
	this->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
	if(this->sock_fd < 0)
		this->error("ERROR opening socket");
}

int ssocket::bind_server(int port_no) {
	this->serv_addr.sin_family = AF_INET;
	this->serv_addr.sin_port = htons(port_no);
	this->serv_addr.sin_addr.s_addr = INADDR_ANY;

	int retVal = bind(sock_fd, (struct sockaddr *) &(this->serv_addr), sizeof(this->serv_addr));

	if(retVal < 0)
		this->error("ERROR on binding");
	return retVal;
}

int ssocket::accept_server() {
	unsigned int cli_len = sizeof(this->cli_addr);
	int retVal =  accept(this->sock_fd, (struct sockaddr *) &(this->cli_addr), &cli_len);
	if (retVal < 0)
		this->error("ERROR on accept");
	return retVal;
}

void ssocket::listen_server(int queues) {
	listen(this->sock_fd, queues);
}

int ssocket::get_sockfd() {
	return this->sock_fd;
}

#endif