#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

std::atomic<bool> running(true);

void receiveMessages(SOCKET clientSocket) {
    char buffer[1024];

    while (running) {
        memset(buffer, 0, sizeof(buffer));

        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0) {
            std::cout << "\nDisconnected from server.\n";
            running = false;
            break;
        }

        std::string message(buffer, bytesReceived);
        std::cout << "\n[SERVER] " << message << "\n> ";
        std::cout.flush();
    }
}

int main(int argc, char* argv[]) {
    std::string serverIp = "127.0.0.1";
    int port = 8080;

    if (argc >= 2) {
        serverIp = argv[1];
    }

    if (argc >= 3) {
        port = std::stoi(argv[2]);
    }

#ifdef _WIN32
    WSADATA wsaData;
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (wsaResult != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }
#endif

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket.\n";
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = inet_addr(serverIp.c_str());

    std::cout << "Connecting to server " << serverIp << ":" << port << "...\n";

    int connectResult = connect(
        clientSocket,
        reinterpret_cast<sockaddr*>(&serverAddress),
        sizeof(serverAddress)
    );

    if (connectResult == SOCKET_ERROR) {
        std::cerr << "Cannot connect to server.\n";

#ifdef _WIN32
        closesocket(clientSocket);
        WSACleanup();
#else
        close(clientSocket);
#endif
        return 1;
    }

    std::cout << "Connected to Smart Cafe POS Server.\n";
    std::cout << "Type message and press Enter.\n";
    std::cout << "Type /quit to exit.\n\n";

    std::thread receiverThread(receiveMessages, clientSocket);

    std::string message;

    while (running) {
        std::cout << "> ";
        std::getline(std::cin, message);

        if (message == "/quit") {
            running = false;
            break;
        }

        if (message.empty()) {
            continue;
        }

        int sendResult = send(clientSocket, message.c_str(), message.size(), 0);

        if (sendResult == SOCKET_ERROR) {
            std::cerr << "Failed to send message.\n";
            running = false;
            break;
        }
    }

#ifdef _WIN32
    closesocket(clientSocket);
    WSACleanup();
#else
    close(clientSocket);
#endif

    if (receiverThread.joinable()) {
        receiverThread.detach();
    }

    std::cout << "Client closed.\n";

    return 0;
}