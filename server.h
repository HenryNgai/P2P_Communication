#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h> // For sockaddr_in
#include <csignal>      // For signal handling

class Server{
    private:
        int PORT;
        int serverSocket;

    public:
        Server(int port = 8080); // Default port
        bool startServer();
        void shutdownServer();
        void handleClient(int clientSocket);
        static void handleSignal(int signal);
};

#endif // SERVER_H