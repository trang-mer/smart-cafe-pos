#include "MenuManager.h"
#include "Database.h"
#include "logger.h"

#include <algorithm>

MenuManager::MenuManager() {
    initializeDefaultMenu();
}

void MenuManager::initializeDefaultMenu() {
    Database& db = Database::getInstance();
    
    std::vector<MenuItemEntity> items = db.getAllMenuItems();
    
    if (items.empty()) {
        std::vector<MenuItemEntity> defaultItems = {
            {0, "Espresso", 25000, "coffee", "☕", "from-amber-900 to-amber-700", true, "", ""},
            {0, "Americano", 30000, "coffee", "☕", "from-stone-800 to-stone-600", true, "", ""},
            {0, "Latte", 40000, "coffee", "🥛", "from-amber-200 to-amber-400", true, "", ""},
            {0, "Cappuccino", 40000, "coffee", "☕", "from-amber-300 to-amber-500", true, "", ""},
            {0, "Mocha", 45000, "coffee", "🍫", "from-amber-800 to-amber-600", true, "", ""},
            {0, "Caramel Macchiato", 45000, "coffee", "🍮", "from-orange-300 to-amber-500", true, "", ""},
            {0, "Matcha Latte", 40000, "tea", "🍵", "from-green-300 to-green-500", true, "", ""},
            {0, "Trà đào", 35000, "tea", "🍑", "from-orange-200 to-pink-300", true, "", ""},
            {0, "Trà vải", 35000, "tea", "🍒", "from-pink-200 to-rose-300", true, "", ""},
            {0, "Chocolate đá xay", 50000, "blended", "🍫", "from-amber-900 to-amber-700", true, "", ""},
            {0, "Cookies", 20000, "cake", "🍪", "from-amber-300 to-amber-500", true, "", ""},
            {0, "Tiramisu", 45000, "cake", "🍰", "from-amber-200 to-amber-400", true, "", ""},
        };
        
        for (const auto& item : defaultItems) {
            db.insertMenuItem(item);
        }
        
        Logger::info("Default menu initialized with " + std::to_string(defaultItems.size()) + " items");
    } else {
        Logger::info("Menu loaded from database: " + std::to_string(items.size()) + " items");
    }
}

json MenuManager::getAllItems() {
    Database& db = Database::getInstance();
    std::vector<MenuItemEntity> items = db.getAllMenuItems();
    
    json result = json::array();
    for (const auto& item : items) {
        result.push_back(itemEntityToJson(item));
    }
    return result;
}

json MenuManager::getItemsByCategory(const std::string& category) {
    Database& db = Database::getInstance();
    std::vector<MenuItemEntity> items = db.getAllMenuItems();
    
    json result = json::array();
    for (const auto& item : items) {
        if (item.category == category) {
            result.push_back(itemEntityToJson(item));
        }
    }
    return result;
}

json MenuManager::getItemById(int id) {
    Database& db = Database::getInstance();
    MenuItemEntity item = db.getMenuItemById(id);
    
    if (item.id <= 0) {
        return json{{"error", "Item not found"}};
    }
    return itemEntityToJson(item);
}

bool MenuManager::updateItem(int id, const json& data) {
    Database& db = Database::getInstance();
    MenuItemEntity item = db.getMenuItemById(id);
    
    if (item.id <= 0) return false;
    
    if (data.contains("name")) item.name = data["name"];
    if (data.contains("price")) item.price = data["price"];
    if (data.contains("category")) item.category = data["category"];
    if (data.contains("emoji")) item.emoji = data["emoji"];
    if (data.contains("bgClass")) item.bgClass = data["bgClass"];
    if (data.contains("available")) item.available = data["available"];
    
    return db.updateMenuItem(id, item);
}

bool MenuManager::addItem(const json& data) {
    Database& db = Database::getInstance();
    
    MenuItemEntity item;
    item.name = data.value("name", "New Item");
    item.price = data.value("price", 0);
    item.category = data.value("category", "coffee");
    item.emoji = data.value("emoji", "☕");
    item.bgClass = data.value("bgClass", "from-amber-900 to-amber-700");
    item.available = data.value("available", true);
    
    return db.insertMenuItem(item);
}

bool MenuManager::deleteItem(int id) {
    Database& db = Database::getInstance();
    return db.deleteMenuItem(id);
}

bool MenuManager::toggleAvailability(int id) {
    Database& db = Database::getInstance();
    MenuItemEntity item = db.getMenuItemById(id);
    
    if (item.id <= 0) return false;
    
    item.available = !item.available;
    return db.updateMenuItem(id, item);
}

std::string MenuManager::categoryToString(const std::string& category) {
    return category;
}

json MenuManager::itemEntityToJson(const MenuItemEntity& item) {
    return {
        {"id", item.id},
        {"name", item.name},
        {"price", item.price},
        {"category", item.category},
        {"emoji", item.emoji},
        {"bg", item.bgClass},
        {"available", item.available}
    };
}
