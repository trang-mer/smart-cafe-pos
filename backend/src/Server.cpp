#include "Server.h"
#include "ClientHandler.h"
#include "Logger.h"

#include <thread>
#include <string>
#include <algorithm>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

Server::Server(int port)
    : port(port), serverSocket(-1), running(false) {}

Server::~Server() {
    stop();
}

bool Server::start() {
#ifdef _WIN32
    WSADATA wsaData;
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaResult != 0) {
        Logger::error("WSAStartup failed");
        return false;
    }
#endif

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        Logger::error("Failed to create socket");
        return false;
    }

    int opt = 1;
#ifdef _WIN32
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    int bindResult = bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress));
    if (bindResult == -1) {
        Logger::error("Bind failed");
        stop();
        return false;
    }

    int listenResult = listen(serverSocket, SOMAXCONN);
    if (listenResult == -1) {
        Logger::error("Listen failed");
        stop();
        return false;
    }

    running = true;
    Logger::info("Server started on port " + std::to_string(port));

    return true;
}

void Server::run() {
    while (running) {
        sockaddr_in clientAddress{};
        int clientSize = sizeof(clientAddress);

        SOCKET clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddress), &clientSize);

        if (clientSocket == -1) {
            if (running) {
                Logger::error("Accept failed");
            }
            continue;
        }

        addClient(clientSocket);

        std::thread clientThread([this, clientSocket]() {
            ClientHandler handler(clientSocket, this);
            handler.handle();
        });

        clientThread.detach();
    }
}

void Server::addClient(SOCKET clientSocket) {
    std::lock_guard<std::mutex> lock(clientsMutex);

    ClientInfo clientInfo;
    clientInfo.socket = clientSocket;
    clientInfo.role = ClientRole::UNKNOWN;
    clientInfo.username = "";

    clients.push_back(clientInfo);

    Logger::info("New client connected. Total clients: " + std::to_string(clients.size()));
}

void Server::removeClient(SOCKET clientSocket) {
    std::lock_guard<std::mutex> lock(clientsMutex);

    auto it = std::remove_if(clients.begin(), clients.end(), [clientSocket](const ClientInfo& client) {
        return client.socket == clientSocket;
    });

    if (it != clients.end()) {
        clients.erase(it);
    }

    Logger::info("Client removed. Total clients: " + std::to_string(clients.size()));
}

void Server::setClientRole(SOCKET clientSocket, ClientRole role) {
    std::lock_guard<std::mutex> lock(clientsMutex);

    for (auto& client : clients) {
        if (client.socket == clientSocket) {
            client.role = role;
            Logger::info("Client role set to " + roleToString(role));
            return;
        }
    }
}

void Server::setClientUsername(SOCKET clientSocket, const std::string& username) {
    std::lock_guard<std::mutex> lock(clientsMutex);

    for (auto& client : clients) {
        if (client.socket == clientSocket) {
            client.username = username;
            return;
        }
    }
}

ClientRole Server::getClientRole(SOCKET clientSocket) {
    std::lock_guard<std::mutex> lock(clientsMutex);

    for (const auto& client : clients) {
        if (client.socket == clientSocket) {
            return client.role;
        }
    }

    return ClientRole::UNKNOWN;
}

std::string Server::getClientUsername(SOCKET clientSocket) {
    std::lock_guard<std::mutex> lock(clientsMutex);

    for (const auto& client : clients) {
        if (client.socket == clientSocket) {
            return client.username;
        }
    }

    return "";
}

void Server::sendToClient(SOCKET clientSocket, const std::string& message) {
    std::string data = message + "\n";
    send(clientSocket, data.c_str(), data.size(), 0);
}

void Server::sendToRole(ClientRole role, const std::string& message) {
    std::lock_guard<std::mutex> lock(clientsMutex);

    std::string data = message + "\n";

    for (const auto& client : clients) {
        if (client.role == role) {
            send(client.socket, data.c_str(), data.size(), 0);
        }
    }
}

void Server::sendToRoles(const std::vector<ClientRole>& roles, const std::string& message) {
    std::lock_guard<std::mutex> lock(clientsMutex);

    std::string data = message + "\n";

    for (const auto& client : clients) {
        for (ClientRole role : roles) {
            if (client.role == role) {
                send(client.socket, data.c_str(), data.size(), 0);
                break;
            }
        }
    }
}

void Server::broadcastMessage(const std::string& message) {
    sendToRoles({ClientRole::CASHIER, ClientRole::KITCHEN, ClientRole::MANAGER}, message);
}

void Server::broadcastMessageExcept(SOCKET excludeSocket, const std::string& message) {
    std::lock_guard<std::mutex> lock(clientsMutex);

    std::string data = message + "\n";

    for (const auto& client : clients) {
        if (client.socket != excludeSocket) {
            send(client.socket, data.c_str(), data.size(), 0);
        }
    }
}

OrderManager& Server::getOrderManager() {
    return orderManager;
}

MenuManager& Server::getMenuManager() {
    return menuManager;
}

TableManager& Server::getTableManager() {
    return tableManager;
}

CustomerManager& Server::getCustomerManager() {
    return customerManager;
}

StatsManager& Server::getStatsManager() {
    return statsManager;
}

ClientRole Server::parseRole(const std::string& roleText) {
    if (roleText == "CASHIER") return ClientRole::CASHIER;
    if (roleText == "KITCHEN") return ClientRole::KITCHEN;
    if (roleText == "MANAGER") return ClientRole::MANAGER;

    return ClientRole::UNKNOWN;
}

std::string Server::roleToString(ClientRole role) {
    switch (role) {
        case ClientRole::CASHIER:
            return "CASHIER";
        case ClientRole::KITCHEN:
            return "KITCHEN";
        case ClientRole::MANAGER:
            return "MANAGER";
        default:
            return "UNKNOWN";
    }
}

void Server::stop() {
    running = false;

#ifdef _WIN32
    if (serverSocket != INVALID_SOCKET && serverSocket != -1) {
        closesocket(serverSocket);
    }
    WSACleanup();
#else
    if (serverSocket != -1) {
        close(serverSocket);
    }
#endif

    Logger::info("Server stopped");
}
