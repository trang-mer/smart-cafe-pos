#ifndef ORDER_ENTITY_H
#define ORDER_ENTITY_H

#include <string>
#include <vector>

enum class OrderStatus {
    PENDING,
    COOKING,
    DONE,
    PAID,
    CANCELLED
};

enum class OrderPriority {
    NORMAL,
    PRIORITY
};

struct OrderItemEntity {
    int id;
    int orderId;
    int menuItemId;
    std::string name;
    int qty;
    double price;
    std::string createdAt;
};

struct OrderEntity {
    int id;
    int tableNumber;
    std::string priority;
    std::string note;
    OrderStatus status;
    double total;
    int createdBy;
    std::string createdAt;
    std::string updatedAt;
    std::vector<OrderItemEntity> items;
};

#endif
