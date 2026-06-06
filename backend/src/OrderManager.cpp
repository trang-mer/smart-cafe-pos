#include "OrderManager.h"

#include <ctime>
#include <iomanip>
#include <sstream>

OrderManager::OrderManager()
    : nextOrderId(1) {}

int OrderManager::createOrder(
    int tableNumber,
    const json& itemsJson,
    const std::string& note
) {
    std::lock_guard<std::mutex> lock(ordersMutex);

    Order order;
    order.id = nextOrderId++;
    order.tableNumber = tableNumber;
    order.note = note;
    order.status = OrderStatus::PENDING;
    order.createdAt = getCurrentDateTime();

    for (const auto& itemJson : itemsJson) {
        OrderItem item;
        item.name = itemJson.value("name", "Unknown");
        item.qty = itemJson.value("qty", 1);
        item.price = itemJson.value("price", 0.0);

        order.items.push_back(item);
    }

    orders.push_back(order);

    return order.id;
}

bool OrderManager::updateOrderStatus(int orderId, OrderStatus status) {
    std::lock_guard<std::mutex> lock(ordersMutex);

    for (auto& order : orders) {
        if (order.id == orderId) {
            order.status = status;
            return true;
        }
    }

    return false;
}

bool OrderManager::cancelOrder(int orderId) {
    return updateOrderStatus(orderId, OrderStatus::CANCELLED);
}

json OrderManager::getOrderJson(int orderId) {
    std::lock_guard<std::mutex> lock(ordersMutex);

    for (const auto& order : orders) {
        if (order.id == orderId) {
            return orderToJson(order);
        }
    }

    return {
        {"error", "Order not found"},
        {"orderId", orderId}
    };
}

json OrderManager::getAllOrdersJson() {
    std::lock_guard<std::mutex> lock(ordersMutex);

    json result = json::array();

    for (const auto& order : orders) {
        result.push_back(orderToJson(order));
    }

    return result;
}

json OrderManager::getOrdersByStatus(const std::string& status) {
    std::lock_guard<std::mutex> lock(ordersMutex);

    json result = json::array();
    OrderStatus targetStatus = parseStatus(status);

    for (const auto& order : orders) {
        if (order.status == targetStatus) {
            result.push_back(orderToJson(order));
        }
    }

    return result;
}

json OrderManager::getOrdersByTable(int tableNumber) {
    std::lock_guard<std::mutex> lock(ordersMutex);

    json result = json::array();

    for (const auto& order : orders) {
        if (order.tableNumber == tableNumber) {
            result.push_back(orderToJson(order));
        }
    }

    return result;
}

int OrderManager::getPendingOrderCount() {
    std::lock_guard<std::mutex> lock(ordersMutex);

    int count = 0;
    for (const auto& order : orders) {
        if (order.status == OrderStatus::PENDING) {
            count++;
        }
    }
    return count;
}

int OrderManager::getCookingOrderCount() {
    std::lock_guard<std::mutex> lock(ordersMutex);

    int count = 0;
    for (const auto& order : orders) {
        if (order.status == OrderStatus::COOKING) {
            count++;
        }
    }
    return count;
}

OrderStatus OrderManager::parseStatus(const std::string& statusText) {
    if (statusText == "new" || statusText == "pending") return OrderStatus::PENDING;
    if (statusText == "cooking" || statusText == "cooking") return OrderStatus::COOKING;
    if (statusText == "done" || statusText == "completed") return OrderStatus::DONE;
    if (statusText == "paid" || statusText == "paid") return OrderStatus::PAID;
    if (statusText == "cancelled" || statusText == "cancelled") return OrderStatus::CANCELLED;

    return OrderStatus::PENDING;
}

std::string OrderManager::statusToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::PENDING:
            return "new";
        case OrderStatus::COOKING:
            return "cooking";
        case OrderStatus::DONE:
            return "done";
        case OrderStatus::PAID:
            return "paid";
        case OrderStatus::CANCELLED:
            return "cancelled";
        default:
            return "unknown";
    }
}

std::string OrderManager::getCurrentTime() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);

    std::ostringstream oss;
    oss << std::put_time(localTime, "%H:%M");

    return oss.str();
}

std::string OrderManager::getCurrentDateTime() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);

    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");

    return oss.str();
}

json OrderManager::orderToJson(const Order& order) {
    json itemsJson = json::array();

    double total = 0;
    for (const auto& item : order.items) {
        double lineTotal = item.qty * item.price;
        total += lineTotal;
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
        {"priority", "normal"},
        {"items", itemsJson},
        {"lines", itemsJson},
        {"note", order.note},
        {"status", statusToString(order.status)},
        {"total", total},
        {"createdAt", order.createdAt}
    };
}
