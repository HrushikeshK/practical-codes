#ifndef CSOCKET_H
#define CSOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>  	// exit(), perror()
#include <stdio.h> 		// stderror
#include <strings.h> 	// bzero()

class csocket {
private:
	int sock_fd;
	struct sockaddr_in serv_addr;
	struct hostent *host;
	void error(char* msg);

public:
	csocket();
	int connect_client(const char* hostname, int port_no);
	int get_sockfd();
};

void csocket::error(char* msg) {
	perror(msg);
  	exit(0);
}

csocket::csocket() {
	this->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->sock_fd < 0)
		error("ERROR opening socket");
}

int csocket::connect_client(const char* hostname, int port_no) {
	this->host = gethostbyname(hostname);
	if (this->host == NULL) {
		fprintf(stderr, "ERROR, no such host");
		exit(0);
	}

	bzero((char *) &(this->serv_addr), sizeof(this->serv_addr));
	this->serv_addr.sin_family = AF_INET;
	this->serv_addr.sin_port = htons(port_no);
	bcopy((char *) this->host->h_addr, (char *) &(this->serv_addr.sin_addr.s_addr), this->host->h_length);

	int retVal = connect(this->sock_fd, (struct sockaddr*) &(this->serv_addr), sizeof(this->serv_addr));

	if(retVal < 0)
		error("ERROR connecting");
	return retVal;
}

int csocket::get_sockfd() {
	return this->sock_fd;
}


#endif