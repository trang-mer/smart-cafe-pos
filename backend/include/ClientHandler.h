#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <string>

#ifdef _WIN32
#include <winsock2.h>
#else
typedef int SOCKET;
#endif

class Server;
class ClientRole;

class ClientHandler {
public:
    ClientHandler(SOCKET clientSocket, Server* server);
    void handle();

private:
    SOCKET clientSocket;
    Server* server;

    void processJsonMessage(const std::string& jsonText);
    void handleLogin(const nlohmann::json& message);
    void handleCreateOrder(const nlohmann::json& message, ClientRole senderRole);
    void handleUpdateStatus(const nlohmann::json& message, ClientRole senderRole);
    void handlePayment(const nlohmann::json& message, ClientRole senderRole);
    void handleCancelOrder(const nlohmann::json& message, ClientRole senderRole);
    void handleGetOrders(const nlohmann::json& message, ClientRole senderRole);
    void handleGetOrder(const nlohmann::json& message, ClientRole senderRole);
    void handleGetMenu(const nlohmann::json& message, ClientRole senderRole);
    void handleGetTables(const nlohmann::json& message, ClientRole senderRole);
    void handleUpdateTable(const nlohmann::json& message, ClientRole senderRole);
    void handleGetCustomers(const nlohmann::json& message, ClientRole senderRole);
    void handleSearchCustomers(const nlohmann::json& message, ClientRole senderRole);
    void handleCreateCustomer(const nlohmann::json& message, ClientRole senderRole);
    void handleGetStats(const nlohmann::json& message, ClientRole senderRole);
    void handleReport(const nlohmann::json& message, ClientRole senderRole);
    void handleBroadcast(const nlohmann::json& message, ClientRole senderRole);
};

#endif
