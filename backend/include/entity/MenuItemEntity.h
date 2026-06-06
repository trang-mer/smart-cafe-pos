#ifndef MENU_ITEM_ENTITY_H
#define MENU_ITEM_ENTITY_H

#include <string>

struct MenuItemEntity {
    int id;
    std::string name;
    double price;
    std::string category;
    std::string emoji;
    std::string bgClass;
    bool available;
    std::string createdAt;
    std::string updatedAt;
};

#endif
