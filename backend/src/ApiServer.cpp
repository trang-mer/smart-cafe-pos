#include "ApiServer.h"
#include "Logger.h"

#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#define CLOSE_SOCKET closesocket
#else
#define CLOSE_SOCKET close
#endif

ApiServer::ApiServer(int port)
    : port(port), serverSocket(-1), running(false),
      menuManager(nullptr), orderManager(nullptr), tableManager(nullptr),
      customerManager(nullptr), statsManager(nullptr) {}

ApiServer::~ApiServer() {
    stop();
}

void ApiServer::start() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        Logger::error("WSAStartup failed");
        return;
    }
#endif

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        Logger::error("Failed to create socket");
        return;
    }

    int opt = 1;
#ifdef _WIN32
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        Logger::error("Bind failed");
        CLOSE_SOCKET(serverSocket);
        return;
    }

    if (listen(serverSocket, 10) < 0) {
        Logger::error("Listen failed");
        CLOSE_SOCKET(serverSocket);
        return;
    }

    running = true;
    Logger::info("API Server started on port " + std::to_string(port));

    acceptThread = std::thread([this]() { acceptConnections(); });
}

void ApiServer::stop() {
    running = false;
    
    if (serverSocket >= 0) {
        CLOSE_SOCKET(serverSocket);
        serverSocket = -1;
    }
    
    if (acceptThread.joinable()) {
        acceptThread.join();
    }
    
#ifdef _WIN32
    WSACleanup();
#endif
    
    Logger::info("API Server stopped");
}

bool ApiServer::isRunning() const {
    return running;
}

void ApiServer::setMenuManager(MenuManager* manager) { menuManager = manager; }
void ApiServer::setOrderManager(OrderManager* manager) { orderManager = manager; }
void ApiServer::setTableManager(TableManager* manager) { tableManager = manager; }
void ApiServer::setCustomerManager(CustomerManager* manager) { customerManager = manager; }
void ApiServer::setStatsManager(StatsManager* manager) { statsManager = manager; }

void ApiServer::acceptConnections() {
    while (running) {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);
        
        if (clientSocket < 0) {
            if (running) {
                Logger::error("Accept failed");
            }
            continue;
        }

        std::thread([this, clientSocket]() {
            handleClient(clientSocket);
        }).detach();
    }
}

void ApiServer::handleClient(int clientSocket) {
    char buffer[8192];
    std::string requestData;

    while (running) {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesRead <= 0) {
            break;
        }

        requestData.append(buffer, bytesRead);

        size_t headerEnd = requestData.find("\r\n\r\n");
        if (headerEnd == std::string::npos) {
            continue;
        }

        std::string headers = requestData.substr(0, headerEnd);
        std::string body;

        std::istringstream headerStream(headers);
        std::string firstLine;
        std::getline(headerStream, firstLine);

        std::regex contentLengthRegex("Content-Length:\\s*(\\d+)");
        std::smatch match;
        int contentLength = 0;
        if (std::regex_search(headers, match, contentLengthRegex)) {
            contentLength = std::stoi(match[1]);
        }

        size_t bodyStart = headerEnd + 4;
        size_t availableBody = requestData.length() - bodyStart;

        if (contentLength > 0 && availableBody < (size_t)contentLength) {
            continue;
        }

        if (contentLength > 0) {
            body = requestData.substr(bodyStart, contentLength);
        }

        HttpRequest request = parseRequest(firstLine + "\n" + headers);
        request.body = body;

        HttpResponse response = handleRequest(request);
        sendResponse(clientSocket, response);

        break;
    }

    CLOSE_SOCKET(clientSocket);
}

HttpRequest ApiServer::parseRequest(const std::string& rawRequest) {
    HttpRequest request;
    std::istringstream stream(rawRequest);
    std::string line;

    std::getline(stream, line);
    std::istringstream lineStream(line);
    lineStream >> request.method >> request.path;

    size_t queryPos = request.path.find('?');
    if (queryPos != std::string::npos) {
        request.query = request.path.substr(queryPos + 1);
        request.path = request.path.substr(0, queryPos);
    }

    while (std::getline(stream, line)) {
        if (line == "\r" || line.empty()) break;
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            if (!value.empty() && value[0] == ' ') {
                value = value.substr(1);
            }
            request.headers[key] = value;
        }
    }

    return request;
}

