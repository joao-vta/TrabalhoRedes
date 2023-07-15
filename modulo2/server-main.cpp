#include "includes/server.hpp"

using namespace std;

#define PORT 5001
#define QUEUE_SIZE 1
#define MAX_MSG_SIZE 4096
#define MAX_CLIENTS 3

Server server(PORT, MAX_MSG_SIZE);
vector<thread> clientThreads;
volatile bool running = true;


void exitSignalHandler(int signum) {
    printf("\nTo exit, type '.exit' in the terminal\n");
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
        if(validMessage || strcmp(message, "exit") == 0 || strlen(message) == 0){
            string exit_message = "Client " + to_string(currConnection.index) + " disconnected.";
            strcpy(message, exit_message.c_str());
            clientOnline = false;
            server.disconnectClient(currConnection.index);
        }

        //sending message
        server._send(message);
    }
}

void serverInput(){
    string input;

    while(running){
        getline(cin, input);
        printf("Server Command: %s\n", input.c_str());
        if(input.compare(".exit") == 0){
            running = false;
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