#include "TableManager.h"
#include "Database.h"
#include "logger.h"

TableManager::TableManager() {
    initializeDefaultTables();
}

void TableManager::initializeDefaultTables() {
    Database& db = Database::getInstance();
    
    std::vector<TableEntity> tables = db.getAllTables();
    
    if (tables.empty()) {
        for (int i = 1; i <= 10; i++) {
            TableEntity table;
            table.name = "Bàn " + std::to_string(i);
            table.status = "available";
            table.currentOrderId = -1;
            db.insertTable(table);
        }
        Logger::info("Default tables initialized: 10 tables");
    } else {
        Logger::info("Tables loaded from database: " + std::to_string(tables.size()) + " tables");
    }
}

json TableManager::getAllTables() {
    Database& db = Database::getInstance();
    std::vector<TableEntity> tables = db.getAllTables();
    
    json result = json::array();
    for (const auto& table : tables) {
        result.push_back(tableEntityToJson(table));
    }
    return result;
}

json TableManager::getTableById(int id) {
    Database& db = Database::getInstance();
    TableEntity table = db.getTableById(id);
    
    if (table.id <= 0) {
        return json{{"error", "Table not found"}};
    }
    return tableEntityToJson(table);
}

bool TableManager::updateTableStatus(int id, const std::string& status) {
    Database& db = Database::getInstance();
    TableEntity table = db.getTableById(id);
    
    if (table.id <= 0) return false;
    
    table.status = status;
    return db.updateTable(id, table);
}

bool TableManager::updateTableOrder(int id, int orderId) {
    Database& db = Database::getInstance();
    TableEntity table = db.getTableById(id);
    
    if (table.id <= 0) return false;
    
    table.currentOrderId = orderId;
    table.status = "occupied";
    return db.updateTable(id, table);
}

bool TableManager::clearTableOrder(int id) {
    Database& db = Database::getInstance();
    TableEntity table = db.getTableById(id);
    
    if (table.id <= 0) return false;
    
    table.currentOrderId = -1;
    table.status = "available";
    return db.updateTable(id, table);
}

std::string TableManager::statusToString(const std::string& status) {
    return status;
}

json TableManager::tableEntityToJson(const TableEntity& table) {
    return {
        {"id", table.id},
        {"name", table.name},
        {"status", table.status},
        {"currentOrderId", table.currentOrderId}
    };
}
