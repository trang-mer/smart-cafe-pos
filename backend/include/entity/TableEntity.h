#ifndef TABLE_ENTITY_H
#define TABLE_ENTITY_H

#include <string>

struct TableEntity {
    int id;
    std::string name;
    std::string status;
    int currentOrderId;
    std::string createdAt;
    std::string updatedAt;
};

#endif
