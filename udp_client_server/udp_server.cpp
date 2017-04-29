/*
 * Write a program using UDP (C/C++) sockets to enable file transfer 
 * (Script, Text, Audio and Video one file each) between two machines
 ***********************
 * Input: ./server
 * Output: Server running and sending file to one client at time
 */

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fstream>
#include <sys/stat.h>

#include <string>
#include <cstdio>
#include <cerrno>

using namespace std;

/* 
 * Because send() and recv() are not synchronous, 
 * A wrapper is written to make them synchronous.
 */
int send_sync(int fd, char* msg, struct sockaddr *addr, socklen_t len);
int recv_sync(int fd, char* buf, struct sockaddr *addr, socklen_t len);

void file_transfer(int fd, struct sockaddr *addr, socklen_t len);

#define PORT 9090
#define BUFFSIZE 1024

/*
 * Create socket
 * Since we are sending data over UDP, we don't need to connect before
 * data. Just send data
 */
int main(int argc, char **argv)
{
	struct sockaddr_in serv_addr; 
	struct sockaddr_in client_addr;
	int fd, retval;
	char buffer[BUFFSIZE];
	socklen_t len = sizeof(client_addr);
	char *msg = "Hello client";

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	if (fd < 0) {
		perror("Error creating socket");
		exit(0);
	}

	// Bind 
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);

	retval = bind(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	if (retval < 0) {
		perror("Error Binding");
		exit(0);
	}

	cout << "Waiting on port: " << PORT << endl;

	bzero(buffer, sizeof(buffer));
	retval = recv_sync(fd, buffer, (struct sockaddr *) &client_addr, len);
	cout << "Message from client: " << buffer << endl;

	retval = send_sync(fd, msg, (struct sockaddr *) &client_addr, len);
	cout << "Reply sent: " << msg << endl;

    file_transfer(fd, (struct sockaddr *) &client_addr, len);

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
    cout << "Size: " << strlen(msg) << endl;
	if (retval < 0) {
		perror("Error sendto");
		exit(0);
	}

	return retval;
}

/*
 * The function file_transfer() first displays all files from server
 * side and then user chooses which file to download.
 */
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

void file_transfer(int fd, struct sockaddr *addr, socklen_t len) 
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
    	retval = send_sync(fd, ent->d_name, addr, len);
    	if (retval <= 0) {
    		perror("Error displaying files");
    		exit(0);
    	}
    } 
    retval = send_sync(fd,"<end_file_list>", addr, len);
    if(retval == -1)
    	perror("Error Sending ending character");
    closedir(dir);
  } else {
  	retval = send_sync(fd,"0", addr, len);
    perror("Directory not found");
  }

  bzero(buffer, sizeof(buffer));
  retval = recv_sync(fd, buffer, addr, len);
  cout << buffer << endl;

  /*
   * File Transfer code starts here
   */

  	struct stat sb;
	if(stat(buffer,&sb) == 0 && S_ISREG(sb.st_mode)) {
		send_sync(fd,"1", addr, len);
	} else {
		send_sync(fd,"0", addr, len);
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
        send_sync(fd, length_char, addr, len);

        char *data_source = new char[contents.size()+1];

        int bytes_sent = 0;
        int byte_count = 0;
        int byte_count_index = 0;
        int index = 0;
        char ack[2];

    /*
     * Send some data and receive an ack.
     * Ideally in udp, ack should not be sent
     * But if we want 100% data transfer, we need to 
     * do it.
     * Try removing that line from both files.
     */

    /*
     * since we are trying to send binary data (audio, video),
     * we can't just read data and send data.
     * Reason: Whenever we read the binary data in char[], the
     * c-style string terminates whenever it finds null-character '\0'
     * In order to avoid it, we send chunks of data at a time.
     * A chunk equals one c-style string
     */
        while (byte_count < contents.size()) {
            if (contents[byte_count] == '\0' || byte_count == contents.size() - 1) {
                data_source[index] = contents[byte_count];

                int msg_len = send_sync(fd, data_source, addr, len);
                bytes_sent += msg_len;
                
                // second condition is to check if last character to be sent is '\0'
                if (byte_count != contents.size() - 1 || contents[contents.size() - 1] == '\0')
                    ++bytes_sent;

                // Need to make transfer synchronous
                bzero(ack, sizeof(ack));
                recv_sync(fd, ack, addr, len); 
                     
                cout << "Bytes sent: " << bytes_sent << ", Remain: " << contents.size() - bytes_sent << endl;
            
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
    }
	infile.close();
}