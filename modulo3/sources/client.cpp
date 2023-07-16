#include "../includes/client.hpp"

Client::Client(){
    // there's no need to bind the socket to a port
    // the OS will automatically do it
    this->SOCKET = socket(AF_INET, SOCK_STREAM, 0);
    if(this->SOCKET < 0){
        printf("Socket creation failed!\n");
        exit(1);
    }
}

void Client::_connect(int serverPort, const char* serverIP){
    // setting up server IP and PORT
    SockAddrIn serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);

    // converting IP from string to bytes
    if (inet_pton(AF_INET, serverIP, &(serverAddress.sin_addr)) <= 0) {
        printf("Invalid server address!\n");
        exit(1);
    }

    // connect the client socket to server address
    if(connect(this->SOCKET, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) != 0){
        printf("Couldn't connect to server!\n");
        exit(1);
    }
    printf("Connection with server stabilished\n");
}

void Client::_join(char channel_name[], int serverPort, const char* serverIP){
    
    this->_connect(serverPort, serverIP);

    int inputSize = strlen(channel_name);
    if(send(this->SOCKET, channel_name, inputSize, 0) < 0){
        printf("Failed to send group name!\n");
        exit(1);
    }

    return;
}

void Client::setNickname(string nickname){
    this->nickname = nickname;
}

void Client::_send(string &message){
    int inputSize = message.size();
    
    // TCP automatically fragments depending if our message is greater than MAX_MSG_SIZE
    if(send(this->SOCKET, message.c_str(), inputSize, 0) < 0){
        printf("Failed to send message!\n");
        exit(1);
    }
}

void Client::_receive(char *message, int size){
    int server_reply = recv(this->SOCKET, message, size, 0);
    if (server_reply <= 0) {
        if (server_reply == 0) raise(SIGPIPE);
        printf("Failed to receive reply");
        exit(1);
    }

    message[server_reply] = '\0';
}

