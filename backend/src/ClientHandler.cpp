#include "ClientHandler.h"
#include "Server.h"
#include "Logger.h"
#include "OrderManager.h"
#include "MenuManager.h"
#include "TableManager.h"
#include "CustomerManager.h"
#include "StatsManager.h"

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
    char buffer[4096];
    std::string pendingData;

    Logger::info("Client handler started");

    json welcome = {
        {"type", "WELCOME"},
        {"message", "Welcome to Smart Cafe POS Server. Please set role first."},
        {"supportedTypes", {"ROLE", "LOGIN", "ORDER", "STATUS", "PAYMENT", "GET_ORDERS", "GET_MENU", "GET_TABLES", "GET_CUSTOMERS", "GET_STATS"}}
    };

    server->sendToClient(clientSocket, welcome.dump());

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

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
                try {
                    processJsonMessage(jsonText);
                } catch (const std::exception& e) {
                    Logger::error(std::string("Error processing message: ") + e.what());
                    json error = {{"type", "ERROR"}, {"message", "Internal server error"}};
                    server->sendToClient(clientSocket, error.dump());
                }
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
            {"message", "Invalid JSON format"},
            {"details", e.what()}
        };
        server->sendToClient(clientSocket, errorResponse.dump());
        return;
    }

    std::string type = message.value("type", "");

    if (type == "ROLE" || type == "LOGIN") {
        handleLogin(message);
        return;
    }

    if (type == "PING") {
        json response = {{"type", "PONG"}, {"timestamp", message.value("timestamp", 0)}};
        server->sendToClient(clientSocket, response.dump());
        return;
    }

    ClientRole senderRole = server->getClientRole(clientSocket);

    if (senderRole == ClientRole::UNKNOWN) {
        json errorResponse = {
            {"type", "ERROR"},
            {"code", "NOT_AUTHENTICATED"},
            {"message", "Please login first using ROLE or LOGIN command."}
        };
        server->sendToClient(clientSocket, errorResponse.dump());
        return;
    }

    if (type == "ORDER" || type == "CREATE_ORDER") {
        handleCreateOrder(message, senderRole);
        return;
    }

    if (type == "STATUS" || type == "UPDATE_STATUS") {
        handleUpdateStatus(message, senderRole);
        return;
    }

    if (type == "PAYMENT" || type == "PAY") {
        handlePayment(message, senderRole);
        return;
    }

    if (type == "CANCEL_ORDER") {
        handleCancelOrder(message, senderRole);
        return;
    }

    if (type == "GET_ORDERS") {
        handleGetOrders(message, senderRole);
        return;
    }

    if (type == "GET_ORDER") {
        handleGetOrder(message, senderRole);
        return;
    }

    if (type == "GET_MENU") {
        handleGetMenu(message, senderRole);
        return;
    }

    if (type == "GET_TABLES") {
        handleGetTables(message, senderRole);
        return;
    }

    if (type == "UPDATE_TABLE") {
        handleUpdateTable(message, senderRole);
        return;
    }

    if (type == "GET_CUSTOMERS") {
        handleGetCustomers(message, senderRole);
        return;
    }

    if (type == "SEARCH_CUSTOMERS") {
        handleSearchCustomers(message, senderRole);
        return;
    }

    if (type == "CREATE_CUSTOMER") {
        handleCreateCustomer(message, senderRole);
        return;
    }

    if (type == "GET_STATS") {
        handleGetStats(message, senderRole);
        return;
    }

    if (type == "REPORT") {
        handleReport(message, senderRole);
        return;
    }

    if (type == "BROADCAST") {
        handleBroadcast(message, senderRole);
        return;
    }

    json errorResponse = {
        {"type", "ERROR"},
        {"code", "UNKNOWN_COMMAND"},
        {"message", "Unknown message type: " + type},
        {"supportedTypes", {"ROLE", "LOGIN", "ORDER", "STATUS", "PAYMENT", "GET_ORDERS", "GET_MENU", "GET_TABLES", "GET_CUSTOMERS", "GET_STATS", "PING"}}
    };
    server->sendToClient(clientSocket, errorResponse.dump());
}

