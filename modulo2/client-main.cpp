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
        /* TODO
         *  print ficará neste formato mesmo?
        */
        //printf("Received reply: %s\n", message);
        printf("%s", message);
    }

    return;
}

/* TODO
 *  Reavaliar se é bom manter para o módulo 2 */
void get_nickname(){
    cout << "Digite seu nickname: ";
    string nickname;
    cin >> nickname;

    client.setNickname(nickname);

    return;
}

void starting_menu(){
    std::string option;
    while(running){

        // input menu
        cout << "Options:\n\t/connect: connect to the server\n" 
                //<< "\t/nickname to set a custom nickname\n" 
                << "\t/quit to exit\n";
        cin >> option;

        // command options
        if (option.compare("/connect") == 0){
            client._connect(SERVER_PORT, SERVER_IP);
            break;
        }
        else if (option.compare("/quit") == 0){
            exit(1);
        }
        /* TODO
         *  Reavaliar se é bom manter para o módulo 2
        else if (option.compare("/nickname") == 0){
            get_nickname();
        }
        */
    }
    return;
}

int main(){
    //initializing signal handlers
    signal(SIGPIPE, sigpipeHandler);
    signal(SIGINT, exitSignalHandler);

    //starting menu
    starting_menu();

    // initializing threads
    thread thSend(send_message);
    thread thReceive(receive_message);

    thSend.join();
    thReceive.join();

    return 0;
}
