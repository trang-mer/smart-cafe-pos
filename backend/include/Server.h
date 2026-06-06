#ifndef SERVER_H
#define SERVER_H

#include "OrderManager.h"
#include "MenuManager.h"
#include "TableManager.h"
#include "CustomerManager.h"
#include "StatsManager.h"

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
    std::string username;
};

class Server {
public:
    explicit Server(int port);
    ~Server();

    bool start();
    void run();
    void stop();

    void addClient(SOCKET clientSocket);
    void removeClient(SOCKET clientSocket);

    void setClientRole(SOCKET clientSocket, ClientRole role);
    void setClientUsername(SOCKET clientSocket, const std::string& username);
    ClientRole getClientRole(SOCKET clientSocket);
    std::string getClientUsername(SOCKET clientSocket);

    void sendToClient(SOCKET clientSocket, const std::string& message);
    void sendToRole(ClientRole role, const std::string& message);
    void sendToRoles(const std::vector<ClientRole>& roles, const std::string& message);
    void broadcastMessage(const std::string& message);
    void broadcastMessageExcept(SOCKET excludeSocket, const std::string& message);

    OrderManager& getOrderManager();
    MenuManager& getMenuManager();
    TableManager& getTableManager();
    CustomerManager& getCustomerManager();
    StatsManager& getStatsManager();

    static ClientRole parseRole(const std::string& roleText);
    static std::string roleToString(ClientRole role);

private:
    int port;
    SOCKET serverSocket;
    bool running;

    std::vector<ClientInfo> clients;
    std::mutex clientsMutex;

    OrderManager orderManager;
    MenuManager menuManager;
    TableManager tableManager;
    CustomerManager customerManager;
    StatsManager statsManager;
};

#endif
