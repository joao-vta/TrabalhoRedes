#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#define MAX_MSG_SIZE 16
#define PORT 5001

static volatile int  keepRunning = 1;
void exitHandler(int dummy) {
    keepRunning = 0;
}

int main() {
    signal(SIGINT, exitHandler);

    int serverSocket, newSocket;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientLength;
    char message[MAX_MSG_SIZE+1];

    // Create the server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Failed to create socket");
        exit(1);
    }

    // Set up server address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    // Bind the socket to the specified port
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Failed to bind the socket");
        exit(1);
    }

    // Listen for incoming connections
    listen(serverSocket, 1);
    printf("Server listening on port %d\n", PORT);

    // Accept a new connection
    clientLength = sizeof(clientAddress);
    newSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);
    if (newSocket < 0) {
        perror("Failed to accept connection");
        exit(1);
    }

    // Receive and send messages
    while (keepRunning) {
        memset(message, 0, sizeof(message));

        int received_bytes = recv(newSocket, message, MAX_MSG_SIZE, 0);
        if (received_bytes < 0) {
            perror("Failed to receive message");
            exit(1);
        }
        message[received_bytes] = '\0';

        printf("Received %i bytes.\n", received_bytes);
        printf("Received message: %s\n", message);

        // Process the message or perform any necessary operations

        // Send a reply message
        char reply[] = "Server reply: Message received\n";
        if (send(newSocket, reply, strlen(reply), 0) < 0) {
            perror("Failed to send reply");
            exit(1);
        }
    }

    // Close the sockets
    close(newSocket);
    close(serverSocket);

    printf("All done :)\n");

    return 0;
}
