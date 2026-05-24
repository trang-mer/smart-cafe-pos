#include "ClientHandler.h"
#include "Logger.h"

#include <iostream>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#endif

ClientHandler::ClientHandler(SOCKET clientSocket)
    : clientSocket(clientSocket) {}

void ClientHandler::handle() {
    char buffer[1024];

    Logger::info("Client handler started");

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0) {
            Logger::info("Client disconnected");
            break;
        }

        std::string message(buffer, bytesReceived);
        Logger::info("Received: " + message);

        std::string response = "Server received: " + message;
        send(clientSocket, response.c_str(), response.size(), 0);
    }

#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
}