void ClientHandler::handleLogin(const json& message) {
    std::string roleText = message.value("role", "");
    std::string username = message.value("username", "Anonymous");

    ClientRole role = Server::parseRole(roleText);

    if (role == ClientRole::UNKNOWN) {
        json errorResponse = {
            {"type", "ERROR"},
            {"code", "INVALID_ROLE"},
            {"message", "Invalid role. Use CASHIER, KITCHEN, or MANAGER."}
        };
        server->sendToClient(clientSocket, errorResponse.dump());
        return;
    }

    server->setClientRole(clientSocket, role);
    server->setClientUsername(clientSocket, username);

    json response = {
        {"type", "LOGIN_SUCCESS"},
        {"role", Server::roleToString(role)},
        {"username", username},
        {"message", "Login successful. Welcome, " + username + "!"}
    };

    server->sendToClient(clientSocket, response.dump());

    json notification = {
        {"type", "USER_JOINED"},
        {"username", username},
        {"role", Server::roleToString(role)}
    };
    server->broadcastMessageExcept(clientSocket, notification.dump());

    Logger::info("User " + username + " logged in as " + Server::roleToString(role));
}

void ClientHandler::handleCreateOrder(const json& message, ClientRole senderRole) {
    if (senderRole != ClientRole::CASHIER) {
        json errorResponse = {
            {"type", "ERROR"},
            {"code", "UNAUTHORIZED"},
            {"message", "Only CASHIER can create orders."}
        };
        server->sendToClient(clientSocket, errorResponse.dump());
        return;
    }

    int tableNumber = message.value("tableNumber", message.value("table", 0));
    json items = message.value("items", json::array());
    std::string note = message.value("note", "");
    std::string priority = message.value("priority", "normal");

    if (items.empty()) {
        json errorResponse = {
            {"type", "ERROR"},
            {"code", "INVALID_ORDER"},
            {"message", "Order must contain at least one item."}
        };
        server->sendToClient(clientSocket, errorResponse.dump());
        return;
    }

    int orderId = server->getOrderManager().createOrder(tableNumber, items, note);

    if (tableNumber > 0) {
        server->getTableManager().updateTableOrder(tableNumber, orderId);
    }

    std::vector<std::pair<std::string, int>> itemList;
    for (const auto& item : items) {
        itemList.push_back({item.value("name", "Unknown"), item.value("qty", 1)});
    }
    server->getStatsManager().recordOrder(tableNumber, itemList, 0);

    json createdOrder = server->getOrderManager().getOrderJson(orderId);

    json notification = {
        {"type", "NEW_ORDER"},
        {"order", createdOrder},
        {"priority", priority}
    };
    server->sendToRole(ClientRole::KITCHEN, notification.dump());
    server->sendToRole(ClientRole::MANAGER, notification.dump());

    json response = {
        {"type", "ORDER_CREATED"},
        {"success", true},
        {"message", "Order created successfully."},
        {"order", createdOrder}
    };

    server->sendToClient(clientSocket, response.dump());

    Logger::info("Order #" + std::to_string(orderId) + " created for table " + std::to_string(tableNumber));
}

