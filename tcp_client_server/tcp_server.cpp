/*
 * Write a program (C/C++) using TCP socket for wired network for following:
 * 1. Say Hello to each other
 * 2. File transfer
 * 3. Calculator (Arithmetic)
 * 4. Calculator (Trigonometry)
 * * * * * * * * * *
 * Input: ./tcp_server <port>
 * Output: Server running and echo commands issued by client
 * * * * * * * * * *
 */

#include <iostream>
#include <strings.h> 
#include <string.h>
#include <sys/select.h>
#include <dirent.h>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <cmath>
#include "ssocket.h"		// Self implemented wrapper for server socket

#include <string>
#include <cstdio>
#include <cerrno>

using namespace std;

/* 
 * Because send() and recv() are not synchronous, 
 * A wrapper is written to make them synchronous.
 */
int send_sync(int sockfd, const char *data);
int recv_sync(int sockfd, char *buffer);

void init(int client_sockfd);
void say_hello(int client_sockfd);
void arith_calc(int client_sockfd);
void arith_trigo(int client_sockfd);
void file_transfer(int client_sockfd);


int main(int argc, char **argv) 
{
	int server_sockfd, client_sockfd, portno, bindval, retval;
	ssocket server_sock;
	char buffer[256];
	char *msg = "Connected to server...";

	if (argc != 2) {
		fprintf(stderr, "usage %s port\n",argv[0]);
		exit(0);
	}

	portno = atoi(argv[1]);
	server_sockfd = server_sock.get_sockfd();
	bindval = server_sock.bind_server(portno);
	server_sock.listen_server(5);

	client_sockfd = server_sock.accept_server();

	retval = send_sync(client_sockfd, msg);
	retval = recv_sync(client_sockfd,buffer);
	cout << buffer << endl;

	init(client_sockfd);

	return 0;
}


void init(int client_sockfd)
{
	char buffer[256];
	int retval;

	do {
		bzero(buffer, 256);
		retval = recv_sync(client_sockfd, buffer);
		cout << "Client: " << buffer << endl;

		if (strcmp(buffer, "hello") == 0)
			say_hello(client_sockfd);
		else if (strcmp(buffer, "arith") == 0)
			arith_calc(client_sockfd);
		else if (strcmp(buffer, "trigo") == 0)
			arith_trigo(client_sockfd);
		else if (strcmp(buffer, "file_transfer") == 0)
			file_transfer(client_sockfd);
		else
			break;

	} while (strcmp(buffer, "q") != 0);
}

