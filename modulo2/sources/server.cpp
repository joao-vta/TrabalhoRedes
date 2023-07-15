#include "../includes/server.hpp"

Server::Server(int port, int max_msg_size){
    this->MAX_MSG_SIZE = max_msg_size;
    // initializing client index
    this->CURR_CLIENT_INDEX = 0;
    // assigning max connections

    // creating server socket
    this->SOCKET = socket(AF_INET, SOCK_STREAM, 0);
    if(this->SOCKET < 0){
        printf("Socket creation failed!\n");
        exit(1);
    }

    // setup server IP and PORT
    this->ADDRESS.sin_family = AF_INET;
    this->ADDRESS.sin_addr.s_addr = INADDR_ANY;
    this->ADDRESS.sin_port = htons(port);

    // bind socket to the given IP and verifying
    if(bind(SOCKET, (struct sockaddr *)&this->ADDRESS, sizeof(this->ADDRESS)) != 0){
        printf("Socket bind failed!\n");
        exit(1);
    }

    printf("Server Socket Successfully binded.\n");
}

void Server::_listen(){
    if(listen(this->SOCKET, 3) != 0){
        printf("Error while trying to listen!\n");
        exit(1);
    }
}

int Server::_accept(){
    // initializing connection
    Connection currConn;
    // accept connection from client and verifying
    SockAddrIn clientAddress;
    socklen_t clientLenght = sizeof(clientAddress);
    currConn.SOCKET = accept(this->SOCKET, (struct sockaddr *)&clientAddress, &clientLenght);
    if(currConn.SOCKET < 0){
        printf("Server failed to connect to client!\n");
        exit(0);
    }
    currConn.index = CURR_CLIENT_INDEX++;

    // attaching current connection
    this->clientConnections.push_back(currConn);
    printf("Connection with client %d stabilished.\n", currConn.index);

    return currConn.index;
}

void Server::disconnectClient(int index){
    this->clientConnections[index].index = DISCONNECTED;
    close(this->clientConnections[index].SOCKET);
}

int Server::_receive(Connection currConnection, char *message){
    int received_bytes = recv(currConnection.SOCKET, message, this->MAX_MSG_SIZE, 0);
    if(received_bytes < 0)
        printf("Failed to receive message from client %d!\n", currConnection.index);
    else
        printf("Received message: '%s' from client %d\n", message, currConnection.index);

    return received_bytes;
}

void Server::_send(char *message){
    for (Connection &connection : this->clientConnections){
        if(connection.index != DISCONNECTED){
            if(send(connection.SOCKET, message, strlen(message)+1, 0) < 0){
                printf("Failed to send message to client %d!\n", connection.index);
            }
        }
    }
}