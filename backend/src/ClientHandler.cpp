#include "ClientHandler.h"
#include "Server.h"
#include "Logger.h"

#include <string>
#include <cstring>
#include <sstream>

#include "nlohmann/json.hpp"

#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#endif

using json = nlohmann::json;

ClientHandler::ClientHandler(SOCKET clientSocket, Server* server)
    : clientSocket(clientSocket), server(server) {}

void ClientHandler::handle() {
    char buffer[1024];
    std::string pendingData;

    Logger::info("Client handler started");

    json welcome = {
        {"type", "WELCOME"},
        {"message", "Welcome to Smart Cafe POS Server. Please set role first."},
        {"example", {{"type", "ROLE"}, {"role", "CASHIER"}}}
    };

    server->sendToClient(clientSocket, welcome.dump());

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0) {
            Logger::info("Client disconnected");
            break;
        }

        pendingData.append(buffer, bytesReceived);

        size_t pos;
        while ((pos = pendingData.find('\n')) != std::string::npos) {
            std::string jsonText = pendingData.substr(0, pos);
            pendingData.erase(0, pos + 1);

            if (!jsonText.empty()) {
                Logger::info("Received JSON: " + jsonText);
                processJsonMessage(jsonText);
            }
        }
    }

    server->removeClient(clientSocket);

#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
}

void ClientHandler::processJsonMessage(const std::string& jsonText) {
    json message;

    try {
        message = json::parse(jsonText);
    } catch (const std::exception& e) {
        json errorResponse = {
            {"type", "ERROR"},
            {"message", "Invalid JSON format"}
        };

        server->sendToClient(clientSocket, errorResponse.dump());
        return;
    }

    std::string type = message.value("type", "");

    if (type == "ROLE") {
        std::string roleText = message.value("role", "");
        ClientRole role = Server::parseRole(roleText);

        if (role == ClientRole::UNKNOWN) {
            json errorResponse = {
                {"type", "ERROR"},
                {"message", "Invalid role. Use CASHIER, KITCHEN, or MANAGER."}
            };

            server->sendToClient(clientSocket, errorResponse.dump());
            return;
        }

        server->setClientRole(clientSocket, role);

        json response = {
            {"type", "ROLE_ACCEPTED"},
            {"role", Server::roleToString(role)}
        };

        server->sendToClient(clientSocket, response.dump());
        return;
    }

    ClientRole senderRole = server->getClientRole(clientSocket);

    if (senderRole == ClientRole::UNKNOWN) {
        json errorResponse = {
            {"type", "ERROR"},
            {"message", "Please set role first."},
            {"example", {{"type", "ROLE"}, {"role", "CASHIER"}}}
        };

        server->sendToClient(clientSocket, errorResponse.dump());
        return;
    }

    if (type == "ORDER") {
        if (senderRole != ClientRole::CASHIER) {
            json errorResponse = {
                {"type", "ERROR"},
                {"message", "Only CASHIER can create orders."}
            };

            server->sendToClient(clientSocket, errorResponse.dump());
            return;
        }

        json notification = {
            {"type", "ORDER_CREATED"},
            {"from", "CASHIER"},
            {"table", message.value("table", 0)},
            {"items", message.value("items", json::array())},
            {"note", message.value("note", "")}
        };

        server->sendToRoles(
            {ClientRole::KITCHEN, ClientRole::MANAGER},
            notification.dump()
        );

        json response = {
            {"type", "ORDER_SENT"},
            {"message", "Order sent to kitchen and manager."}
        };

        server->sendToClient(clientSocket, response.dump());
        return;
    }

    if (type == "STATUS") {
        if (senderRole != ClientRole::KITCHEN) {
            json errorResponse = {
                {"type", "ERROR"},
                {"message", "Only KITCHEN can update order status."}
            };

            server->sendToClient(clientSocket, errorResponse.dump());
            return;
        }

        json notification = {
            {"type", "ORDER_STATUS_UPDATED"},
            {"from", "KITCHEN"},
            {"orderId", message.value("orderId", 0)},
            {"status", message.value("status", "unknown")}
        };

        server->sendToRoles(
            {ClientRole::CASHIER, ClientRole::MANAGER},
            notification.dump()
        );

        json response = {
            {"type", "STATUS_SENT"},
            {"message", "Status sent to cashier and manager."}
        };

        server->sendToClient(clientSocket, response.dump());
        return;
    }

    if (type == "REPORT") {
        if (senderRole != ClientRole::MANAGER) {
            json errorResponse = {
                {"type", "ERROR"},
                {"message", "Only MANAGER can send report messages."}
            };

            server->sendToClient(clientSocket, errorResponse.dump());
            return;
        }

        json notification = {
            {"type", "MANAGER_REPORT"},
            {"from", "MANAGER"},
            {"message", message.value("message", "")}
        };

        server->sendToRoles(
            {ClientRole::CASHIER, ClientRole::KITCHEN},
            notification.dump()
        );

        json response = {
            {"type", "REPORT_SENT"},
            {"message", "Report sent."}
        };

        server->sendToClient(clientSocket, response.dump());
        return;
    }

    json errorResponse = {
        {"type", "ERROR"},
        {"message", "Unknown message type."},
        {"allowedTypes", {"ROLE", "ORDER", "STATUS", "REPORT"}}
    };

    server->sendToClient(clientSocket, errorResponse.dump());
}