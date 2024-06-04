#include <iostream>     
#include "server.h"
#include <thread>

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
        } else {
            command+="\n";
            server.sendMessage(command);
        }
    }

    return 0;
}