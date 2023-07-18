#include "includes/client.hpp"
#include <iostream>
#include <string.h>

using namespace std;

#define SERVER_PORT 5002
#define SERVER_IP "127.0.0.1"
#define MAX_MSG_SIZE 16

Client client;
volatile bool running = true;

void exitSignalHandler(int signum){
    printf("\nTo exit, type '/quit' in the terminal\n");
}

void sigpipeHandler(int signum){
    printf("Server disconnected. Closing client.\n");
    close(client.SOCKET);
    exit(1);
}

void send_message(){
    string message;
    while(running){
        // getting input
        getline(cin, message);
        // send it to client
        client._send(message);

        // checking if client wants to quit
        if(strcmp(message.c_str(), "/quit") == 0){
            running = false;
            close(client.SOCKET);
        }
    }

    return;
}

void receive_message(){
    char message[MAX_MSG_SIZE+1];
    while(running){
        memset(message, 0, sizeof(message));
        client._receive(message, MAX_MSG_SIZE);
        printf("%s", message);
    }

    return;
}

void set_nickname(){
    char nickname[50];
    bool notValid = true;
    
    while(notValid){
        memset(nickname, 0, 50);
        cout << "Type your nickname: ";
        cin >> nickname;
        if(strlen(nickname) < 50)
            notValid = false;
        else
            cout << "Nickname must be shorter than 50 characters." << endl;
    }

    client.setNickname(nickname);
    return;
}

bool check_channel_name(char name[]){
    if(!(name[0] == '#' || name[0] == '&')){
        cout << "Channel name must begin with either '#' or '&'!" << endl;
        return false;
    }

    if(!(strchr(name, ',') == NULL && strchr(name, ' ') == NULL)){
        cout << "Channel name can't contain spaces or commas!" << endl;
        return false;
    }
    
    return true;
}

void starting_menu(){
    std::string option;
    while(running){

        // input menu
        cout << "Options:"
                << "\n\t/join: connect to a channel\n" 
                << "\t/nickname to set a custom nickname\n" 
                << "\t/quit to exit\n";
        getline(cin, option);

        if (option.compare("/join") == 0){
            char channel_name[200];
            memset(channel_name, 0, sizeof(channel_name));
            cout << "Insira o nome do canal: ";
            cin >> channel_name;
            if(check_channel_name(channel_name)){
                client._join(channel_name, SERVER_PORT, SERVER_IP);
                break;
            }
        }
        else if (strncmp(option.data(), "/join ", 6) == 0){
            char channel_name[200];
            memset(channel_name, 0, sizeof(channel_name));

            strcpy(channel_name, &option.data()[6]);
            if(check_channel_name(channel_name)){
                client._join(channel_name, SERVER_PORT, SERVER_IP);
                break;
            }
        }
        else if (option.compare("/nickname") == 0){
            set_nickname();
        }
        else if (option.compare("/quit") == 0){
            exit(1);
        }
    }
    return;
}

int main(){
    //initializing signal handlers
    signal(SIGPIPE, sigpipeHandler);
    signal(SIGINT, exitSignalHandler);

    //asking for nickname on start
    set_nickname();
    getchar();

    //starting menu
    starting_menu();

    // initializing threads
    thread thSend(send_message);
    thread thReceive(receive_message);

    thSend.join();
    thReceive.join();

    return 0;
}
