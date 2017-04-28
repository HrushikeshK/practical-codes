/*
 * Write a program (C/C++) using TCP socket for wired network for following:
 * 1. Say Hello to each other
 * 2. File transfer
 * 3. Calculator (Arithmetic)
 * 4. Calculator (Trigonometry)
 * * * * * * * * * *
 * Input: ./tcp_client <IP> <port>
 * Output: Client side code connecting server
 * * * * * * * * * *
 */

#include <iostream>
#include <strings.h>
#include <string.h>
#include <fstream>
#include "csocket.h"		// Self implemented wrapper for client socket

using namespace std;

/* 
 * Because send() and recv() are not synchronous, 
 * A wrapper is written to make them synchronous.
 */
int send_sync(int sockfd, const char *data);
int recv_sync(int sockfd, char *buffer);

void init(int sockfd);
void say_hello(int sockfd);
void file_transfer(int sockfd);
void arith_calc(int sockfd);
void trigo_calc(int sockfd);


int main(int argc, char **argv) 
{
	int portno, sockfd, retval;
	int choice;
	csocket client_sock;
	char buffer[256];
	char *msg = "Connected to client...";

	if (argc != 3) {
		fprintf(stderr, "Usage: %s IP PORT\n", argv[0]);
		exit(0);
	}

	portno = atoi(argv[2]);
	sockfd = client_sock.get_sockfd();
	retval = client_sock.connect_client(argv[1], portno);

	bzero(buffer, 256);
	retval = recv_sync(sockfd, buffer);
	cout << buffer << endl;
	retval = send_sync(sockfd, msg);

	init(sockfd);

	return 0;
}


void init(int sockfd) 
{
		int choice, retval;

		do {
		cout << endl << "1. Say Hello to server" << endl << "2. File Transfer" << endl;
		cout << "3. Calculator (Arithmetic)" << endl << "4. Calculator (Trigonometry)" << endl;
		cout << "> ";
		cin >> choice;

		switch (choice) {
			case 1:
				say_hello(sockfd);
				break;
			case 2:
				file_transfer(sockfd);
				break;
			case 3:
				arith_calc(sockfd);
				break;
			case 4:
				trigo_calc(sockfd);
				break;
			default:
				cout << "Invalid Input" << endl;
				retval = send_sync(sockfd, "q");
				break;
		}
	} while (choice < 5);
}


/* 
 * Simple Client-Server Hello World program
 */
void say_hello(int sockfd) 
{
	char buffer[256];
	char *msg = "hello";
	int retval;

	retval = send_sync(sockfd, msg);

	bzero(buffer, 256);
	retval = recv_sync(sockfd, buffer);
	cout << "Server: " << buffer << endl;
}


void file_transfer(int sockfd)
{
	char buffer[256];
	char path[100];
	int file_count = 0;
	int retval;
	string file_name;
	const char *msg = "file_transfer";

	bzero(buffer, sizeof(buffer));
	bzero(path,sizeof(path));

	retval = send_sync(sockfd, msg);

	while((retval = recv_sync(sockfd,buffer)) > 0) {
    	if(strcmp(buffer, "<end_file_list>") == 0) 
      		break;
      	if(buffer[0] == '0') {
      		cout << "Error finding directory" << endl;
      		exit(0);
      	}    
    	cout << ++file_count << ") " << buffer << endl;
    	bzero(buffer, sizeof(buffer));
  	}

  	bzero(buffer, sizeof(buffer));
  	cout << "Enter File name: " << endl;
  	cin >> buffer;
  	file_name = buffer;
  	retval = send_sync(sockfd, buffer);

  	/*
  	 * File transfer code starts here
     * Currently it works for file size <= buffer size
  	 */
  	bzero(buffer, sizeof(buffer));
  	retval = recv_sync(sockfd, buffer);
  	if(!atoi(buffer)) {
    	cout << file_name << " is not a file" << endl;
    	return;
  	} else {
    	cout << "File found" << endl;
  	}

  	ofstream ofile;
  	
  	bzero(buffer, sizeof(buffer));
  	retval = recv_sync(sockfd, buffer);

  	ofile.open(file_name);
  	if(ofile) {
   		ofile << buffer << endl;
    	cout << "File " << file_name << " created..." << endl;
  	}
  	ofile.close(); 


}

/* 
 * Client sends the data in terms of val1 op val2
 * Server evaluates the expression and send to client
 * Client print it
 */
void arith_calc(int sockfd)
{
	int retval;
	char buffer[256];
	bzero(buffer, 256);
	char *msg;
	string a,b,c;

	msg = "arith";

	retval = send_sync(sockfd, msg);

	retval = recv_sync(sockfd, buffer);
	cout << buffer << endl;
	cin >> a >> b >> c;

	retval = send_sync(sockfd, a.c_str());
	retval = send_sync(sockfd, b.c_str());
	retval = send_sync(sockfd, c.c_str());

	retval = recv_sync(sockfd, buffer);
	cout << buffer << endl;
}

/*
 * Server sends options -> users chooses one of the options
 * Server calculates value and send it to client.
 * Client prints the output
 */
void trigo_calc(int sockfd)
{
	int ch;
	double angle;
	char buffer[256];
	int retval;

	const char *msg = "trigo";

	retval = send_sync(sockfd, msg);

	bzero(buffer, sizeof(buffer));
	retval = recv_sync(sockfd, buffer);
	cout << buffer << endl;
	cin >> ch;
	retval = send_sync(sockfd, to_string(ch).c_str());

	bzero(buffer, sizeof(buffer));
	retval = recv_sync(sockfd, buffer);
	cout << buffer << endl;
	cin >> angle;
	retval = send_sync(sockfd, to_string(ch).c_str());

	bzero(buffer, sizeof(buffer));
	retval = recv_sync(sockfd, buffer);
	cout << buffer << endl;
}


int send_sync(int sockfd, const char *data) 
{
	char size[4];

	sprintf(size, "%d", strlen(data));
	int retval = send(sockfd, size, sizeof(size), 0);

	if (retval < 0) {
		cout << "Error Sending data size" << endl;
		exit(0);
	}

	retval = send(sockfd, data, strlen(data), 0);

	if (retval < 0) {
		cout << "Error sending data" << endl;
		exit(0);
	}

	return retval;
}


int recv_sync(int sockfd, char *buffer) 
{
	char msg_size[4];
	bzero(msg_size, sizeof(msg_size));

	int retval = recv(sockfd, msg_size, sizeof(msg_size), 0);
	if (retval < 0) {
		cout << "Error receving msg size" << endl;
		exit(0);
	}

	bzero(buffer, sizeof(buffer));
	retval = recv(sockfd, buffer, atoi(msg_size), 0);
	if (retval < 0) {
		cout << "Error receving message" << endl;
		exit(0);
	}

	return retval;
}