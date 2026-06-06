#include "TableManager.h"
#include "logger.h"

TableManager::TableManager() {
    initializeDefaultTables();
}

void TableManager::initializeDefaultTables() {
    std::lock_guard<std::mutex> lock(mutex);
    for (int i = 1; i <= 10; i++) {
        Table table;
        table.id = i;
        table.name = "Bàn " + std::to_string(i);
        table.status = TableStatus::AVAILABLE;
        table.currentOrderId = -1;
        tables.push_back(table);
    }
    Logger::info("Tables initialized: " + std::to_string(tables.size()) + " tables");
}

json TableManager::getAllTables() {
    std::lock_guard<std::mutex> lock(mutex);
    json result = json::array();
    for (const auto& table : tables) {
        result.push_back(tableToJson(table));
    }
    return result;
}

json TableManager::getTableById(int id) {
    std::lock_guard<std::mutex> lock(mutex);
    for (const auto& table : tables) {
        if (table.id == id) {
            return tableToJson(table);
        }
    }
    return json{{"error", "Table not found"}};
}

bool TableManager::updateTableStatus(int id, TableStatus status) {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& table : tables) {
        if (table.id == id) {
            table.status = status;
            if (status != TableStatus::OCCUPIED) {
                table.currentOrderId = -1;
            }
            return true;
        }
    }
    return false;
}

bool TableManager::updateTableOrder(int id, int orderId) {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& table : tables) {
        if (table.id == id) {
            table.currentOrderId = orderId;
            table.status = TableStatus::OCCUPIED;
            return true;
        }
    }
    return false;
}

bool TableManager::clearTableOrder(int id) {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& table : tables) {
        if (table.id == id) {
            table.currentOrderId = -1;
            table.status = TableStatus::AVAILABLE;
            return true;
        }
    }
    return false;
}

std::string TableManager::statusToString(TableStatus status) {
    switch (status) {
        case TableStatus::AVAILABLE: return "available";
        case TableStatus::OCCUPIED: return "occupied";
        case TableStatus::RESERVED: return "reserved";
        default: return "available";
    }
}

TableStatus TableManager::stringToStatus(const std::string& str) {
    if (str == "available") return TableStatus::AVAILABLE;
    if (str == "occupied") return TableStatus::OCCUPIED;
    if (str == "reserved") return TableStatus::RESERVED;
    return TableStatus::AVAILABLE;
}

json TableManager::tableToJson(const Table& table) {
    return {
        {"id", table.id},
        {"name", table.name},
        {"status", statusToString(table.status)},
        {"currentOrderId", table.currentOrderId}
    };
}
