#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>

#define PORT 5001
#define QUEUE_SIZE 5
#define MAX_MSG_SIZE 16

int main(){
    // creating socket and verifying
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket < 0){
        printf("Socket creation failed!\n");
        exit(1);
    }

    // setup server IP and PORT
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    // bind socket to the given IP and verifying
    if(bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) != 0){
        printf("Socket bind failed!\n");
        exit(1);
    }

    // listen for QUEUE_SIZE connections
    if(listen(serverSocket, QUEUE_SIZE) != 0){
        printf("Error while trying to listen!\n");
        exit(1);
    }
    printf("Server listening on port %d\n", PORT);

    // accept connection from client and verifying
    struct sockaddr_in clientAddress;
    socklen_t clientLenght = sizeof(clientAddress);
    int newSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLenght);
    if(newSocket < 0){
        printf("Server failed to connect to client!\n");
        exit(0);
    }
    printf("Connection with client stabilished.\n");

    // receive and send messages
    bool running = true;
    char message[MAX_MSG_SIZE+1];
    while(running){
        // reseting message to receive new one
        memset(message, 0, sizeof(message));

        // receiving message from client
        int received_bytes = recv(newSocket, message, MAX_MSG_SIZE, 0);
        printf("Received message: %s (%i bytes)\n", message, received_bytes);
        if(received_bytes < 0){
            printf("Failed to receive message!\n");
            exit(1);
        }

        // adding '\0' to the end of the message to print it

        // sending a reply message
        char reply[] = "Server reply";
        if(send(newSocket, reply, strlen(reply)+1, 0) < 0){
            printf("Failed to send reply!\n");
            exit(1);
        }

        // exiting if client exit
        if(strcmp(message, "exit") == 0){
            running = false;
        }
    }

    printf("Server connection closed.\n");
    // closing server and client socket
    close(serverSocket);
    close(newSocket);

    return 0;
}