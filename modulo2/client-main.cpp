#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <csignal>
#include <thread>
#include <vector>
#include <arpa/inet.h>

#define SERVER_PORT 5001
#define SERVER_IP "127.0.0.1"

#define MAX_MSG_SIZE 16

int running = true;

// creating socket and verifying
int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

void sigpipe_handler(int err){
    printf("Server disconnected. Closing client %d.\n", clientSocket);
    close(clientSocket);
    exit(1);
}

void send_message(std::string input_msg){

    while(running){
        //printf("Enter a message: ");
        std::getline(std::cin, input_msg);

        // we need inputSize+1 so the message sent includes '\0'
        int inputSize = input_msg.size();
        
        // TCP automatically fragments depending if our input_msg is greater than MAX_MSG_SIZE
        if(send(clientSocket, input_msg.c_str(), inputSize, 0) < 0){
            printf("Failed to send message!\n");
            exit(1);
        }

        if(strcmp(input_msg.c_str(), "exit") == 0){
            running = false;
        }
    }

    return;
}

void receive_message(){
    char message[MAX_MSG_SIZE+1];
    while(running){
        // receiving reply from server
        memset(message, 0, sizeof(message));
        int server_reply = recv(clientSocket, message, sizeof(message), 0);
        if (server_reply <= 0) {
            if (server_reply == 0) raise(SIGPIPE);
            printf("Failed to receive reply");
            exit(1);
        }

        message[server_reply]=0;
        printf("Received reply: %s\n", message);
    }

    return;
}

int main(){

    signal(SIGPIPE, sigpipe_handler);

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
    //bool running = true;
    std::string input_msg;

    std::vector<std::thread> v_threads;

    std::thread th_send(send_message, input_msg);
    std::thread th_recv(receive_message);

    /*
    while(running){
        
        std::thread th_send(send_message, input_msg);
        std::thread th_recv(receive_message, message);

        send_message(input_msg);
        receive_message(message);


    }
    */
    th_send.join();
    th_recv.join();

    printf("Client connection closed.\n");
    close(clientSocket);

    return 0;
}
