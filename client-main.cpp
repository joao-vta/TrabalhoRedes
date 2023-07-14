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

        // we need inputSize+1 so the message sent includes '\0'
        int inputSize = strlen(input_msg);
        //int sent = 0;
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
        if(send(clientSocket, input_msg, inputSize+1, 0) < 0){
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

        if(strcmp(input_msg, "exit") == 0){
            running = false;
        }
    }

    printf("Client connection closed.\n");
    close(clientSocket);

    return 0;
}