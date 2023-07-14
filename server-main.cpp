// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <signal.h>

// #define MAX_MSG_SIZE 16
// #define PORT 5001

// static volatile int  keepRunning = 1;
// void exitHandler(int dummy) {
//     keepRunning = 0;
// }

// int main() {
//     signal(SIGINT, exitHandler);

//     int serverSocket, newSocket;
//     struct sockaddr_in serverAddress, clientAddress;
//     socklen_t clientLength;
//     char message[MAX_MSG_SIZE+1];

//     // Create the server socket
//     serverSocket = socket(AF_INET, SOCK_STREAM, 0);
//     if (serverSocket < 0) {
//         perror("Failed to create socket");
//         exit(1);
//     }

//     // Set up server address
//     serverAddress.sin_family = AF_INET;
//     serverAddress.sin_addr.s_addr = INADDR_ANY;
//     serverAddress.sin_port = htons(PORT);

//     // Bind the socket to the specified port
//     if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
//         perror("Failed to bind the socket");
//         exit(1);
//     }

//     // Listen for incoming connections
//     listen(serverSocket, 1);
//     printf("Server listening on port %d\n", PORT);

//     // Accept a new connection
//     clientLength = sizeof(clientAddress);
//     newSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);
//     if (newSocket < 0) {
//         perror("Failed to accept connection");
//         exit(1);
//     }

//     // Receive and send messages
//     while (keepRunning) {
//         memset(message, 0, sizeof(message));

//         int received_bytes = recv(newSocket, message, MAX_MSG_SIZE, 0);
//         if (received_bytes < 0) {
//             perror("Failed to receive message");
//             exit(1);
//         }
//         message[received_bytes] = '\0';

//         printf("Received %i bytes.\n", received_bytes);
//         printf("Received message: %s\n", message);

//         // Process the message or perform any necessary operations

//         // Send a reply message
//         char reply[] = "Server reply: Message received\n";
//         if (send(newSocket, reply, strlen(reply), 0) < 0) {
//             perror("Failed to send reply");
//             exit(1);
//         }
//     }

//     // Close the sockets
//     close(newSocket);
//     close(serverSocket);

//     printf("All done :)\n");

//     return 0;
// }

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>

#define PORT 5001
#define QUEUE_SIZE 5

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

    printf("Server connection closed.\n");
    close(serverSocket);

    return 0;
}