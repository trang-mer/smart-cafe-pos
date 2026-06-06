#ifndef TABLE_MANAGER_H
#define TABLE_MANAGER_H

#include "entity/TableEntity.h"
#include "nlohmann/json.hpp"

#include <vector>
#include <mutex>
#include <string>

using json = nlohmann::json;

class TableManager {
public:
    TableManager();
    void initializeDefaultTables();

    json getAllTables();
    json getTableById(int id);
    bool updateTableStatus(int id, const std::string& status);
    bool updateTableOrder(int id, int orderId);
    bool clearTableOrder(int id);

    static std::string statusToString(const std::string& status);

private:
    json tableEntityToJson(const TableEntity& table);
};

#endif
