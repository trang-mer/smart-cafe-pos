#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <cstring>

#include "nlohmann/json.hpp"

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

using json = nlohmann::json;

std::atomic<bool> running(true);

void sendJson(SOCKET socket, const json& data) {
    std::string text = data.dump() + "\n";
    send(socket, text.c_str(), text.size(), 0);
}

void receiveMessages(SOCKET clientSocket) {
    char buffer[1024];
    std::string pendingData;

    while (running) {
        memset(buffer, 0, sizeof(buffer));

        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0) {
            std::cout << "\nDisconnected from server.\n";
            running = false;
            break;
        }

        pendingData.append(buffer, bytesReceived);

        size_t pos;
        while ((pos = pendingData.find('\n')) != std::string::npos) {
            std::string jsonText = pendingData.substr(0, pos);
            pendingData.erase(0, pos + 1);

            if (!jsonText.empty()) {
                try {
                    json response = json::parse(jsonText);
                    std::cout << "\n[SERVER JSON]\n" << response.dump(4) << "\n> ";
                } catch (...) {
                    std::cout << "\n[SERVER RAW] " << jsonText << "\n> ";
                }

                std::cout.flush();
            }
        }
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

    std::thread receiverThread(receiveMessages, clientSocket);

    std::string role;
    std::cout << "\nChoose role: CASHIER / KITCHEN / MANAGER\n";
    std::cout << "> ";
    std::getline(std::cin, role);

    json roleMessage = {
        {"type", "ROLE"},
        {"role", role}
    };

    sendJson(clientSocket, roleMessage);

    std::cout << "\nExample messages:\n\n";

    std::cout << "CASHIER order:\n";
    std::cout << R"({"type":"ORDER","table":3,"items":[{"name":"Latte","qty":2},{"name":"Matcha","qty":1}],"note":"less sugar"})" << "\n\n";

    std::cout << "KITCHEN status:\n";
    std::cout << R"({"type":"STATUS","orderId":1,"status":"cooking"})" << "\n\n";

    std::cout << "MANAGER report:\n";
    std::cout << R"({"type":"REPORT","message":"Daily revenue checked"})" << "\n\n";

    std::cout << "Type JSON message and press Enter. Type /quit to exit.\n\n";

    std::string input;

    while (running) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "/quit") {
            running = false;
            break;
        }

        if (input.empty()) {
            continue;
        }

        try {
            json message = json::parse(input);
            sendJson(clientSocket, message);
        } catch (...) {
            std::cout << "Invalid JSON. Please type a valid JSON message.\n";
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