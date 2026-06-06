#ifndef INVENTORY_ENTITY_H
#define INVENTORY_ENTITY_H

#include <string>

struct IngredientEntity {
    int id;
    std::string name;
    std::string unit;
    double quantity;
    double minStock;
    double costPerUnit;
    std::string category;
    std::string createdAt;
    std::string updatedAt;
};

struct InventoryTransactionEntity {
    int id;
    int ingredientId;
    std::string type;
    double quantity;
    double unitPrice;
    std::string note;
    std::string createdAt;
};

#endif
