// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <unistd.h>
// #include <signal.h>

// #define MAX_MSG_SIZE 16
// #define SERVER_IP "127.0.0.1"
// #define PORT 5001
// #define READLINE_BUFFER 4096

// static volatile int  keepRunning = 1;
// // Handles the CTRL+C signal
// void exitHandler(int dummy) {
//     keepRunning = 0;
// }

// char *readline(FILE *stream) {
//     char *string = 0;
//     int pos = 0; 

//     do{
//         if (pos % READLINE_BUFFER == 0) {
//             string = (char *) realloc(string, (pos / READLINE_BUFFER + 1) * READLINE_BUFFER);
//         }
//         string[pos] = (char) fgetc(stream);
//     }while(string[pos++] != '\n' && string[pos-1] != '\r' && !feof(stream));

//     string[pos-1] = 0;
//     string = (char *) realloc(string, pos);

//     return string;
// }

// int min(int a, int b) {
//     return a < b ? a : b;
// }

// int main() {
//     signal(SIGINT, exitHandler);

//     int clientSocket;
//     struct sockaddr_in serverAddress;
//     char message[MAX_MSG_SIZE];

//     // Create the client socket
//     clientSocket = socket(AF_INET, SOCK_STREAM, 0);
//     if (clientSocket < 0) {
//         perror("Failed to create socket");
//         exit(1);
//     }

//     // Set up server address
//     serverAddress.sin_family = AF_INET;
//     serverAddress.sin_port = htons(PORT);
//     if (inet_pton(AF_INET, SERVER_IP, &(serverAddress.sin_addr)) <= 0) {
//         perror("Invalid server address");
//         exit(1);
//     }

//     // Connect to the server
//     if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
//         perror("Failed to connect to the server");
//         exit(1);
//     }

//     // Send and receive messages
//     while (keepRunning) {
//         printf("Enter a message: ");
//         char* input_msg = readline(stdin);

//         int sent = 0;
//         int messageSize = strlen(input_msg);
//         // Fragmenting so that message never exceedes the MAX_MSG_SIZE
//         while (sent < messageSize) {
//             int sendSize = min(messageSize - sent, MAX_MSG_SIZE);
            
//             printf("Bytes Sent: %d\n", sendSize);
//             if (send(clientSocket, input_msg + sent, sendSize, 0) < 0) {
//                 perror("Failed to send message");
//                 exit(1);
//             }

//             sent += sendSize;
//             // We do this to guarantee that the packet will be fragmented
//             sleep(1);
//         }

//         memset(message, 0, sizeof(message));
//         if (recv(clientSocket, message, sizeof(message), 0) < 0) {
//             perror("Failed to receive reply");
//             exit(1);
//         }
//         printf("Received reply: %s\n", message);
//     }


//     // Close the socket
//     close(clientSocket);
//     printf("All done :)\n");

//     return 0;
// }

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 5001
#define SERVER_IP "127.0.0.1"

#define MAX_MSG_SIZE 16

int main(){
    // creating socket and verifying
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(clientSocket < 0){
        printf("Socket creation failed!\n");
        exit(1);
    }
    // there's no need to bind the socket to a port
    // the OS will automatically do it

    // setting up server IP and PORT
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    // converting IP from string to bytes
    if (inet_pton(AF_INET, SERVER_IP, &(serverAddress.sin_addr)) <= 0) {
        printf("Invalid server address!\n");
        exit(1);
    }

    // connect the client socket to server address
    if(connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) != 0){
        printf("Couldn't connect to server!\n");
        exit(0);
    }
    printf("Connection with server stabilished\n");
    
    // receive and send messages
    bool running = true;
    char input_msg[MAX_MSG_SIZE+1];
    char message[MAX_MSG_SIZE+1];
    while(running){
        printf("Enter a message: ");
        std::cin.getline(input_msg, sizeof(input_msg));

        int sent = 0;
        int inputSize = strlen(input_msg);
        // // Fragmenting so that message never exceedes the MAX_MSG_SIZE
        // while (sent < messageSize) {
        //     int sendSize = min(messageSize - sent, MAX_MSG_SIZE);

        //     printf("Bytes Sent: %d\n", sendSize);
        //     if (send(clientSocket, input_msg + sent, sendSize, 0) < 0) {
        //         perror("Failed to send message");
        //         exit(1);
        //     }

        //     sent += sendSize;
        //     // We do this to guarantee that the packet will be fragmented
        //     sleep(1);
        // }

        // sending message and veryfing
        if(send(clientSocket, message, inputSize, 0) < 0){
            printf("Failed to send message!\n");
            exit(1);
        }

        // receiving reply from server
        memset(message, 0, sizeof(message));
        if (recv(clientSocket, message, sizeof(message), 0) < 0) {
            printf("Failed to receive reply");
            exit(1);
        }
        printf("Received reply: %s\n", message);
    }

    printf("Client connection closed.\n");
    close(clientSocket);

    return 0;
}