HttpResponse ApiServer::handleRequest(const HttpRequest& request) {
    if (request.path == "/api/health" || request.path == "/health") {
        return {200, "OK", "{\"status\":\"ok\"}", "application/json"};
    }

    if (request.path == "/api/menu" || request.path.rfind("/api/menu", 0) == 0) {
        return handleMenu(request.path, request.method, request.body);
    }

    if (request.path == "/api/orders" || request.path.rfind("/api/orders", 0) == 0) {
        return handleOrders(request.path, request.method, request.body);
    }

    if (request.path == "/api/tables" || request.path.rfind("/api/tables", 0) == 0) {
        return handleTables(request.path, request.method, request.body);
    }

    if (request.path == "/api/customers" || request.path.rfind("/api/customers", 0) == 0) {
        return handleCustomers(request.path, request.method, request.body);
    }

    if (request.path == "/api/stats" || request.path.rfind("/api/stats", 0) == 0) {
        return handleStats(request.path, request.method, request.body);
    }

    return {404, "Not Found", "{\"error\":\"Not found\"}", "application/json"};
}

HttpResponse ApiServer::handleMenu(const std::string& path, const std::string& method, const std::string& body) {
    if (method == "GET") {
        std::string category = parseQueryParam(path, "category");
        
        json items;
        if (category.empty()) {
            items = menuManager->getAllItems();
        } else {
            items = menuManager->getItemsByCategory(category);
        }
        
        return {200, "OK", items.dump(), "application/json"};
    }

    return {405, "Method Not Allowed", "{\"error\":\"Method not allowed\"}", "application/json"};
}

HttpResponse ApiServer::handleOrders(const std::string& path, const std::string& method, const std::string& body) {
    std::string orderId;
    size_t lastSlash = path.rfind('/');
    if (lastSlash != std::string::npos && lastSlash > 9) {
        std::string afterSlash = path.substr(lastSlash + 1);
        if (!afterSlash.empty() && afterSlash != "orders") {
            orderId = afterSlash;
        }
    }

    if (method == "GET") {
        if (!orderId.empty() && orderId != "status") {
            int id = std::stoi(orderId);
            json order = orderManager->getOrderJson(id);
            if (order.contains("error")) {
                return {404, "Not Found", "{\"error\":\"Order not found\"}", "application/json"};
            }
            return {200, "OK", order.dump(), "application/json"};
        }

        std::string status = parseQueryParam(path, "status");
        
        json orders;
        if (status.empty()) {
            orders = orderManager->getAllOrdersJson();
        } else {
            orders = orderManager->getOrdersByStatus(status);
        }
        
        return {200, "OK", orders.dump(), "application/json"};
    }

    if (method == "POST") {
        try {
            json data = json::parse(body);
            int tableNumber = data.value("table", 0);
            json items = data.value("lines", json::array());
            std::string note = data.value("note", "");

            int orderId = orderManager->createOrder(tableNumber, items, note);
            json order = orderManager->getOrderJson(orderId);

            return {201, "Created", order.dump(), "application/json"};
        } catch (const std::exception& e) {
            return {400, "Bad Request", "{\"error\":\"" + std::string(e.what()) + "\"}", "application/json"};
        }
    }

    if (method == "PATCH" && !orderId.empty()) {
        try {
            json data = json::parse(body);
            int id = std::stoi(orderId);

            if (data.contains("status")) {
                std::string status = data["status"];
                OrderStatus orderStatus = OrderManager::parseStatus(status);
                orderManager->updateOrderStatus(id, orderStatus);
            }

            json order = orderManager->getOrderJson(id);
            return {200, "OK", order.dump(), "application/json"};
        } catch (const std::exception& e) {
            return {400, "Bad Request", "{\"error\":\"" + std::string(e.what()) + "\"}", "application/json"};
        }
    }

    if (method == "DELETE" && !orderId.empty()) {
        try {
            int id = std::stoi(orderId);
            orderManager->cancelOrder(id);
            return {200, "OK", "{\"success\":true}", "application/json"};
        } catch (const std::exception& e) {
            return {400, "Bad Request", "{\"error\":\"" + std::string(e.what()) + "\"}", "application/json"};
        }
    }

    return {405, "Method Not Allowed", "{\"error\":\"Method not allowed\"}", "application/json"};
}

HttpResponse ApiServer::handleTables(const std::string& path, const std::string& method, const std::string& body) {
    if (method == "GET") {
        json tables = tableManager->getAllTables();
        return {200, "OK", tables.dump(), "application/json"};
    }

    if (method == "PATCH") {
        try {
            json data = json::parse(body);
            int id = data.value("id", 0);
            if (data.contains("status")) {
                tableManager->updateTableStatus(id, data["status"]);
            }
            json table = tableManager->getTableById(id);
            return {200, "OK", table.dump(), "application/json"};
        } catch (const std::exception& e) {
            return {400, "Bad Request", "{\"error\":\"" + std::string(e.what()) + "\"}", "application/json"};
        }
    }

    return {405, "Method Not Allowed", "{\"error\":\"Method not allowed\"}", "application/json"};
}

