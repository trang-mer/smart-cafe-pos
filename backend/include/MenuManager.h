#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include "entity/MenuItemEntity.h"
#include "nlohmann/json.hpp"

#include <vector>
#include <mutex>
#include <string>

using json = nlohmann::json;

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

    static std::string categoryToString(const std::string& category);

private:
    json itemEntityToJson(const MenuItemEntity& item);
};

#endif
