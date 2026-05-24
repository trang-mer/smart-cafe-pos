#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#ifdef _WIN32
#include <winsock2.h>
#else
typedef int SOCKET;
#endif

class ClientHandler {
public:
    explicit ClientHandler(SOCKET clientSocket);
    void handle();

private:
    SOCKET clientSocket;
};

#endif