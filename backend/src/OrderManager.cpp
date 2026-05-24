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
    order.createdAt = getCurrentTime();

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

json OrderManager::getOrderJson(int orderId) {
    std::lock_guard<std::mutex> lock(ordersMutex);

    for (const auto& order : orders) {
        if (order.id == orderId) {
            return orderToJson(order);
        }
    }

    return {
        {"type", "ERROR"},
        {"message", "Order not found"}
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

OrderStatus OrderManager::parseStatus(const std::string& statusText) {
    if (statusText == "pending") return OrderStatus::PENDING;
    if (statusText == "cooking") return OrderStatus::COOKING;
    if (statusText == "done") return OrderStatus::DONE;
    if (statusText == "paid") return OrderStatus::PAID;
    if (statusText == "cancelled") return OrderStatus::CANCELLED;

    return OrderStatus::PENDING;
}

std::string OrderManager::statusToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::PENDING:
            return "pending";
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
    oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");

    return oss.str();
}

json OrderManager::orderToJson(const Order& order) {
    json itemsJson = json::array();

    for (const auto& item : order.items) {
        itemsJson.push_back({
            {"name", item.name},
            {"qty", item.qty},
            {"price", item.price}
        });
    }

    return {
        {"id", order.id},
        {"table", order.tableNumber},
        {"items", itemsJson},
        {"note", order.note},
        {"status", statusToString(order.status)},
        {"createdAt", order.createdAt}
    };
}