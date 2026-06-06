#ifndef TABLE_MANAGER_H
#define TABLE_MANAGER_H

#include "nlohmann/json.hpp"
#include <vector>
#include <mutex>
#include <string>

using json = nlohmann::json;

enum class TableStatus {
    AVAILABLE,
    OCCUPIED,
    RESERVED
};

struct Table {
    int id;
    std::string name;
    TableStatus status;
    int currentOrderId;
};

class TableManager {
public:
    TableManager();
    void initializeDefaultTables();

    json getAllTables();
    json getTableById(int id);
    bool updateTableStatus(int id, TableStatus status);
    bool updateTableOrder(int id, int orderId);
    bool clearTableOrder(int id);

    static std::string statusToString(TableStatus status);
    static TableStatus stringToStatus(const std::string& str);

private:
    std::vector<Table> tables;
    std::mutex mutex;
    json tableToJson(const Table& table);
};

#endif