void ClientHandler::handleUpdateStatus(const json& message, ClientRole senderRole) {
    if (senderRole != ClientRole::KITCHEN) {
        json errorResponse = {
            {"type", "ERROR"},
            {"code", "UNAUTHORIZED"},
            {"message", "Only KITCHEN can update order status."}
        };
        server->sendToClient(clientSocket, errorResponse.dump());
        return;
    }

    int orderId = message.value("orderId", message.value("id", 0));
    std::string statusText = message.value("status", "pending");

    OrderStatus newStatus = OrderManager::parseStatus(statusText);

    json oldOrder = server->getOrderManager().getOrderJson(orderId);
    std::string oldStatus = oldOrder.value("status", "pending");

    bool updated = server->getOrderManager().updateOrderStatus(orderId, newStatus);

    if (!updated) {
        json errorResponse = {
            {"type", "ERROR"},
            {"code", "ORDER_NOT_FOUND"},
            {"message", "Order not found."},
            {"orderId", orderId}
        };
        server->sendToClient(clientSocket, errorResponse.dump());
        return;
    }

    server->getStatsManager().recordOrderStatusChange(oldStatus, statusText);

    json updatedOrder = server->getOrderManager().getOrderJson(orderId);

    if (newStatus == OrderStatus::DONE) {
        int tableNum = updatedOrder.value("table", 0);
        if (tableNum > 0) {
            server->getTableManager().updateTableStatus(tableNum, TableStatus::AVAILABLE);
        }
    }

    json notification = {
        {"type", "ORDER_STATUS_CHANGED"},
        {"order", updatedOrder},
        {"previousStatus", oldStatus},
        {"newStatus", statusText}
    };

    server->sendToRole(ClientRole::CASHIER, notification.dump());
    server->sendToRole(ClientRole::MANAGER, notification.dump());

    json response = {
        {"type", "STATUS_UPDATED"},
        {"success", true},
        {"message", "Order status updated to " + statusText},
        {"order", updatedOrder}
    };

    server->sendToClient(clientSocket, response.dump());

    Logger::info("Order #" + std::to_string(orderId) + " status changed: " + oldStatus + " -> " + statusText);
}

void ClientHandler::handlePayment(const json& message, ClientRole senderRole) {
    if (senderRole != ClientRole::CASHIER) {
        json errorResponse = {
            {"type", "ERROR"},
            {"code", "UNAUTHORIZED"},
            {"message", "Only CASHIER can process payments."}
        };
        server->sendToClient(clientSocket, errorResponse.dump());
        return;
    }

    int orderId = message.value("orderId", message.value("id", 0));
    std::string paymentMethod = message.value("method", "cash");
    double amount = message.value("amount", 0);
    double received = message.value("received", amount);

    json order = server->getOrderManager().getOrderJson(orderId);

    if (order.contains("error")) {
        json errorResponse = {
            {"type", "ERROR"},
            {"code", "ORDER_NOT_FOUND"},
            {"message", "Order not found."}
        };
        server->sendToClient(clientSocket, errorResponse.dump());
        return;
    }

    double change = std::max(0.0, received - amount);

    server->getOrderManager().updateOrderStatus(orderId, OrderStatus::PAID);

    int tableNum = order.value("table", 0);
    if (tableNum > 0) {
        server->getTableManager().clearTableOrder(tableNum);
    }

    server->getStatsManager().recordPayment(orderId, amount);

    json paidOrder = server->getOrderManager().getOrderJson(orderId);

    json notification = {
        {"type", "ORDER_PAID"},
        {"order", paidOrder},
        {"method", paymentMethod},
        {"amount", amount},
        {"change", change}
    };

    server->sendToRole(ClientRole::KITCHEN, notification.dump());
    server->sendToRole(ClientRole::MANAGER, notification.dump());

    json response = {
        {"type", "PAYMENT_SUCCESS"},
        {"success", true},
        {"message", "Payment processed successfully."},
        {"order", paidOrder},
        {"paymentMethod", paymentMethod},
        {"amount", amount},
        {"change", change}
    };

    server->sendToClient(clientSocket, response.dump());

    Logger::info("Order #" + std::to_string(orderId) + " paid with " + paymentMethod + ": " + std::to_string(amount));
}

