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
    char channel_name[200];
    int SOCKET;
    char nickname[50];
} Connection;

typedef struct channel_ {
    char name[200];
    char admin_nickname[50];
    vector<Connection> v_connections;
} Channel;

class Server {
    private:
        int SOCKET;
        SockAddrIn ADDRESS;
        int CURR_CLIENT_INDEX;
        int MAX_MSG_SIZE;
        
    public:
        vector<Connection> clientConnections;
        vector<Channel> v_channels;
        
        Server(int port, int max_msg_size);
        void _listen();
        int _accept();
        Channel* _search_channel(char name[]);
        int _receive(Connection currConnection, char *message);
        void _reply(char *message, Connection connection);
        void _send(Connection srcConn, char *message);

        void disconnectClient(int index);
};
