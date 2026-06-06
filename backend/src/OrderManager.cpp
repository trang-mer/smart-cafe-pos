#include "OrderManager.h"
#include "Database.h"
#include "logger.h"

#include <ctime>
#include <iomanip>
#include <sstream>

OrderManager::OrderManager() {}

int OrderManager::createOrder(
    int tableNumber,
    const json& itemsJson,
    const std::string& note,
    int createdBy
) {
    Database& db = Database::getInstance();
    
    double total = 0;
    for (const auto& itemJson : itemsJson) {
        total += itemJson.value("qty", 1) * itemJson.value("price", 0.0);
    }

    OrderEntity order;
    order.tableNumber = tableNumber;
    order.priority = "normal";
    order.note = note;
    order.status = OrderStatus::PENDING;
    order.total = total;
    order.createdBy = createdBy;
    order.createdAt = getCurrentDateTime();

    int orderId = db.insertOrder(order);

    if (orderId <= 0) {
        Logger::error("Failed to create order");
        return -1;
    }

    for (const auto& itemJson : itemsJson) {
        OrderItemEntity item;
        item.orderId = orderId;
        item.menuItemId = 0;
        item.name = itemJson.value("name", "Unknown");
        item.qty = itemJson.value("qty", 1);
        item.price = itemJson.value("price", 0.0);
        db.insertOrderItem(item);
    }

    Logger::info("Order #" + std::to_string(orderId) + " created for table " + std::to_string(tableNumber));
    return orderId;
}

bool OrderManager::updateOrderStatus(int orderId, OrderStatus status) {
    Database& db = Database::getInstance();
    return db.updateOrderStatus(orderId, status);
}

bool OrderManager::cancelOrder(int orderId) {
    return updateOrderStatus(orderId, OrderStatus::CANCELLED);
}

json OrderManager::getOrderJson(int orderId) {
    Database& db = Database::getInstance();
    OrderEntity order = db.getOrderById(orderId);

    if (order.id <= 0) {
        return {{"error", "Order not found"}, {"orderId", orderId}};
    }

    return orderEntityToJson(order);
}

json OrderManager::getAllOrdersJson() {
    Database& db = Database::getInstance();
    std::vector<OrderEntity> orders = db.getAllOrders();

    json result = json::array();
    for (const auto& order : orders) {
        result.push_back(orderEntityToJson(order));
    }

    return result;
}

json OrderManager::getOrdersByStatus(const std::string& status) {
    Database& db = Database::getInstance();
    OrderStatus targetStatus = parseStatus(status);
    std::vector<OrderEntity> orders = db.getOrdersByStatus(targetStatus);

    json result = json::array();
    for (const auto& order : orders) {
        result.push_back(orderEntityToJson(order));
    }

    return result;
}

json OrderManager::getOrdersByTable(int tableNumber) {
    Database& db = Database::getInstance();
    std::vector<OrderEntity> orders = db.getOrdersByTable(tableNumber);

    json result = json::array();
    for (const auto& order : orders) {
        result.push_back(orderEntityToJson(order));
    }

    return result;
}

int OrderManager::getPendingOrderCount() {
    Database& db = Database::getInstance();
    std::vector<OrderEntity> orders = db.getOrdersByStatus(OrderStatus::PENDING);
    return static_cast<int>(orders.size());
}

int OrderManager::getCookingOrderCount() {
    Database& db = Database::getInstance();
    std::vector<OrderEntity> orders = db.getOrdersByStatus(OrderStatus::COOKING);
    return static_cast<int>(orders.size());
}

OrderStatus OrderManager::parseStatus(const std::string& statusText) {
    if (statusText == "new" || statusText == "pending") return OrderStatus::PENDING;
    if (statusText == "cooking") return OrderStatus::COOKING;
    if (statusText == "done" || statusText == "completed") return OrderStatus::DONE;
    if (statusText == "paid") return OrderStatus::PAID;
    if (statusText == "cancelled") return OrderStatus::CANCELLED;

    return OrderStatus::PENDING;
}

std::string OrderManager::statusToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::PENDING: return "new";
        case OrderStatus::COOKING: return "cooking";
        case OrderStatus::DONE: return "done";
        case OrderStatus::PAID: return "paid";
        case OrderStatus::CANCELLED: return "cancelled";
        default: return "unknown";
    }
}

std::string OrderManager::getCurrentDateTime() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);

    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");

    return oss.str();
}

json OrderManager::orderEntityToJson(const OrderEntity& order) {
    json itemsJson = json::array();

    for (const auto& item : order.items) {
        double lineTotal = item.qty * item.price;
        itemsJson.push_back({
            {"itemId", item.name},
            {"name", item.name},
            {"qty", item.qty},
            {"price", item.price},
            {"total", lineTotal}
        });
    }

    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    std::ostringstream timeoss;
    timeoss << std::put_time(localTime, "%H:%M");

    return {
        {"id", "#" + std::to_string(order.id)},
        {"orderId", order.id},
        {"table", order.tableNumber == 0 ? "Mang đi" : "Bàn " + std::to_string(order.tableNumber)},
        {"tableNumber", order.tableNumber},
        {"time", timeoss.str()},
        {"priority", order.priority},
        {"items", itemsJson},
        {"lines", itemsJson},
        {"note", order.note},
        {"status", statusToString(order.status)},
        {"total", order.total},
        {"createdAt", order.createdAt}
    };
}
