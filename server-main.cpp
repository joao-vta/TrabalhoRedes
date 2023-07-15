#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <unistd.h>

#define PORT 5002
#define QUEUE_SIZE 1
#define MAX_MSG_SIZE 16
#define MAX_CLIENTS 3

// amount of clients at time
std::atomic<int> n_clients_gl{0};

void connect_to_client(int serverSocket){

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
    
    // amount of clients is incremented
    n_clients_gl++;
    
    // receive and send messages
    bool running = true;
    char message[MAX_MSG_SIZE+1];
    while(running){
        // reseting message to receive new one
        memset(message, 0, sizeof(message));

        // receiving message from client
        int received_bytes = recv(newSocket, message, MAX_MSG_SIZE, 0);
        if(received_bytes < 0){
            printf("Failed to receive message!\n");
            exit(1);
        }
        printf("Received message: %s (%i bytes)\n", message, received_bytes);

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

    // amount of clients is decremented
    n_clients_gl--;
    //std::cout << ": " << n_clients_gl << '\n';

    return;
}

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
     
    // creates a vector of threads
    std::vector<std::thread> v_threads;

    /* TODO:
     * PROBLEMAS:
     *  um número fixo de clientes. 
     *  quando uma conexão é fechada, não se libera espaço para mais um cliente
    */
    int i = MAX_CLIENTS;
    do{ 
        std::thread th(connect_to_client, serverSocket);
        v_threads.push_back(std::move(th));
    }while(i--);

    for (std::thread & th : v_threads){
        th.join(); 
    }

    return 0;
}
