#ifndef INVENTORY_MANAGER_H
#define INVENTORY_MANAGER_H

#include "entity/InventoryEntity.h"
#include "nlohmann/json.hpp"

#include <vector>
#include <mutex>
#include <string>

using json = nlohmann::json;

class InventoryManager {
public:
    InventoryManager();
    void initializeDefaultIngredients();

    json getAllIngredients();
    json getIngredientById(int id);
    json getIngredientsByCategory(const std::string& category);
    bool updateIngredient(int id, const json& data);
    int addIngredient(const json& data);
    bool deleteIngredient(int id);

    bool importStock(int id, double quantity, double unitPrice, const std::string& note);
    bool exportStock(int id, double quantity, const std::string& note);
    json getLowStockIngredients();
    json getInventoryTransactions(int ingredientId);

private:
    json ingredientToJson(const IngredientEntity& ingredient);
    json transactionToJson(const InventoryTransactionEntity& transaction);
};

#endif
