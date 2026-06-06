#ifndef API_SERVER_H
#define API_SERVER_H

#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <functional>

#include "OrderManager.h"
#include "MenuManager.h"
#include "TableManager.h"
#include "CustomerManager.h"
#include "StatsManager.h"
#include "InventoryManager.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

struct HttpRequest {
    std::string method;
    std::string path;
    std::string body;
    std::string query;
    std::map<std::string, std::string> headers;
};

struct HttpResponse {
    int statusCode;
    std::string statusText;
    std::string body;
    std::string contentType;
    std::map<std::string, std::string> headers;
};

class ApiServer {
public:
    ApiServer(int port);
    ~ApiServer();

    void start();
    void stop();
    bool isRunning() const;

    void setMenuManager(MenuManager* manager);
    void setOrderManager(OrderManager* manager);
    void setTableManager(TableManager* manager);
    void setCustomerManager(CustomerManager* manager);
    void setStatsManager(StatsManager* manager);
    void setInventoryManager(InventoryManager* manager);

private:
    int port;
    int serverSocket;
    bool running;
    std::thread acceptThread;

    MenuManager* menuManager;
    OrderManager* orderManager;
    TableManager* tableManager;
    CustomerManager* customerManager;
    StatsManager* statsManager;
    InventoryManager* inventoryManager;

    std::mutex mutex;

    void acceptConnections();
    void handleClient(int clientSocket);
    HttpRequest parseRequest(const std::string& rawRequest);
    HttpResponse handleRequest(const HttpRequest& request);
    
    HttpResponse handleMenu(const std::string& path, const std::string& method, const std::string& body);
    HttpResponse handleOrders(const std::string& path, const std::string& method, const std::string& body);
    HttpResponse handleTables(const std::string& path, const std::string& method, const std::string& body);
    HttpResponse handleCustomers(const std::string& path, const std::string& method, const std::string& body);
    HttpResponse handleStats(const std::string& path, const std::string& method, const std::string& body);
    HttpResponse handleInventory(const std::string& path, const std::string& method, const std::string& body);

    std::string urlDecode(const std::string& str);
    std::string parseQueryParam(const std::string& query, const std::string& param);
    
    void sendResponse(int clientSocket, const HttpResponse& response);
};

#endif
