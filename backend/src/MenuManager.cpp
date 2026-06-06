#include "MenuManager.h"
#include "logger.h"

#include <algorithm>

MenuManager::MenuManager() : nextId(1) {
    initializeDefaultMenu();
}

void MenuManager::initializeDefaultMenu() {
    std::lock_guard<std::mutex> lock(mutex);

    items = {
        {nextId++, "Espresso", 25000, MenuCategory::COFFEE, "☕", "from-amber-900 to-amber-700", true},
        {nextId++, "Americano", 30000, MenuCategory::COFFEE, "☕", "from-stone-800 to-stone-600", true},
        {nextId++, "Latte", 40000, MenuCategory::COFFEE, "🥛", "from-amber-200 to-amber-400", true},
        {nextId++, "Cappuccino", 40000, MenuCategory::COFFEE, "☕", "from-amber-300 to-amber-500", true},
        {nextId++, "Mocha", 45000, MenuCategory::COFFEE, "🍫", "from-amber-800 to-amber-600", true},
        {nextId++, "Caramel Macchiato", 45000, MenuCategory::COFFEE, "🍮", "from-orange-300 to-amber-500", true},
        {nextId++, "Matcha Latte", 40000, MenuCategory::TEA, "🍵", "from-green-300 to-green-500", true},
        {nextId++, "Trà đào", 35000, MenuCategory::TEA, "🍑", "from-orange-200 to-pink-300", true},
        {nextId++, "Trà vải", 35000, MenuCategory::TEA, "🍒", "from-pink-200 to-rose-300", true},
        {nextId++, "Chocolate đá xay", 50000, MenuCategory::BLENDED, "🍫", "from-amber-900 to-amber-700", true},
        {nextId++, "Cookies", 20000, MenuCategory::CAKE, "🍪", "from-amber-300 to-amber-500", true},
        {nextId++, "Tiramisu", 45000, MenuCategory::CAKE, "🍰", "from-amber-200 to-amber-400", true},
    };

    Logger::info("Menu initialized with " + std::to_string(items.size()) + " items");
}

json MenuManager::getAllItems() {
    std::lock_guard<std::mutex> lock(mutex);
    json result = json::array();
    for (const auto& item : items) {
        result.push_back(itemToJson(item));
    }
    return result;
}

json MenuManager::getItemsByCategory(const std::string& category) {
    std::lock_guard<std::mutex> lock(mutex);
    json result = json::array();
    MenuCategory cat = stringToCategory(category);
    for (const auto& item : items) {
        if (item.category == cat) {
            result.push_back(itemToJson(item));
        }
    }
    return result;
}

json MenuManager::getItemById(int id) {
    std::lock_guard<std::mutex> lock(mutex);
    for (const auto& item : items) {
        if (item.id == id) {
            return itemToJson(item);
        }
    }
    return json{{"error", "Item not found"}};
}

bool MenuManager::updateItem(int id, const json& data) {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& item : items) {
        if (item.id == id) {
            if (data.contains("name")) item.name = data["name"];
            if (data.contains("price")) item.price = data["price"];
            if (data.contains("category")) item.category = stringToCategory(data["category"]);
            if (data.contains("emoji")) item.emoji = data["emoji"];
            if (data.contains("bgClass")) item.bgClass = data["bgClass"];
            if (data.contains("available")) item.available = data["available"];
            return true;
        }
    }
    return false;
}

bool MenuManager::addItem(const json& data) {
    std::lock_guard<std::mutex> lock(mutex);
    MenuItem item;
    item.id = nextId++;
    item.name = data.value("name", "New Item");
    item.price = data.value("price", 0);
    item.category = stringToCategory(data.value("category", "coffee"));
    item.emoji = data.value("emoji", "☕");
    item.bgClass = data.value("bgClass", "from-amber-900 to-amber-700");
    item.available = data.value("available", true);
    items.push_back(item);
    return true;
}

bool MenuManager::deleteItem(int id) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = std::remove_if(items.begin(), items.end(), [id](const MenuItem& item) {
        return item.id == id;
    });
    if (it != items.end()) {
        items.erase(it);
        return true;
    }
    return false;
}

bool MenuManager::toggleAvailability(int id) {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& item : items) {
        if (item.id == id) {
            item.available = !item.available;
            return true;
        }
    }
    return false;
}

std::string MenuManager::categoryToString(MenuCategory cat) {
    switch (cat) {
        case MenuCategory::COFFEE: return "coffee";
        case MenuCategory::TEA: return "tea";
        case MenuCategory::BLENDED: return "blended";
        case MenuCategory::DRINKS: return "drinks";
        case MenuCategory::CAKE: return "cake";
        default: return "coffee";
    }
}

MenuCategory MenuManager::stringToCategory(const std::string& str) {
    if (str == "coffee") return MenuCategory::COFFEE;
    if (str == "tea") return MenuCategory::TEA;
    if (str == "blended") return MenuCategory::BLENDED;
    if (str == "drinks") return MenuCategory::DRINKS;
    if (str == "cake") return MenuCategory::CAKE;
    return MenuCategory::COFFEE;
}

json MenuManager::itemToJson(const MenuItem& item) {
    return {
        {"id", item.id},
        {"name", item.name},
        {"price", item.price},
        {"category", categoryToString(item.category)},
        {"emoji", item.emoji},
        {"bg", item.bgClass},
        {"available", item.available}
    };
}
