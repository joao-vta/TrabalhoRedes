#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <csignal>
#include <thread>
#include <vector>
#include <arpa/inet.h>

using namespace std;

typedef struct sockaddr_in SockAddrIn;

class Client {
    private:
        string nickname = "";
    public:
        int SOCKET;

        Client();
        void _connect(int serverPort, const char* serverIP);
        void _join(char channel_name[], int serverPort, const char* serverIP);
        void _send(string &message);
        void _receive(char *message, int size);

        void setNickname(string nickname);
};
