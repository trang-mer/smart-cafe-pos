#ifndef STATS_ENTITY_H
#define STATS_ENTITY_H

#include <string>
#include <vector>

struct DailyStatEntity {
    int id;
    std::string date;
    double revenue;
    int ordersCount;
    std::string createdAt;
    std::string updatedAt;
};

struct HourlyStatEntity {
    int id;
    std::string date;
    int hour;
    double revenue;
    int ordersCount;
    std::string createdAt;
    std::string updatedAt;
};

struct ItemStatEntity {
    int id;
    std::string date;
    std::string itemName;
    int quantity;
    std::string createdAt;
    std::string updatedAt;
};

#endif
