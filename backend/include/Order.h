#ifndef ORDER_H
#define ORDER_H

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

struct OrderItem {
    std::string name;
    int qty;
    double price;
};

struct Order {
    int id;
    int tableNumber;
    OrderPriority priority;
    std::vector<OrderItem> items;
    std::string note;
    OrderStatus status;
    std::string createdAt;
    int createdBy;
};

#endif
