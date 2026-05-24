#include "Server.h"
#include "ClientHandler.h"
#include "Logger.h"

#include <thread>
#include <string>

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

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    int bindResult = bind(
        serverSocket,
        reinterpret_cast<sockaddr*>(&serverAddress),
        sizeof(serverAddress)
    );

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

        SOCKET clientSocket = accept(
            serverSocket,
            reinterpret_cast<sockaddr*>(&clientAddress),
            &clientSize
        );

        if (clientSocket == -1) {
            Logger::error("Accept failed");
            continue;
        }

        Logger::info("New client connected");

        std::thread clientThread([clientSocket]() {
            ClientHandler handler(clientSocket);
            handler.handle();
        });

        clientThread.detach();
    }
}

void Server::stop() {
    running = false;

#ifdef _WIN32
    if (serverSocket != INVALID_SOCKET) {
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