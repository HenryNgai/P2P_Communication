#include <iostream>     
#include "server.h"

int main() {
    Server server(8080);
    server.startServer();
}