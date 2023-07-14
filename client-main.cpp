#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 5002
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
    std::string input_msg;
    char message[MAX_MSG_SIZE+1];
    while(running){
        printf("Enter a message: ");
        std::getline(std::cin, input_msg);

        // we need inputSize+1 so the message sent includes '\0'
        int inputSize = input_msg.size()+1;
        
        // TCP automatically fragments depending if our input_msg is greater than inputSize
        if(send(clientSocket, input_msg.c_str(), inputSize, 0) < 0){
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

        if(strcmp(input_msg.c_str(), "exit") == 0){
            running = false;
        }
    }

    printf("Client connection closed.\n");
    close(clientSocket);

    return 0;
}