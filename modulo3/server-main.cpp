#include "includes/server.hpp"
#include <atomic>

using namespace std;

#define PORT 5002
#define QUEUE_SIZE 1
#define MAX_MSG_SIZE 16
#define MAX_CLIENTS 3

Server server(PORT, MAX_MSG_SIZE);
vector<thread> clientThreads;
volatile bool running = true;

void cmd_kick(Channel* currChann, char *nickname){

    for (Connection &currConn : currChann->v_connections){
        if (strcmp(currConn.nickname, nickname) == 0){
            if (currConn.index != DISCONNECTED){
                server.disconnectClient(currConn.index);
            }
        }
    }
    return;
}

void cmd_mute(Channel *currChann, char *nickname){
    for (Connection &currConn : currChann->v_connections){
        if (strcmp(currConn.nickname, nickname) == 0){
            if (currConn.index != DISCONNECTED){
                server.muteClient(currConn.index);
            }
        }
    }
    return;
}

void exitSignalHandler(int signum) {
    printf("\nTo exit, type '/quit' in the terminal\n");
}

void serverClientCommunication(int index){
    bool clientOnline = true;
    char message[MAX_MSG_SIZE+1];

    Connection currConnection = server.clientConnections[index];
    while(clientOnline and running){
        // reseting message to receive new one
        memset(message, 0, sizeof(message));

        // receiving message from client
        int validMessage = server._receive(currConnection, message) < 0;
        // killing thread if client disconnects or any error happens while
        if(validMessage || strcmp(message, "/quit") == 0 || strlen(message) == 0){
            string exit_message = "Client " + to_string(currConnection.index) + " disconnected.";
            strcpy(message, exit_message.c_str());
            clientOnline = false;
            server.disconnectClient(currConnection.index);
        }

        Channel *currChann = server._search_channel(currConnection.channel_name);

        // '/ping' return
        if (strcmp(message, "/ping") == 0){
            server._reply((char*)"Server: pong\n", currConnection);
            continue;
        }

        // if current user is admin, checks commands
        if (strcmp(currChann->admin_nickname, currConnection.nickname) == 0){
            if (!strncmp(message, "/kick", 5)){
                printf("kick\n");
                cmd_kick(currChann, &message[6]);
                //server.disconnectClient(index);
            }
            if (!strncmp(message, "/mute", 5)){
                printf("mute\n");
                cmd_mute(currChann, &message[6]);
            }
            if (!strcmp(message, "/unmute")){
                printf("unmute\n");
            }
            if (!strcmp(message, "/whois")){
                printf("whois\n");
            }
        } 

        /* TODO
         *  Mostrar nickname personalizado */

        //sending message
        std::string str(message);
        std::string nick_str(currConnection.nickname);
        str = nick_str+": "+str+"\n";
        server._send(currConnection, str.data());
    }
}

void serverInput(){
    string input;

    while(running){
        getline(cin, input);
        printf("Server Command: %s\n", input.c_str());
        if(input.compare("/quit") == 0){
            running = false;
            close(server.SOCKET);
        }
    }

    return;
}

void serverListen(){
    while(running){
        server._listen();
        int clientIndex = server._accept();
        thread thrCommunication(serverClientCommunication, clientIndex);
        thrCommunication.detach();
    }
}

int main(){
    signal(SIGINT, exitSignalHandler);
    thread thrInput(serverInput);
    thread thrListen(serverListen);
    thrListen.detach();

    thrInput.join();
    return 0;
}
