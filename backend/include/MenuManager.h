#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include "nlohmann/json.hpp"
#include <vector>
#include <mutex>
#include <string>

using json = nlohmann::json;

enum class MenuCategory {
    COFFEE,
    TEA,
    BLENDED,
    DRINKS,
    CAKE
};

struct MenuItem {
    int id;
    std::string name;
    double price;
    MenuCategory category;
    std::string emoji;
    std::string bgClass;
    bool available;
};

class MenuManager {
public:
    MenuManager();
    void initializeDefaultMenu();

    json getAllItems();
    json getItemsByCategory(const std::string& category);
    json getItemById(int id);
    bool updateItem(int id, const json& data);
    bool addItem(const json& data);
    bool deleteItem(int id);
    bool toggleAvailability(int id);

    static std::string categoryToString(MenuCategory cat);
    static MenuCategory stringToCategory(const std::string& str);

private:
    std::vector<MenuItem> items;
    int nextId;
    std::mutex mutex;
    json itemToJson(const MenuItem& item);
};

#endif
