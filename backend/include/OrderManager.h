#ifndef ORDER_MANAGER_H
#define ORDER_MANAGER_H

#include "entity/OrderEntity.h"
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
        const std::string& note,
        int createdBy = 0
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
    std::string getCurrentDateTime();
    json orderEntityToJson(const OrderEntity& order);
};

#endif
