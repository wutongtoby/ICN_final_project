#include <stdio.h>
#include <stdlib.h>
#include <string.h>	// for bzero()
#include <unistd.h>	// for close()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define AddressSize 20

int main(int argc, char *argv[])
{
	struct sockaddr_in serverAddress;
	int server_addr_length = sizeof(serverAddress);
	int serverSocket;
	int ServerPortNumber;
	char ServerIP[AddressSize]; 
		
	if (argc == 3) { // executable, IP address, port number
		strcpy(ServerIP, argv[1]);
		ServerPortNumber = atoi(argv[2]);
	}

	// Create socket
	serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0) {
		printf("Error creating socket\n");
		exit(0);
	}

	// Set the server information
	bzero(&serverAddress, server_addr_length);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(ServerPortNumber);
	serverAddress.sin_addr.s_addr = inet_addr(ServerIP); // convert 127.0.0.1 into some integer

	// Connect to server
	if (connect(serverSocket, (struct sockaddr *)&serverAddress, server_addr_length) == -1) {
		printf("connect failed\n");
		close(serverSocket);
		exit(0);
	}

	int bytesSend, bytesRecv;
	char send_buf[500];
    char recv_buf[500];	

	while (1) {
		bytesRecv = recv(serverSocket, recv_buf, sizeof(recv_buf), 0);
		if (bytesRecv < 0) 
            printf("Error recving packet\n");
		system("clear");
		printf("%s\n", recv_buf);
		fflush(stdin);
		scanf(" %[^\n]", send_buf); // read word until '\n'
		
		bytesSend = send(serverSocket, send_buf, sizeof(send_buf), 0);
		if (bytesSend < 0) 
            printf("Error sending packet\n");
	}
	return 0;
}