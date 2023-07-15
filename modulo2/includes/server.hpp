#pragma once

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <unistd.h>
#include <csignal>

using namespace std;

#define DISCONNECTED -1
typedef struct sockaddr_in SockAddrIn;

typedef struct connection_t {
    int index;
    int SOCKET;
} Connection;

class Server {
    private:
        int SOCKET;
        SockAddrIn ADDRESS;
        int CURR_CLIENT_INDEX;
        int MAX_MSG_SIZE;
        
    public:
        vector<Connection> clientConnections;
        
        Server(int port, int max_msg_size);
        void _listen();
        int _accept();
        int _receive(Connection currConnection, char *message);
        void _send(char *message);

        void disconnectClient(int index);
};