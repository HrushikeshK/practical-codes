/*
 * Write a program using UDP (C/C++) sockets to enable file transfer 
 * (Script, Text, Audio and Video one file each) between two machines
 ***********************
 * Input: ./client
 * Output: Client side code connecting server and receiving file
 */

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>

using namespace std;

/* 
 * Because send() and recv() are not synchronous, 
 * A wrapper is written to make them synchronous.
 */
int send_sync(int fd, char* msg, struct sockaddr *addr, socklen_t len);
int recv_sync(int fd, char* buf, struct sockaddr *addr, socklen_t len);

void file_transfer(int fd, struct sockaddr *addr, socklen_t len);

#define PORT 9090
#define BUFFSIZE 4096

/*
 * Create socket
 * Since we are sending data over UDP, we don't need to connect before
 * data. Just send data
 */
int main(int argc, char **argv)
{
	struct sockaddr_in serv_addr;
	struct hostent *hp;
	int fd, retval;
	socklen_t len = sizeof(serv_addr);
	char *host = argv[1];
	char *msg = "Hello server";
	char buffer[BUFFSIZE];

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	hp = gethostbyname(host);
	if (!hp) {
		fprintf(stderr, "Could not obtain host address of %s\n", host);
		return 0;
	}

	memcpy((void *)&serv_addr.sin_addr, hp->h_addr_list[0], hp->h_length);

	retval = send_sync(fd, msg, (struct sockaddr *) &serv_addr, len);
	cout << "Message sent: " << msg << endl;

	bzero(buffer, sizeof(buffer));
	retval = recv_sync(fd, buffer, (struct sockaddr*) &serv_addr, len);
	cout << "Reply Received: " << buffer << endl;

	file_transfer(fd, (struct sockaddr *) &serv_addr, len);

	close(fd);
	return 0;
}

int send_sync(int fd, char* msg, struct sockaddr *addr, socklen_t len) 
{
	char msg_size[4];
	int retval;
	bzero(msg_size, sizeof(msg_size));

	sprintf(msg_size, "%d", strlen(msg));

	retval = sendto(fd, msg_size, sizeof(msg_size), 0, addr, len);

	if (retval < 0) {
		perror("Error sendto");
		exit(0);
	}

	retval = sendto(fd, msg, strlen(msg), 0, addr, len);

	if (retval < 0) {
		perror("Error sendto");
		exit(0);
	}

	return retval;
}

int recv_sync(int fd, char* buf, struct sockaddr *addr, socklen_t len) 
{
	int retval;
	char msg_size[4];
	bzero(buf, sizeof(buf));
	bzero(msg_size, sizeof(msg_size));

	retval = recvfrom(fd, msg_size, sizeof(msg_size), 0, addr, &len);

	if (retval < 0) {
		perror("Error recvfrom");
		exit(0);
	}

	retval = recvfrom(fd, buf, atoi(msg_size), 0, addr, &len);

	if (retval < 0) {
		perror("Error recvfrom");
		exit(0);
	}

	return retval;
}

/*
 * The function file_transfer() first displays all files from server
 * side and then user chooses which file to download.
 */

void file_transfer(int fd, struct sockaddr *addr, socklen_t len)
{
	char buffer[256];
	char path[100];
	int file_count = 0;
	int retval;
	string file_name;

	bzero(buffer, sizeof(buffer));
	bzero(path,sizeof(path));

	while((retval = recv_sync(fd, buffer, addr, len)) > 0) {
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
  	retval = send_sync(fd, buffer, addr, len);

  	/*
  	 * File transfer code starts here
  	 */
  	bzero(buffer, sizeof(buffer));
  	retval = recv_sync(fd, buffer, addr, len);
  	if(!atoi(buffer)) {
    	cout << file_name << " is not a file" << endl;
    	return;
  	} else {
    	cout << "File found" << endl;
  	}

  	ofstream ofile;


  	bzero(buffer, sizeof(buffer));
  	retval = recv_sync(fd, buffer, addr, len);
  	int length = atoi(buffer);
  	cout << "Length Received: " << length << endl;
  	char *data_source = new char[length];

  	ofile.open(file_name, ios::binary | ios::out);
  	
  	int bytes_recv = 0;
  	char *ack = "1";
  	
  	/*
	 * Receive some data and send ack.
	 * Ideally in udp, ack should not be sent
	 * But if we want 100% data transfer, we need to 
	 * do it.
	 * Try removing that line from both files.
  	 */
  	do {
  		bzero(data_source, length);
  		int msg_len = recv_sync(fd, data_source, addr, len);
  		bytes_recv += msg_len;

  		// Need to make transfer synchronous
  		send_sync(fd, ack, addr, len);

  		if (bytes_recv != length) {
  			data_source[msg_len] = '\0';
  			bytes_recv++;	// need to consider "\0" explicitly
  		}
  		
  		if(ofile) {
  			// need to consider "\0" explicitly
   			if (bytes_recv != length)
   				ofile.write(data_source, msg_len+1);
   			else
   				ofile.write(data_source, msg_len);
   		} else {
  			cout << "Unbale to write data" << endl;
   		}

  	} while (bytes_recv < length);

  	cout << "File " << file_name << " created..." << endl;
  	cout << "Total bytes Received: " << bytes_recv << endl;

	delete[] data_source;
  	ofile.close(); 
}