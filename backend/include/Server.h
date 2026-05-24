#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <mutex>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#else
typedef int SOCKET;
#endif

enum class ClientRole {
    UNKNOWN,
    CASHIER,
    KITCHEN,
    MANAGER
};

struct ClientInfo {
    SOCKET socket;
    ClientRole role;
};

class Server {
public:
    explicit Server(int port);

    bool start();
    void run();
    void stop();

    void addClient(SOCKET clientSocket);
    void removeClient(SOCKET clientSocket);

    void setClientRole(SOCKET clientSocket, ClientRole role);
    ClientRole getClientRole(SOCKET clientSocket);

    void sendToClient(SOCKET clientSocket, const std::string& message);
    void sendToRole(ClientRole role, const std::string& message);
    void sendToRoles(const std::vector<ClientRole>& roles, const std::string& message);

    static ClientRole parseRole(const std::string& roleText);
    static std::string roleToString(ClientRole role);

private:
    int port;
    SOCKET serverSocket;
    bool running;

    std::vector<ClientInfo> clients;
    std::mutex clientsMutex;
};

#endif