int send_sync(int sockfd, const char *data) 
{
	char size[4];
	bzero(size, sizeof(size));

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
	bzero(buffer, sizeof(buffer));
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

/* 
 * Simple Client-Server Hello World program
 */
void say_hello(int client_sockfd) 
{
	char *msg = "Hello Client...";
	int retval;


	retval = send_sync(client_sockfd, msg);
}

/* 
 * Client sends the data in terms of val1 op val2
 * Server evaluates the expression and send to client
 * Client print it
 */
void arith_calc(int client_sockfd) 
{
	char buffer[256];
	int retval, expr_val;
	char *msg = "Enter expression";
	bzero(buffer, 256);

	retval = send_sync(client_sockfd, msg);

	retval = recv_sync(client_sockfd, buffer);
	int a = atoi(buffer);
	
	retval = recv_sync(client_sockfd, buffer);
	char op = buffer[0];

	retval = recv_sync(client_sockfd, buffer);
	int b = atoi(buffer);

	switch (op) {
		case '+':
		expr_val = a + b;
		break;

		case '-':
		expr_val = a - b;
		break;

		case '/':
		expr_val = a / b;
		break;

		case '*':
		expr_val = a * b;
		break;

		default:
		expr_val = -255;
		break;
	}

	// Convert int -> string -> const char* 
	retval = send_sync(client_sockfd, to_string(expr_val).c_str());
	
}

/*
 * Server sends options -> Client choose the corresponding option
 * Server calculates value and sends it to client.
 * Client prints the value.
 */
void arith_trigo(int client_sockfd) 
{
	char *msg = "1. cos\n2. sin\n3. tan\n4. acos\n5. asin\n6. atan";
	int retval;
	double result_val;
	char angle[4];
	char choice[4];
	bzero(choice, sizeof(choice));
	bzero(angle, sizeof(angle));

	retval = send_sync(client_sockfd, msg);
	retval = recv_sync(client_sockfd, choice);
	int ch = atoi(choice);

	msg = "Enter Angle in radians";
	retval = send_sync(client_sockfd, msg);
	retval = recv_sync(client_sockfd, angle);
	int angle_rad = atoi(angle);

	switch (ch) {
		case 1:
			result_val = cos(angle_rad);
			break;
		case 2:
			result_val = sin(angle_rad);
			break;
		case 3:
			result_val = tan(angle_rad);
			break;
		case 4:
			result_val = acos(angle_rad);
			break;
		case 5:
			result_val = asin(angle_rad);
			break;
		case 6:
			result_val = atan(angle_rad);
			break;
		default:
		 	result_val = -255.0;
		 	break;
	}
	// Convert int -> string -> const char* 
	retval = send_sync(client_sockfd, to_string(result_val).c_str());
}

/*
 * File is transferred from current working directory.
 * Client fetches file from server and display.
 * User choose which file to transfer.
 * Server transfer the required file at client side.
 */
void file_transfer(int client_sockfd) 
{
  DIR* dir;
  struct dirent *ent;

  int retval;
  char name_size[4];
  char buffer[256];
  char path[100];
  bzero(path,100);

  /*
   * This code is to display files for a given path
   * File transfer code will be written after this code
   */
  if ((dir = opendir(getcwd(path, sizeof(path)))) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
    	retval = send_sync(client_sockfd, ent->d_name);
    	if (retval <= 0) {
    		perror("Error displaying files");
    		exit(0);
    	}
    } 
    retval = send_sync(client_sockfd,"<end_file_list>");
    if(retval == -1)
    	perror("Error Sending ending character");
    closedir(dir);
  } else {
  	retval = send_sync(client_sockfd,"0");
    perror("Directory not found");
  }

  bzero(buffer, sizeof(buffer));
  retval = recv_sync(client_sockfd, buffer);
  cout << buffer << endl;

  /*
   * File Transfer code starts here
   */

  	struct stat sb;
	if(stat(buffer,&sb) == 0 && S_ISREG(sb.st_mode)) {
		send_sync(client_sockfd,"1");
	} else {
		send_sync(client_sockfd,"0");
		return;
	}

	ifstream infile;
	infile.open(buffer, ios::binary | ios::in);

	 if (infile) {
  
        std::string contents;
        infile.seekg(0, std::ios::end);
        contents.resize(infile.tellg());
        infile.seekg(0, std::ios::beg);
        infile.read(&contents[0], contents.size());
        infile.close();
    
        cout << contents.size() << endl;

 		cout << "Length: " << contents.size() << endl;

        char length_char[10];
        bzero(length_char, sizeof(length_char));
        sprintf(length_char, "%d", contents.size());
        send_sync(client_sockfd, length_char);

        char *data_source = new char[contents.size()+1];

        int bytes_sent = 0;
        int byte_count = 0;
        int index = 0;

    /*
     * Since we are also trying to send binary data (audio, video),
     * we can't just read data and send data.
     * Reason: Whenever we read the binary data in char[], the
     * c-style string terminates whenever it finds null-character '\0'
     * In order to avoid it, we send chunks of data at a time.
     * A chunk equals one c-style string
     */
        while (byte_count < contents.size()) {
            if (contents[byte_count] == '\0' || byte_count == contents.size() - 1) {
                data_source[index] = contents[byte_count];

                int msg_len = send_sync(client_sockfd, data_source);
                bytes_sent += msg_len;
                
                // second condition is to check if last character to be sent is '\0'
                if (byte_count != contents.size() - 1 || contents[contents.size() - 1] == '\0')
                    ++bytes_sent;
            
                byte_count++;
                index = 0;
                bzero(data_source, contents.size()+1);
            } else {
                data_source[index] = contents[byte_count];
                index++;
                byte_count++;
            }
        }
        delete[] data_source;

        cout << "File transferred successfully" << endl;
        cout << "Bytes sent: " << bytes_sent << endl;
    }
	infile.close();
}