void ClientHandler::handleCancelOrder(const json& message, ClientRole senderRole) {
    if (senderRole != ClientRole::CASHIER && senderRole != ClientRole::MANAGER) {
        json errorResponse = {
            {"type", "ERROR"},
            {"code", "UNAUTHORIZED"},
            {"message", "Only CASHIER or MANAGER can cancel orders."}
        };
        server->sendToClient(clientSocket, errorResponse.dump());
        return;
    }

    int orderId = message.value("orderId", message.value("id", 0));

    bool cancelled = server->getOrderManager().updateOrderStatus(orderId, OrderStatus::CANCELLED);

    if (!cancelled) {
        json errorResponse = {
            {"type", "ERROR"},
            {"code", "ORDER_NOT_FOUND"},
            {"message", "Order not found."}
        };
        server->sendToClient(clientSocket, errorResponse.dump());
        return;
    }

    json cancelledOrder = server->getOrderManager().getOrderJson(orderId);

    json notification = {
        {"type", "ORDER_CANCELLED"},
        {"order", cancelledOrder}
    };

    server->broadcastMessage(notification.dump());

    json response = {
        {"type", "ORDER_CANCELLED"},
        {"success", true},
        {"message", "Order cancelled successfully."},
        {"order", cancelledOrder}
    };

    server->sendToClient(clientSocket, response.dump());

    Logger::info("Order #" + std::to_string(orderId) + " cancelled by " + Server::roleToString(senderRole));
}

void ClientHandler::handleGetOrders(const json& message, ClientRole senderRole) {
    std::string status = message.value("status", "");

    json orders;
    if (status.empty()) {
        orders = server->getOrderManager().getAllOrdersJson();
    } else {
        orders = server->getOrderManager().getOrdersByStatus(status);
    }

    json response = {
        {"type", "ORDERS_LIST"},
        {"count", orders.size()},
        {"orders", orders}
    };

    server->sendToClient(clientSocket, response.dump());
}

void ClientHandler::handleGetOrder(const json& message, ClientRole senderRole) {
    int orderId = message.value("orderId", message.value("id", 0));

    json order = server->getOrderManager().getOrderJson(orderId);

    if (order.contains("error")) {
        json errorResponse = {
            {"type", "ERROR"},
            {"code", "ORDER_NOT_FOUND"},
            {"message", "Order not found."}
        };
        server->sendToClient(clientSocket, errorResponse.dump());
        return;
    }

    json response = {
        {"type", "ORDER_DETAIL"},
        {"order", order}
    };

    server->sendToClient(clientSocket, response.dump());
}

void ClientHandler::handleGetMenu(const json& message, ClientRole senderRole) {
    std::string category = message.value("category", "");

    json items;
    if (category.empty()) {
        items = server->getMenuManager().getAllItems();
    } else {
        items = server->getMenuManager().getItemsByCategory(category);
    }

    json response = {
        {"type", "MENU_LIST"},
        {"count", items.size()},
        {"items", items}
    };

    server->sendToClient(clientSocket, response.dump());
}

void ClientHandler::handleGetTables(const json& message, ClientRole senderRole) {
    json tables = server->getTableManager().getAllTables();

    json response = {
        {"type", "TABLES_LIST"},
        {"count", tables.size()},
        {"tables", tables}
    };

    server->sendToClient(clientSocket, response.dump());
}

void ClientHandler::handleUpdateTable(const json& message, ClientRole senderRole) {
    if (senderRole != ClientRole::CASHIER) {
        json errorResponse = {
            {"type", "ERROR"},
            {"code", "UNAUTHORIZED"},
            {"message", "Only CASHIER can update tables."}
        };
        server->sendToClient(clientSocket, errorResponse.dump());
        return;
    }

    int tableId = message.value("tableId", message.value("id", 0));
    std::string status = message.value("status", "");

    if (!status.empty()) {
        TableStatus tableStatus = TableManager::stringToStatus(status);
        server->getTableManager().updateTableStatus(tableId, tableStatus);
    }

    json table = server->getTableManager().getTableById(tableId);

    json response = {
        {"type", "TABLE_UPDATED"},
        {"success", true},
        {"table", table}
    };

    server->sendToClient(clientSocket, response.dump());
}

void ClientHandler::handleGetCustomers(const json& message, ClientRole senderRole) {
    json customers = server->getCustomerManager().getAllCustomers();

    json response = {
        {"type", "CUSTOMERS_LIST"},
        {"count", customers.size()},
        {"customers", customers}
    };

    server->sendToClient(clientSocket, response.dump());
}

