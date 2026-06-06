#ifndef ORDER_MANAGER_H
#define ORDER_MANAGER_H

#include "Order.h"
#include "nlohmann/json.hpp"

#include <vector>
#include <mutex>
#include <string>

using json = nlohmann::json;

class OrderManager {
public:
    OrderManager();

    int createOrder(
        int tableNumber,
        const json& itemsJson,
        const std::string& note
    );

    bool updateOrderStatus(int orderId, OrderStatus status);
    bool cancelOrder(int orderId);

    json getOrderJson(int orderId);
    json getAllOrdersJson();
    json getOrdersByStatus(const std::string& status);
    json getOrdersByTable(int tableNumber);

    int getPendingOrderCount();
    int getCookingOrderCount();

    static OrderStatus parseStatus(const std::string& statusText);
    static std::string statusToString(OrderStatus status);

private:
    int nextOrderId;
    std::vector<Order> orders;
    std::mutex ordersMutex;

    std::string getCurrentTime();
    std::string getCurrentDateTime();
    json orderToJson(const Order& order);
};

#endif
