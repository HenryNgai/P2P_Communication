#include <iostream>
#include <cstring>
#include <arpa/inet.h> // For socket functions and structures
#include <unistd.h>    // For close function
#include <csignal>     // For signal handling
#include <thread>
#include "server.h"

// Global pointer to the server instance for signal handling
Server* serverInstance = nullptr;

Server::Server(int port) : PORT(port), serverSocket(-1){
    serverInstance = this;
};

bool Server::startServer(){
    serverSocket = socket(AF_INET, SOCK_STREAM, 0); // 2 way TCP Connection
    if (serverSocket == -1){
        std::cout << "Failed to create socket\n";
        return false;
    }

    // Populate sockaddr_in struct 
    sockaddr_in serverAddress; 
    serverAddress.sin_family = AF_INET; // IPV4 protocol
    serverAddress.sin_port = htons(PORT); // Port
    serverAddress.sin_addr.s_addr = INADDR_ANY; // Accept connections from any ip on this machine
    memset(serverAddress.sin_zero, '\0', sizeof(serverAddress.sin_zero)); // Safety - initilaize rest of struct 

    int bind_result = bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)); // Bind 
    if (bind_result == -1){
        std::cout << "Failed to bind address to port"<< std::endl;
        return false;
    }

    int listen_result = listen(serverSocket, 5); // Limit queue of 5 pending connection requests
    if (listen_result == -1){
        std::cout << "Failed to listen on binded socket"<< std::endl;
        return false;
    }
    sockaddr_in localAddress;
    socklen_t addressLength = sizeof(localAddress);
    if (getsockname(serverSocket, (struct sockaddr*)&localAddress, &addressLength) == -1) { // Get address that socket is bound to
        std::cout<<"getsockname failed, please check to ensure server is running" << std::endl;
    }
    else{
        char ip [INET_ADDRSTRLEN]; // char array with length INET_ADDRSTRLEN
        inet_ntop(AF_INET, &localAddress.sin_addr, ip, sizeof(ip)); // Convert to human readable text
        std::cout << "Server running on IP: " << ip << " Port: " << ntohs(localAddress.sin_port) << std::endl;
    }

    // Set up the signal handler for graceful shutdown
    std::signal(SIGINT, handleSignal);

    while(true){
        sockaddr_in clientAddress;
        socklen_t clientAddressLen = sizeof(clientAddress);
        
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLen);
        std::lock_guard<std::mutex> lock(clientSocketsMutex); // Lock 
        clientSockets.push_back(clientSocket); // Add to clientSockets vector

        // Convert the client's IP address and port to a readable format
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIP, INET_ADDRSTRLEN);
        std::cout<< "New Connection from IP:" << clientAddress.sin_addr.s_addr << " PORT " << clientAddress.sin_port << std::endl;

        std::thread clientThread (&Server::handleClient,this, clientSocket);
        clientThread.detach();        
    }
    return true;
}

// Static signal handler function
void Server::handleSignal(int signal) {
    if (serverInstance) {
        serverInstance->shutdownServer();
    }
}

// Member function for handling shutdown
void Server::shutdownServer() {
    std::cout << "Received signal, shutting down server." << std::endl;
    close(serverSocket);
    exit(EXIT_SUCCESS);
}

// Process incoming connection
void Server::handleClient(int clientSocket){
    char buffer[1024];
    while(true){
        memset(buffer, 0, sizeof(buffer)); // Zero out buffer
        int bytesRead = read(clientSocket, buffer, sizeof(buffer));\
        if (bytesRead < 0){
            std::cout<<"Error reading from client socket"<<std::endl;
            close(clientSocket);
            std::lock_guard<std::mutex> lock(clientSocketsMutex); //Lock clientSockets vector
            // Fancy hack to remove specific clientSocket from clientSocket Vector
            clientSockets.erase(std::remove(clientSockets.begin(), clientSockets.end(), clientSocket), clientSockets.end()); 
            return;
        }
        else if (bytesRead == 0){
            std::cout<<"Client disconnected"<<std::endl;
            close(clientSocket);
            std::lock_guard<std::mutex> lock(clientSocketsMutex); //Lock clientSockets vector
            // Fancy hack to remove specific clientSocket from clientSocket Vector
            clientSockets.erase(std::remove(clientSockets.begin(), clientSockets.end(), clientSocket), clientSockets.end()); 
            return;
        }
        else{
            std::cout<< "Received Message: " << buffer <<std::endl;
        }
    }
    close(clientSocket);
}


// Send messages
void Server::sendMessage(std::string &message){ 
    std::lock_guard<std::mutex> lock(clientSocketsMutex); // Lock
    for (int clientSocket : clientSockets){
        send(clientSocket, message.c_str(), message.size(), 0);
    }
}