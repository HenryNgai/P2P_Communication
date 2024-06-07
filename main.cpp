#include <iostream>     
#include "server.h"
#include <thread>
#include <vector>
#include <string>

int main() {
    Server server(8080);
    std::thread serverThread(&Server::startServer, &server);
    serverThread.detach();

    std::string command;
    while (true) {
        std::getline(std::cin, command);
        if (command == "exit") {
            server.shutdownServer();
            break;
        }
        else if(command == "select client"){
            std::cout<< "Please choose a client" << std::endl;
            std::vector<int> connectedClients = server.getConnectedClients();
            for (int client : connectedClients){
                std::cout<<client<<std::endl;
            }
            std::string selectedClient;
            std::getline(std::cin, selectedClient);
            server.setClient(std::stoi(selectedClient));
        }
        else {
            command+="\n";
            server.sendMessage(command);
        }
    }

    return 0;
}