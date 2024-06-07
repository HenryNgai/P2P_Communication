#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h> // For sockaddr_in
#include <csignal>      // For signal handling
#include <mutex>
#include <vector>

class Server{
    private:
        int PORT;
        int serverSocket;
        std::vector<int>clientSockets;
        std::mutex clientSocketsMutex;
        int currentClientSocket;

    public:
        Server(int port = 8080); // Default port
        bool startServer();
        void shutdownServer();
        void handleClient(int clientSocket);
        void sendMessage(std::string &message);
        static void handleSignal(int signal);
        std::vector<int> getConnectedClients();
        bool setClient(int clientSocket);
        
};

#endif // SERVER_H