HttpResponse ApiServer::handleCustomers(const std::string& path, const std::string& method, const std::string& body) {
    std::string customerId;
    size_t lastSlash = path.rfind('/');
    if (lastSlash != std::string::npos && lastSlash > 10) {
        std::string afterSlash = path.substr(lastSlash + 1);
        if (!afterSlash.empty() && afterSlash != "customers" && afterSlash.find('?') == std::string::npos) {
            customerId = afterSlash;
        }
    }

    if (method == "GET") {
        if (!customerId.empty()) {
            int id = std::stoi(customerId);
            json customer = customerManager->getCustomerById(id);
            if (customer.contains("error")) {
                return {404, "Not Found", "{\"error\":\"Customer not found\"}", "application/json"};
            }
            return {200, "OK", customer.dump(), "application/json"};
        }

        std::string query = parseQueryParam(path, "q");
        if (!query.empty()) {
            json customers = customerManager->searchCustomers(query);
            return {200, "OK", customers.dump(), "application/json"};
        }

        json customers = customerManager->getAllCustomers();
        return {200, "OK", customers.dump(), "application/json"};
    }

    if (method == "POST") {
        try {
            json data = json::parse(body);
            std::string name = data.value("name", "");
            std::string phone = data.value("phone", "");
            std::string email = data.value("email", "");

            int id = customerManager->createCustomer(name, phone, email);
            json customer = customerManager->getCustomerById(id);

            return {201, "Created", customer.dump(), "application/json"};
        } catch (const std::exception& e) {
            return {400, "Bad Request", "{\"error\":\"" + std::string(e.what()) + "\"}", "application/json"};
        }
    }

    return {405, "Method Not Allowed", "{\"error\":\"Method not allowed\"}", "application/json"};
}

HttpResponse ApiServer::handleStats(const std::string& path, const std::string& method, const std::string& body) {
    if (method == "GET") {
        std::string type = parseQueryParam(path, "type");
        std::string period = parseQueryParam(path, "period");
        std::string limitStr = parseQueryParam(path, "limit");

        if (path.find("revenue") != std::string::npos) {
            if (period.empty()) period = "week";
            json revenue = statsManager->getRevenueByPeriod(period);
            return {200, "OK", revenue.dump(), "application/json"};
        }

        if (path.find("top-items") != std::string::npos) {
            int limit = limitStr.empty() ? 5 : std::stoi(limitStr);
            json topItems = statsManager->getTopItems(limit);
            return {200, "OK", topItems.dump(), "application/json"};
        }

        if (path.find("orders") != std::string::npos) {
            json orderStats = statsManager->getOrderStats();
            return {200, "OK", orderStats.dump(), "application/json"};
        }

        if (path.find("revenue-by-hour") != std::string::npos) {
            json hourly = statsManager->getRevenueByHour();
            return {200, "OK", hourly.dump(), "application/json"};
        }

        json response = {
            {"revenue", statsManager->getRevenueByPeriod(period.empty() ? "week" : period)},
            {"topItems", statsManager->getTopItems(limitStr.empty() ? 5 : std::stoi(limitStr))},
            {"orderStats", statsManager->getOrderStats()},
            {"revenueByHour", statsManager->getRevenueByHour()},
            {"todayRevenue", statsManager->getTodayRevenue()},
            {"todayOrders", statsManager->getTodayOrderCount()}
        };

        return {200, "OK", response.dump(), "application/json"};
    }

    return {405, "Method Not Allowed", "{\"error\":\"Method not allowed\"}", "application/json"};
}

std::string ApiServer::urlDecode(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '%' && i + 2 < str.length()) {
            std::string hex = str.substr(i + 1, 2);
            char decoded = (char)std::stoi(hex, nullptr, 16);
            result += decoded;
            i += 2;
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    return result;
}

std::string ApiServer::parseQueryParam(const std::string& path, const std::string& param) {
    size_t queryPos = path.find('?');
    if (queryPos == std::string::npos) {
        return "";
    }

    std::string query = path.substr(queryPos + 1);
    size_t pos = query.find(param + "=");
    if (pos == std::string::npos) {
        return "";
    }

    pos += param.length() + 1;
    size_t end = query.find('&', pos);
    if (end == std::string::npos) {
        end = query.length();
    }

    return urlDecode(query.substr(pos, end - pos));
}

void ApiServer::sendResponse(int clientSocket, const HttpResponse& response) {
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 " << response.statusCode << " " << response.statusText << "\r\n";
    responseStream << "Content-Type: " << response.contentType << "\r\n";
    responseStream << "Content-Length: " << response.body.length() << "\r\n";
    responseStream << "Access-Control-Allow-Origin: *\r\n";
    responseStream << "Access-Control-Allow-Methods: GET, POST, PATCH, DELETE, OPTIONS\r\n";
    responseStream << "Access-Control-Allow-Headers: Content-Type\r\n";
    responseStream << "Connection: close\r\n";
    responseStream << "\r\n";
    responseStream << response.body;

    std::string responseStr = responseStream.str();
    send(clientSocket, responseStr.c_str(), responseStr.length(), 0);
}
