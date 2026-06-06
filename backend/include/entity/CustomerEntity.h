#ifndef CUSTOMER_ENTITY_H
#define CUSTOMER_ENTITY_H

#include <string>

struct CustomerEntity {
    int id;
    std::string name;
    std::string phone;
    std::string email;
    int visits;
    double totalSpent;
    std::string createdAt;
    std::string lastVisit;
};

#endif
