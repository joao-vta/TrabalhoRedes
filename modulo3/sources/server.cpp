#include "../includes/server.hpp"

#include <algorithm>

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

Channel* Server::_search_channel(char name[]){

    for (Channel &C : this->v_channels){
        if (!strcmp(C.name, name)){
            return &C;
        }
    }

    return NULL;
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

    // receiving connection nickname
    char nickname[50];
    memset(nickname, 0, 50);
    if (recv(currConn.SOCKET, nickname, 50, 0) < 0){
        printf("Failed to receive channel name from client!\n");
    }
    else{
        printf("Nickname = %s\n", nickname);
    }
    strcpy(currConn.nickname, nickname);

    // receiving channel name
    char channel_name[200];
    memset(channel_name, 0, 200);
    if (recv(currConn.SOCKET, channel_name, 200, 0) < 0){
        printf("Failed to receive channel name from client!\n");
    }
    else{
        printf("channel = %s\n", channel_name);
        printf("Connection with client %s stabilished.\n", currConn.nickname);
    }
    strcpy(currConn.channel_name, channel_name);

    Channel *currChann = _search_channel(channel_name);
    if (currChann == NULL){
        currChann = (Channel*) malloc(sizeof(Channel));
        strcpy(currChann->name, channel_name);
        strcpy(currChann->admin_nickname, currConn.nickname);
    }

    currChann->v_connections.push_back(currConn);
    this->v_channels.push_back((*currChann));
    printf("_accept) channelname: %s\n", currConn.channel_name);

    // attaching current connection
    this->clientConnections.push_back(currConn);

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


void Server::_reply(char *message, Connection connection){

    if(connection.index != DISCONNECTED){
        int attempt;
        for(attempt = 0; attempt < 5; attempt++){
            if(send(connection.SOCKET, message, strlen(message), 0) < 0){
                printf("Failed to send message to client %d (try %d)!\n", connection.index, attempt+1);
            }
            else{
                break;
            }
        }
        if(attempt > 4){
            disconnectClient(connection.index);
        }
    }
    return;
}

void Server::_send(Connection srcConn, char *message){

    printf("_send) channelname: %s\n", srcConn.channel_name);

    Channel *currChann = NULL;
    for (Channel &C : this->v_channels){
        if (!strcmp(srcConn.channel_name, C.name)){
            currChann = &C;
            break;
        }
    }

    string srcNickname = srcConn.nickname;
    if(find(v_muted.begin(), v_muted.end(), srcNickname) != v_muted.end()){
        return;
    }

    for (Connection &connection : currChann->v_connections){
        this->_reply(message, connection);
    }
    return;
}

void Server::muteClient(int index){
    this->v_muted.push_back(this->clientConnections[index].nickname);
    return;
}
