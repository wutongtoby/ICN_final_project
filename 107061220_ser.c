#include <stdio.h>
#include <stdlib.h>
#include <string.h>	// for bzero()
#include <unistd.h>	// for close()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	struct sockaddr_in serverAddress, clientAddress;
	int server_addr_length = sizeof(serverAddress);
    int client_addr_length = sizeof(clientAddress);
    int serverSocket, clientSocket;
	int ServerPortNumber;

	if (argc == 2) 
		ServerPortNumber = atoi(argv[1]); // the port number
   	
	serverSocket = socket(PF_INET, SOCK_STREAM, 0); // IP, TCP, default
	if (serverSocket < 0) { 
		fprintf(stderr, "Error creating socket : %s\n", strerror(errno));
		exit(0);
	}

	// initialize every detail for server, including family, IP address, etc.
 	bzero(&serverAddress, server_addr_length); // set everything in the struct to 0 
	serverAddress.sin_family = AF_INET; // == IP family
  	serverAddress.sin_port = htons(ServerPortNumber); // covert port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // server, accept any message

	if (bind(serverSocket,(struct sockaddr *) &serverAddress, server_addr_length) == -1) {
		fprintf(stderr, "Error binding : %s\n", strerror(errno));
		close(serverSocket);
		exit(0);
	}
	
	if (listen(serverSocket, 3) == -1) {
		fprintf(stderr, "Error listening : %s\n", strerror(errno));
		close(serverSocket);
		exit(0);
	}

	printf("Waiting...\n");
	// will wait until a client connect to the server
	if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &client_addr_length)) == -1) {
		printf("accept failed\n");
		close(serverSocket);
		exit(0);
	}
	printf("Client connect successfully\n");	

	int bytesRecv, bytesSend;
	int selection;
    char send_buf[500];
	char recv_buf[500];
	int message_store_len = 500;
	//char *message_store = (char *) malloc(sizeof(char) * message_store_len);
	char message_store[500] = "";
	int message_count = 0;
	char temp[5] = "";
    char *menu = "\
\n\n------------------Menu------------------\n\
1. Read all existing messages\n\
2. Write a new message\n\
Please type \"1\" or \"2\" to select an option: \0";

	//message_store[0] = '\0';
	//strcat(message_store, "\n\nAll messages:\n");
	// Send menu to client
	send_buf[0] = '\0'; // to let it to be recogize as a string
	// send menu at first 
	strcat(send_buf, menu);
	bytesSend = send(clientSocket, send_buf, sizeof(send_buf), 0);
	if (bytesSend < 0) 
		printf("Error sending packet\n");

  	while(1) {		
		// wait the client to send message
		bytesRecv = recv(clientSocket, recv_buf, sizeof(recv_buf), 0);
		if(bytesRecv < 0) 
			printf("Error receiving packet\n");
		
		printf("%s\n", recv_buf);

		if (!strncmp(recv_buf, "1", 1)) { // Read all existing message
			send_buf[0] = '\0'; // reset
			strcat(send_buf, "\n\nAll messages:\n");
			strcat(send_buf, message_store); 
			strcat(send_buf, menu);  
			// send the message back to the client
			bytesSend = send(clientSocket, send_buf, sizeof(send_buf), 0);
			if (bytesSend < 0) 
                printf("Error sending packet\n");
		}
		else if (!strncmp(recv_buf, "2", 1)) { // Write a new message
			// Ask the client to type message
			send_buf[0] = '\0';
			strcat(send_buf, "Type a new message:");
			bytesSend = send(clientSocket, send_buf, sizeof(send_buf), 0);
			if (bytesSend < 0)
                printf("Error sending packet\n");

			// get the the message from the client
			bytesRecv = recv(clientSocket, recv_buf, sizeof(recv_buf), 0);
			if (bytesRecv < 0) 
                printf("Error receiving packet\n");
			/*
			if (strlen(message_store) + strlen(recv_buf) > message_store_len) {
				message_store = (char *) realloc(message_store, sizeof(char) * message_store_len * 2);
				if (message_store == NULL) {
					printf("Message storage is full\n");
					exit(0);
				}
				message_store_len *= 2;
			}
			else {
				strcat(message_store, recv_buf);
				strcat(message_store, "\n\n");
			}
			*/
			sprintf(temp, "%d. ", ++message_count);
			strcat(message_store, temp);
			strcat(message_store, recv_buf);
			strcat(message_store, "\n");

			// Tell the client that we haved received the message
			send_buf[0] = '\0';
			strcat(send_buf, "New message sent.\n");
			strcat(send_buf, menu);
			bytesSend = send(clientSocket, send_buf, sizeof(send_buf), 0);
			if (bytesSend < 0) 
                printf("Error sending packet\n");
		}
		else { // garbage, send the menu again
			bytesSend = send(clientSocket, menu, strlen(menu), 0);
			if (bytesSend < 0) 
                printf("Error sending packet\n");		
		}	
	}      
	return 0;
}