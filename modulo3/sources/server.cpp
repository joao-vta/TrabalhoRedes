#include "../includes/server.hpp"

#include <algorithm>
#include <arpa/inet.h>

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

Connection* Server::_search_connection(char nickname[]){
    for (Connection &C : this->clientConnections){
        if (!strcmp(C.nickname, nickname)){
            return &C;
        }
    }
    return NULL;
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
    
    // getting client ip address
    inet_ntop(AF_INET, &clientAddress.sin_addr, currConn.ipv4_address, sizeof(currConn.ipv4_address));
    printf("addr: %s\n", currConn.ipv4_address); 

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
        
        strcpy(currConn.nickname, nickname);

        currChann = (Channel*) malloc(sizeof(Channel));
        strcpy(currChann->name, channel_name);
        strcpy(currChann->admin_nickname, currConn.nickname);
    }

    currChann->v_connections.push_back(currConn);
    this->v_channels.push_back((*currChann));

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

bool Server::_isMuted(Connection currConn){

    Channel *currChann = _search_channel(currConn.channel_name);
    if (currChann == NULL) return false;

    std::vector<std::string> v_muted = currChann->v_muted;
    if(find(v_muted.begin(), v_muted.end(), currConn.nickname) != v_muted.end()){
        return true;
    }
    return false;
}

void Server::_send(Connection srcConn, char *message){

    Channel *currChann = NULL;
    for (Channel &C : this->v_channels){
        if (!strcmp(srcConn.channel_name, C.name)){
            currChann = &C;
            break;
        }
    }

    string srcNickname = srcConn.nickname;

    std::vector<std::string> v_muted = currChann->v_muted;
    if(find(v_muted.begin(), v_muted.end(), srcNickname) != v_muted.end()){
        return;
    }

    for (Connection &connection : currChann->v_connections){
        this->_reply(message, connection);
    }
    return;
}

void Server::_changeConnChannel(Connection *currConn, char new_channel_name[]){

    Channel *currChann = _search_channel(currConn->channel_name);
    if (currChann == NULL) return;

    // erases nickname from current channel's list
    for (int i=0; i < (int)currChann->v_connections.size(); i++){
        if (!strcmp(currChann->v_connections[i].nickname, currConn->nickname)){
            currChann->v_connections.erase(currChann->v_connections.begin()+i);
            break;
        }
    }

    Channel *newChann = _search_channel(new_channel_name);
    if (newChann == NULL){
        newChann = (Channel*) malloc(sizeof(Channel));
        strcpy(newChann->name, new_channel_name);
        strcpy(newChann->admin_nickname, currConn->nickname);

        newChann->v_connections.push_back((*currConn));
        this->v_channels.push_back((*newChann));
        printf("creates new channel!\n");
    }

    newChann->v_connections.push_back((*currConn));
    strcpy(currConn->channel_name, new_channel_name);

    return;
}

void Server::muteClient(int index){

    Connection currConn = this->clientConnections[index];

    Channel *currChann = _search_channel(currConn.channel_name);
    if (currChann == NULL) return;

    // if client is not already in the muted list
    std::vector<std::string> v_muted = currChann->v_muted;
    if(find(v_muted.begin(), v_muted.end(), this->clientConnections[index].nickname) == v_muted.end()){
        currChann->v_muted.push_back(this->clientConnections[index].nickname);
    }
    return;
}

void Server::unmuteClient(int index){
    Connection currConn = this->clientConnections[index];

    Channel *currChann = _search_channel(currConn.channel_name);
    if (currChann == NULL) return;
    
    std::vector<std::string> v_muted = currChann->v_muted;
    if(find(v_muted.begin(), v_muted.end(), this->clientConnections[index].nickname) != v_muted.end()){
        currChann->v_muted.erase(v_muted.begin() + index);
    }
    return;
}

