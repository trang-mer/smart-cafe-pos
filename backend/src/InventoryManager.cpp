#include "InventoryManager.h"
#include "Database.h"
#include "logger.h"

InventoryManager::InventoryManager() {
    initializeDefaultIngredients();
}

void InventoryManager::initializeDefaultIngredients() {
    Database& db = Database::getInstance();
    
    std::vector<IngredientEntity> ingredients = db.getAllIngredients();
    
    if (ingredients.empty()) {
        std::vector<IngredientEntity> defaultIngredients = {
            {0, "Cà phê hạt", "kg", 10, 2, 120000, "coffee", "", ""},
            {0, "Sữa tươi", "lít", 20, 5, 25000, "dairy", "", ""},
            {0, "Đường", "kg", 15, 3, 18000, "sweetener", "", ""},
            {0, "Trà", "g", 500, 100, 500, "tea", "", ""},
            {0, "Matcha", "g", 200, 50, 800, "tea", "", ""},
            {0, "Siro caramel", "ml", 1000, 200, 150, "syrup", "", ""},
            {0, "Siro vanilla", "ml", 800, 200, 150, "syrup", "", ""},
            {0, "Bột cacao", "g", 500, 100, 400, "additive", "", ""},
            {0, "Whipping cream", "lít", 5, 1, 80000, "dairy", "", ""},
            {0, "Đá viên", "kg", 50, 10, 5000, "general", "", ""},
            {0, "Ly nhựa", "cái", 500, 100, 2000, "packaging", "", ""},
            {0, "Ống hút", "cái", 1000, 200, 500, "packaging", "", ""},
        };
        
        for (const auto& ingredient : defaultIngredients) {
            db.insertIngredient(ingredient);
        }
        
        Logger::info("Default ingredients initialized with " + std::to_string(defaultIngredients.size()) + " items");
    } else {
        Logger::info("Ingredients loaded from database: " + std::to_string(ingredients.size()) + " items");
    }
}

json InventoryManager::getAllIngredients() {
    Database& db = Database::getInstance();
    std::vector<IngredientEntity> ingredients = db.getAllIngredients();
    
    json result = json::array();
    for (const auto& ingredient : ingredients) {
        result.push_back(ingredientToJson(ingredient));
    }
    return result;
}

json InventoryManager::getIngredientById(int id) {
    Database& db = Database::getInstance();
    IngredientEntity ingredient = db.getIngredientById(id);
    
    if (ingredient.id <= 0) {
        return json{{"error", "Ingredient not found"}};
    }
    return ingredientToJson(ingredient);
}

json InventoryManager::getIngredientsByCategory(const std::string& category) {
    Database& db = Database::getInstance();
    std::vector<IngredientEntity> ingredients = db.getAllIngredients();
    
    json result = json::array();
    for (const auto& ingredient : ingredients) {
        if (ingredient.category == category) {
            result.push_back(ingredientToJson(ingredient));
        }
    }
    return result;
}

bool InventoryManager::updateIngredient(int id, const json& data) {
    Database& db = Database::getInstance();
    IngredientEntity ingredient = db.getIngredientById(id);
    
    if (ingredient.id <= 0) return false;
    
    if (data.contains("name")) ingredient.name = data["name"];
    if (data.contains("unit")) ingredient.unit = data["unit"];
    if (data.contains("quantity")) ingredient.quantity = data["quantity"];
    if (data.contains("minStock")) ingredient.minStock = data["minStock"];
    if (data.contains("costPerUnit")) ingredient.costPerUnit = data["costPerUnit"];
    if (data.contains("category")) ingredient.category = data["category"];
    
    return db.updateIngredient(id, ingredient);
}

int InventoryManager::addIngredient(const json& data) {
    Database& db = Database::getInstance();
    
    IngredientEntity ingredient;
    ingredient.name = data.value("name", "New Ingredient");
    ingredient.unit = data.value("unit", "pcs");
    ingredient.quantity = data.value("quantity", 0);
    ingredient.minStock = data.value("minStock", 0);
    ingredient.costPerUnit = data.value("costPerUnit", 0);
    ingredient.category = data.value("category", "general");
    
    if (db.insertIngredient(ingredient)) {
        return db.getAllIngredients().back().id;
    }
    return -1;
}

bool InventoryManager::deleteIngredient(int id) {
    Database& db = Database::getInstance();
    return db.deleteIngredient(id);
}

bool InventoryManager::importStock(int id, double quantity, double unitPrice, const std::string& note) {
    Database& db = Database::getInstance();
    IngredientEntity ingredient = db.getIngredientById(id);
    
    if (ingredient.id <= 0) return false;
    
    double newQuantity = ingredient.quantity + quantity;
    db.updateIngredientQuantity(id, newQuantity);
    
    InventoryTransactionEntity transaction;
    transaction.ingredientId = id;
    transaction.type = "import";
    transaction.quantity = quantity;
    transaction.unitPrice = unitPrice;
    transaction.note = note;
    db.insertInventoryTransaction(transaction);
    
    return true;
}

bool InventoryManager::exportStock(int id, double quantity, const std::string& note) {
    Database& db = Database::getInstance();
    IngredientEntity ingredient = db.getIngredientById(id);
    
    if (ingredient.id <= 0) return false;
    if (ingredient.quantity < quantity) return false;
    
    double newQuantity = ingredient.quantity - quantity;
    db.updateIngredientQuantity(id, newQuantity);
    
    InventoryTransactionEntity transaction;
    transaction.ingredientId = id;
    transaction.type = "export";
    transaction.quantity = quantity;
    transaction.unitPrice = ingredient.costPerUnit;
    transaction.note = note;
    db.insertInventoryTransaction(transaction);
    
    return true;
}

json InventoryManager::getLowStockIngredients() {
    Database& db = Database::getInstance();
    std::vector<IngredientEntity> ingredients = db.getAllIngredients();
    
    json result = json::array();
    for (const auto& ingredient : ingredients) {
        if (ingredient.quantity <= ingredient.minStock) {
            json item = ingredientToJson(ingredient);
            item["lowStock"] = true;
            item["deficit"] = ingredient.minStock - ingredient.quantity;
            result.push_back(item);
        }
    }
    return result;
}

json InventoryManager::getInventoryTransactions(int ingredientId) {
    Database& db = Database::getInstance();
    std::vector<InventoryTransactionEntity> transactions = db.getInventoryTransactions(ingredientId);
    
    json result = json::array();
    for (const auto& transaction : transactions) {
        result.push_back(transactionToJson(transaction));
    }
    return result;
}

json InventoryManager::ingredientToJson(const IngredientEntity& ingredient) {
    return {
        {"id", ingredient.id},
        {"name", ingredient.name},
        {"unit", ingredient.unit},
        {"quantity", ingredient.quantity},
        {"minStock", ingredient.minStock},
        {"costPerUnit", ingredient.costPerUnit},
        {"category", ingredient.category},
        {"lowStock", ingredient.quantity <= ingredient.minStock}
    };
}

json InventoryManager::transactionToJson(const InventoryTransactionEntity& transaction) {
    return {
        {"id", transaction.id},
        {"ingredientId", transaction.ingredientId},
        {"type", transaction.type},
        {"quantity", transaction.quantity},
        {"unitPrice", transaction.unitPrice},
        {"note", transaction.note},
        {"createdAt", transaction.createdAt}
    };
}
