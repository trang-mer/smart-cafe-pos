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

struct OrderItem {
    std::string name;
    int qty;
    double price;
};

struct Order {
    int id;
    int tableNumber;
    std::vector<OrderItem> items;
    std::string note;
    OrderStatus status;
    std::string createdAt;
};

#endif