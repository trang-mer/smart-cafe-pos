#ifndef SERVER_H
#define SERVER_H

#ifdef _WIN32
#include <winsock2.h>
#else
typedef int SOCKET;
#endif

class Server {
public:
    explicit Server(int port);

    bool start();
    void run();
    void stop();

private:
    int port;
    SOCKET serverSocket;
    bool running;
};

#endif