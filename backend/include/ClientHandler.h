#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <string>

#ifdef _WIN32
#include <winsock2.h>
#else
typedef int SOCKET;
#endif

class Server;

class ClientHandler {
public:
    ClientHandler(SOCKET clientSocket, Server* server);
    void handle();

private:
    SOCKET clientSocket;
    Server* server;

    void processJsonMessage(const std::string& jsonText);
};

#endif