void ClientHandler::handleSearchCustomers(const json& message, ClientRole senderRole) {
    std::string query = message.value("query", "");

    json customers = server->getCustomerManager().searchCustomers(query);

    json response = {
        {"type", "CUSTOMERS_SEARCH"},
        {"query", query},
        {"count", customers.size()},
        {"customers", customers}
    };

    server->sendToClient(clientSocket, response.dump());
}

void ClientHandler::handleCreateCustomer(const json& message, ClientRole senderRole) {
    if (senderRole != ClientRole::CASHIER) {
        json errorResponse = {
            {"type", "ERROR"},
            {"code", "UNAUTHORIZED"},
            {"message", "Only CASHIER can create customers."}
        };
        server->sendToClient(clientSocket, errorResponse.dump());
        return;
    }

    std::string name = message.value("name", "");
    std::string phone = message.value("phone", "");
    std::string email = message.value("email", "");

    if (name.empty() || phone.empty()) {
        json errorResponse = {
            {"type", "ERROR"},
            {"code", "INVALID_DATA"},
            {"message", "Name and phone are required."}
        };
        server->sendToClient(clientSocket, errorResponse.dump());
        return;
    }

    int customerId = server->getCustomerManager().createCustomer(name, phone, email);

    json customer = server->getCustomerManager().getCustomerById(customerId);

    json response = {
        {"type", "CUSTOMER_CREATED"},
        {"success", true},
        {"customer", customer}
    };

    server->sendToClient(clientSocket, response.dump());
}

void ClientHandler::handleGetStats(const json& message, ClientRole senderRole) {
    std::string statsType = message.value("type", message.value("statsType", "all"));
    std::string period = message.value("period", "week");
    int limit = message.value("limit", 5);

    json response = {
        {"type", "STATS_DATA"},
        {"revenue", server->getStatsManager().getRevenueByPeriod(period)},
        {"topItems", server->getStatsManager().getTopItems(limit)},
        {"orderStats", server->getStatsManager().getOrderStats()},
        {"revenueByHour", server->getStatsManager().getRevenueByHour()},
        {"todayRevenue", server->getStatsManager().getTodayRevenue()},
        {"todayOrders", server->getStatsManager().getTodayOrderCount()}
    };

    server->sendToClient(clientSocket, response.dump());
}

void ClientHandler::handleReport(const json& message, ClientRole senderRole) {
    if (senderRole != ClientRole::MANAGER) {
        json errorResponse = {
            {"type", "ERROR"},
            {"code", "UNAUTHORIZED"},
            {"message", "Only MANAGER can send reports."}
        };
        server->sendToClient(clientSocket, errorResponse.dump());
        return;
    }

    std::string reportMessage = message.value("message", "");

    json notification = {
        {"type", "MANAGER_REPORT"},
        {"from", server->getClientUsername(clientSocket)},
        {"message", reportMessage},
        {"timestamp", time(nullptr)}
    };

    server->sendToRoles({ClientRole::CASHIER, ClientRole::KITCHEN}, notification.dump());

    json response = {
        {"type", "REPORT_SENT"},
        {"success", true},
        {"message", "Report broadcast to all stations."}
    };

    server->sendToClient(clientSocket, response.dump());
}

void ClientHandler::handleBroadcast(const json& message, ClientRole senderRole) {
    if (senderRole == ClientRole::UNKNOWN) {
        json errorResponse = {
            {"type", "ERROR"},
            {"code", "NOT_AUTHENTICATED"},
            {"message", "Please login first."}
        };
        server->sendToClient(clientSocket, errorResponse.dump());
        return;
    }

    std::string broadcastMessage = message.value("message", "");

    json notification = {
        {"type", "BROADCAST"},
        {"from", server->getClientUsername(clientSocket)},
        {"role", Server::roleToString(senderRole)},
        {"message", broadcastMessage},
        {"timestamp", time(nullptr)}
    };

    server->broadcastMessageExcept(clientSocket, notification.dump());

    json response = {
        {"type", "BROADCAST_SENT"},
        {"success", true}
    };

    server->sendToClient(clientSocket, response.dump());
}
