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

void cmd_unmute(Channel *currChann, char *nickname){
    for (Connection &currConn : currChann->v_connections){
        if (strcmp(currConn.nickname, nickname) == 0){
            if (currConn.index != DISCONNECTED){
                server.unmuteClient(currConn.index);
            }
        }
    }
    return;
}

std::string cmd_whois(Channel *currChann, char *nickname){
    printf("nickname: %s\n", nickname);
    std::string whois_reply("User not found.");
    for (Connection &currConn : currChann->v_connections){
        if (strcmp(currConn.nickname, nickname) == 0){
            if (currConn.index != DISCONNECTED){
                whois_reply = "IP from "+ std::string(nickname) + " => " + std::string(currConn.ipv4_address) + "\n";
                break;
            }
        }
    }
    printf("cmd_whois_reply: %s\n", whois_reply.data());

    return whois_reply;
}

bool change_nickname(Connection *currConn, char *nickname){

    if(strlen(nickname) > 50){
        cout << "Nickname must be shorter than 50 characters." << endl;
        return false;
    }

    strcpy(currConn->nickname, nickname);
    return true;
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

        // if cmd is join, user must switch channels
        if (strncmp(message, "/join ", 6) == 0){
            server._changeConnChannel(&currConnection, &message[6]);
            continue;
        }

        //std::string srcNickname(currConnection.nickname);
        if (server._isMuted(currConnection)){
            continue;
        }
        
        if (strncmp(message, "/nickname ", 10) == 0){

            // if current user is admin, channel must be updated
            bool is_admin = false;
            if (strcmp(currChann->admin_nickname, currConnection.nickname) == 0){
                is_admin = true;
            }

            /* TODO
             * alterar o nome no canal apenas depois de checar se a alteração
             * vai ser de fato realizada
            */
            // searches for currConn nickname in channel's connections list
            for (int i=0; i < (int)currChann->v_connections.size(); i++){
                if (strcmp(currChann->v_connections[i].nickname, currConnection.nickname) == 0){
                    strcpy(currChann->v_connections[i].nickname, &message[10]);
                }
            }

            if (change_nickname(&currConnection, &message[10]) == false){
                continue;
            }

                
            if (is_admin == true){
                strcpy(currChann->admin_nickname, currConnection.nickname);
            }
            server.clientConnections[index] = currConnection;

            continue;
        }

        // '/ping' return
        if (strcmp(message, "/ping") == 0){
            server._reply((char*)"Server: pong\n", currConnection);
            continue;
        }

        // if current user is admin, checks commands
        std::string nick_str(currConnection.nickname);
        if (strcmp(currChann->admin_nickname, currConnection.nickname) == 0){

            // adds '@' to admin's name
            nick_str = "@"+nick_str;

            if (!strncmp(message, "/kick ", 6)){
                printf("kick\n");
                cmd_kick(currChann, &message[6]);
            }
            if (!strncmp(message, "/mute ", 6)){
                printf("mute\n");
                cmd_mute(currChann, &message[6]);
            }
            if (!strncmp(message, "/unmute ", 8)){
                printf("unmute\n");
                cmd_unmute(currChann, &message[8]);
            }
            if (!strncmp(message, "/whois ", 7)){
                printf("whois\n");
                std::string whois_reply = cmd_whois(currChann, &message[7]);
                printf("whois_reply: %s\n", whois_reply.data());
                server._reply(whois_reply.data(), currConnection);
            
                continue;
            }
        } 

        //sending message
        std::string str_message(message);
        str_message = nick_str+": "+str_message+"\n";
        server._send(currConnection, str_message.data());
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
