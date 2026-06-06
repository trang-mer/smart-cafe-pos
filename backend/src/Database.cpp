#include "Database.h"
#include "logger.h"

#include <sstream>
#include <iomanip>
#include <algorithm>

Database::Database() : conn(nullptr), connected(false) {}

Database::~Database() {
    disconnect();
}

Database& Database::getInstance() {
    static Database instance;
    return instance;
}

bool Database::connect(const std::string& conninfo) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (connected && conn) {
        disconnect();
    }

    conn = PQconnectdb(conninfo.c_str());

    if (PQstatus(conn) != CONNECTION_OK) {
        Logger::error("Database connection failed: " + std::string(PQerrorMessage(conn)));
        PQfinish(conn);
        conn = nullptr;
        connected = false;
        return false;
    }

    connected = true;
    Logger::info("Database connected successfully");
    return true;
}

void Database::disconnect() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (conn) {
        PQfinish(conn);
        conn = nullptr;
    }
    connected = false;
    Logger::info("Database disconnected");
}

bool Database::isConnected() const {
    return connected && conn != nullptr;
}

void Database::execute(const std::string& sql) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!isConnected()) {
        Logger::error("Cannot execute: not connected to database");
        return;
    }

    PGresult* res = PQexec(conn, sql.c_str());
    
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        Logger::error("Execute failed: " + std::string(PQerrorMessage(conn)));
    }
    
    PQclear(res);
}

PGresult* Database::query(const std::string& sql) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!isConnected()) {
        Logger::error("Cannot query: not connected to database");
        return nullptr;
    }

    PGresult* res = PQexec(conn, sql.c_str());
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        Logger::error("Query failed: " + std::string(PQerrorMessage(conn)));
        PQclear(res);
        return nullptr;
    }
    
    return res;
}

std::vector<MenuItemEntity> Database::getAllMenuItems() {
    std::vector<MenuItemEntity> items;
    
    PGresult* res = query("SELECT id, name, price, category, emoji, bg_class, available, created_at, updated_at FROM menu_items ORDER BY id");
    
    if (!res) return items;
    
    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        MenuItemEntity item;
        item.id = std::stoi(PQgetvalue(res, i, 0));
        item.name = PQgetvalue(res, i, 1);
        item.price = std::stod(PQgetvalue(res, i, 2));
        item.category = PQgetvalue(res, i, 3);
        item.emoji = PQgetvalue(res, i, 4);
        item.bgClass = PQgetvalue(res, i, 5);
        item.available = std::string(PQgetvalue(res, i, 6)) == "t";
        item.createdAt = PQgetvalue(res, i, 7);
        item.updatedAt = PQgetvalue(res, i, 8);
        items.push_back(item);
    }
    
    PQclear(res);
    return items;
}

MenuItemEntity Database::getMenuItemById(int id) {
    MenuItemEntity item;
    item.id = -1;
    
    std::string sql = "SELECT id, name, price, category, emoji, bg_class, available, created_at, updated_at FROM menu_items WHERE id = " + std::to_string(id);
    PGresult* res = query(sql);
    
    if (!res || PQntuples(res) == 0) {
        if (res) PQclear(res);
        return item;
    }
    
    item.id = std::stoi(PQgetvalue(res, 0, 0));
    item.name = PQgetvalue(res, 0, 1);
    item.price = std::stod(PQgetvalue(res, 0, 2));
    item.category = PQgetvalue(res, 0, 3);
    item.emoji = PQgetvalue(res, 0, 4);
    item.bgClass = PQgetvalue(res, 0, 5);
    item.available = std::string(PQgetvalue(res, 0, 6)) == "t";
    item.createdAt = PQgetvalue(res, 0, 7);
    item.updatedAt = PQgetvalue(res, 0, 8);
    
    PQclear(res);
    return item;
}

bool Database::insertMenuItem(const MenuItemEntity& item) {
    std::ostringstream sql;
    sql << "INSERT INTO menu_items (name, price, category, emoji, bg_class, available) VALUES ("
        << "'" << escapeString(item.name) << "', "
        << item.price << ", "
        << "'" << escapeString(item.category) << "', "
        << "'" << escapeString(item.emoji) << "', "
        << "'" << escapeString(item.bgClass) << "', "
        << (item.available ? "TRUE" : "FALSE") << ")";
    
    execute(sql.str());
    return true;
}

bool Database::updateMenuItem(int id, const MenuItemEntity& item) {
    std::ostringstream sql;
    sql << "UPDATE menu_items SET "
        << "name = '" << escapeString(item.name) << "', "
        << "price = " << item.price << ", "
        << "category = '" << escapeString(item.category) << "', "
        << "emoji = '" << escapeString(item.emoji) << "', "
        << "bg_class = '" << escapeString(item.bgClass) << "', "
        << "available = " << (item.available ? "TRUE" : "FALSE") << ", "
        << "updated_at = CURRENT_TIMESTAMP "
        << "WHERE id = " << id;
    
    execute(sql.str());
    return true;
}

bool Database::deleteMenuItem(int id) {
    std::string sql = "DELETE FROM menu_items WHERE id = " + std::to_string(id);
    execute(sql);
    return true;
}

std::vector<TableEntity> Database::getAllTables() {
    std::vector<TableEntity> tables;
    
    PGresult* res = query("SELECT id, name, status, current_order_id, created_at, updated_at FROM tables ORDER BY id");
    
    if (!res) return tables;
    
    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        TableEntity table;
        table.id = std::stoi(PQgetvalue(res, i, 0));
        table.name = PQgetvalue(res, i, 1);
        table.status = PQgetvalue(res, i, 2);
        table.currentOrderId = PQgetisnull(res, i, 3) ? -1 : std::stoi(PQgetvalue(res, i, 3));
        table.createdAt = PQgetvalue(res, i, 4);
        table.updatedAt = PQgetvalue(res, i, 5);
        tables.push_back(table);
    }
    
    PQclear(res);
    return tables;
}

TableEntity Database::getTableById(int id) {
    TableEntity table;
    table.id = -1;
    
    std::string sql = "SELECT id, name, status, current_order_id, created_at, updated_at FROM tables WHERE id = " + std::to_string(id);
    PGresult* res = query(sql);
    
    if (!res || PQntuples(res) == 0) {
        if (res) PQclear(res);
        return table;
    }
    
    table.id = std::stoi(PQgetvalue(res, 0, 0));
    table.name = PQgetvalue(res, 0, 1);
    table.status = PQgetvalue(res, 0, 2);
    table.currentOrderId = PQgetisnull(res, 0, 3) ? -1 : std::stoi(PQgetvalue(res, 0, 3));
    table.createdAt = PQgetvalue(res, 0, 4);
    table.updatedAt = PQgetvalue(res, 0, 5);
    
    PQclear(res);
    return table;
}

bool Database::insertTable(const TableEntity& table) {
    std::ostringstream sql;
    sql << "INSERT INTO tables (name, status) VALUES ("
        << "'" << escapeString(table.name) << "', "
        << "'" << table.status << "')";
    
    execute(sql.str());
    return true;
}

bool Database::updateTable(int id, const TableEntity& table) {
    std::ostringstream sql;
    sql << "UPDATE tables SET "
        << "name = '" << escapeString(table.name) << "', "
        << "status = '" << table.status << "', "
        << "current_order_id = " << (table.currentOrderId > 0 ? std::to_string(table.currentOrderId) : "NULL") << ", "
        << "updated_at = CURRENT_TIMESTAMP "
        << "WHERE id = " << id;
    
    execute(sql.str());
    return true;
}

std::vector<CustomerEntity> Database::getAllCustomers() {
    std::vector<CustomerEntity> customers;
    
    PGresult* res = query("SELECT id, name, phone, email, visits, total_spent, created_at, last_visit FROM customers ORDER BY id");
    
    if (!res) return customers;
    
    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        CustomerEntity customer;
        customer.id = std::stoi(PQgetvalue(res, i, 0));
        customer.name = PQgetvalue(res, i, 1);
        customer.phone = PQgetvalue(res, i, 2);
        customer.email = PQgetvalue(res, i, 3);
        customer.visits = std::stoi(PQgetvalue(res, i, 4));
        customer.totalSpent = std::stod(PQgetvalue(res, i, 5));
        customer.createdAt = PQgetvalue(res, i, 6);
        customer.lastVisit = PQgetvalue(res, i, 7);
        customers.push_back(customer);
    }
    
    PQclear(res);
    return customers;
}

CustomerEntity Database::getCustomerById(int id) {
    CustomerEntity customer;
    customer.id = -1;
    
    std::string sql = "SELECT id, name, phone, email, visits, total_spent, created_at, last_visit FROM customers WHERE id = " + std::to_string(id);
    PGresult* res = query(sql);
    
    if (!res || PQntuples(res) == 0) {
        if (res) PQclear(res);
        return customer;
    }
    
    customer.id = std::stoi(PQgetvalue(res, 0, 0));
    customer.name = PQgetvalue(res, 0, 1);
    customer.phone = PQgetvalue(res, 0, 2);
    customer.email = PQgetvalue(res, 0, 3);
    customer.visits = std::stoi(PQgetvalue(res, 0, 4));
    customer.totalSpent = std::stod(PQgetvalue(res, 0, 5));
    customer.createdAt = PQgetvalue(res, 0, 6);
    customer.lastVisit = PQgetvalue(res, 0, 7);
    
    PQclear(res);
    return customer;
}

std::vector<CustomerEntity> Database::searchCustomers(const std::string& query) {
    std::vector<CustomerEntity> customers;
    
    std::string sql = "SELECT id, name, phone, email, visits, total_spent, created_at, last_visit FROM customers "
                      "WHERE LOWER(name) LIKE LOWER('%" + escapeString(query) + "%') "
                      "OR phone LIKE '%" + escapeString(query) + "%' "
                      "ORDER BY id";
    
    PGresult* res = query(sql);
    
    if (!res) return customers;
    
    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        CustomerEntity customer;
        customer.id = std::stoi(PQgetvalue(res, i, 0));
        customer.name = PQgetvalue(res, i, 1);
        customer.phone = PQgetvalue(res, i, 2);
        customer.email = PQgetvalue(res, i, 3);
        customer.visits = std::stoi(PQgetvalue(res, i, 4));
        customer.totalSpent = std::stod(PQgetvalue(res, i, 5));
        customer.createdAt = PQgetvalue(res, i, 6);
        customer.lastVisit = PQgetvalue(res, i, 7);
        customers.push_back(customer);
    }
    
    PQclear(res);
    return customers;
}

int Database::insertCustomer(const CustomerEntity& customer) {
    std::ostringstream sql;
    sql << "INSERT INTO customers (name, phone, email, visits, total_spent) VALUES ("
        << "'" << escapeString(customer.name) << "', "
        << "'" << escapeString(customer.phone) << "', "
        << "'" << escapeString(customer.email) << "', "
        << customer.visits << ", "
        << customer.totalSpent << ") "
        << "RETURNING id";
    
    PGresult* res = query(sql.str());
    
    if (!res || PQntuples(res) == 0) {
        if (res) PQclear(res);
        return -1;
    }
    
    int id = std::stoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    return id;
}

bool Database::updateCustomer(int id, const CustomerEntity& customer) {
    std::ostringstream sql;
    sql << "UPDATE customers SET "
        << "name = '" << escapeString(customer.name) << "', "
        << "phone = '" << escapeString(customer.phone) << "', "
        << "email = '" << escapeString(customer.email) << "' "
        << "WHERE id = " << id;
    
    execute(sql.str());
    return true;
}

bool Database::deleteCustomer(int id) {
    std::string sql = "DELETE FROM customers WHERE id = " + std::to_string(id);
    execute(sql);
    return true;
}

bool Database::incrementCustomerVisit(int id, double amount) {
    std::ostringstream sql;
    sql << "UPDATE customers SET "
        << "visits = visits + 1, "
        << "total_spent = total_spent + " << amount << ", "
        << "last_visit = CURRENT_TIMESTAMP "
        << "WHERE id = " << id;
    
    execute(sql.str());
    return true;
}

std::vector<OrderEntity> Database::getAllOrders() {
    std::vector<OrderEntity> orders;
    
    PGresult* res = query("SELECT id, table_number, priority, note, status, total, created_by, created_at, updated_at FROM orders ORDER BY id DESC");
    
    if (!res) return orders;
    
    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        OrderEntity order;
        order.id = std::stoi(PQgetvalue(res, i, 0));
        order.tableNumber = std::stoi(PQgetvalue(res, i, 1));
        order.priority = PQgetvalue(res, i, 2);
        order.note = PQgetvalue(res, i, 3);
        order.status = static_cast<OrderStatus>(std::stoi(PQgetvalue(res, i, 4)));
        order.total = std::stod(PQgetvalue(res, i, 5));
        order.createdBy = std::stoi(PQgetvalue(res, i, 6));
        order.createdAt = PQgetvalue(res, i, 7);
        order.updatedAt = PQgetvalue(res, i, 8);
        order.items = getOrderItems(order.id);
        orders.push_back(order);
    }
    
    PQclear(res);
    return orders;
}

OrderEntity Database::getOrderById(int id) {
    OrderEntity order;
    order.id = -1;
    
    std::string sql = "SELECT id, table_number, priority, note, status, total, created_by, created_at, updated_at FROM orders WHERE id = " + std::to_string(id);
    PGresult* res = query(sql);
    
    if (!res || PQntuples(res) == 0) {
        if (res) PQclear(res);
        return order;
    }
    
    order.id = std::stoi(PQgetvalue(res, 0, 0));
    order.tableNumber = std::stoi(PQgetvalue(res, 0, 1));
    order.priority = PQgetvalue(res, 0, 2);
    order.note = PQgetvalue(res, 0, 3);
    order.status = static_cast<OrderStatus>(std::stoi(PQgetvalue(res, 0, 4)));
    order.total = std::stod(PQgetvalue(res, 0, 5));
    order.createdBy = std::stoi(PQgetvalue(res, 0, 6));
    order.createdAt = PQgetvalue(res, 0, 7);
    order.updatedAt = PQgetvalue(res, 0, 8);
    order.items = getOrderItems(order.id);
    
    PQclear(res);
    return order;
}

std::vector<OrderEntity> Database::getOrdersByStatus(OrderStatus status) {
    std::vector<OrderEntity> orders;
    
    std::string sql = "SELECT id, table_number, priority, note, status, total, created_by, created_at, updated_at FROM orders WHERE status = " 
                      + std::to_string(static_cast<int>(status)) + " ORDER BY id DESC";
    
    PGresult* res = query(sql);
    
    if (!res) return orders;
    
    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        OrderEntity order;
        order.id = std::stoi(PQgetvalue(res, i, 0));
        order.tableNumber = std::stoi(PQgetvalue(res, i, 1));
        order.priority = PQgetvalue(res, i, 2);
        order.note = PQgetvalue(res, i, 3);
        order.status = static_cast<OrderStatus>(std::stoi(PQgetvalue(res, i, 4)));
        order.total = std::stod(PQgetvalue(res, i, 5));
        order.createdBy = std::stoi(PQgetvalue(res, i, 6));
        order.createdAt = PQgetvalue(res, i, 7);
        order.updatedAt = PQgetvalue(res, i, 8);
        order.items = getOrderItems(order.id);
        orders.push_back(order);
    }
    
    PQclear(res);
    return orders;
}

std::vector<OrderEntity> Database::getOrdersByTable(int tableNumber) {
    std::vector<OrderEntity> orders;
    
    std::string sql = "SELECT id, table_number, priority, note, status, total, created_by, created_at, updated_at FROM orders WHERE table_number = " 
                      + std::to_string(tableNumber) + " ORDER BY id DESC";
    
    PGresult* res = query(sql);
    
    if (!res) return orders;
    
    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        OrderEntity order;
        order.id = std::stoi(PQgetvalue(res, i, 0));
        order.tableNumber = std::stoi(PQgetvalue(res, i, 1));
        order.priority = PQgetvalue(res, i, 2);
        order.note = PQgetvalue(res, i, 3);
        order.status = static_cast<OrderStatus>(std::stoi(PQgetvalue(res, i, 4)));
        order.total = std::stod(PQgetvalue(res, i, 5));
        order.createdBy = std::stoi(PQgetvalue(res, i, 6));
        order.createdAt = PQgetvalue(res, i, 7);
        order.updatedAt = PQgetvalue(res, i, 8);
        order.items = getOrderItems(order.id);
        orders.push_back(order);
    }
    
    PQclear(res);
    return orders;
}

int Database::insertOrder(const OrderEntity& order) {
    std::ostringstream sql;
    sql << "INSERT INTO orders (table_number, priority, note, status, total, created_by) VALUES ("
        << order.tableNumber << ", "
        << "'" << order.priority << "', "
        << "'" << escapeString(order.note) << "', "
        << static_cast<int>(order.status) << ", "
        << order.total << ", "
        << order.createdBy << ") "
        << "RETURNING id";
    
    PGresult* res = query(sql.str());
    
    if (!res || PQntuples(res) == 0) {
        if (res) PQclear(res);
        return -1;
    }
    
    int id = std::stoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    return id;
}

bool Database::updateOrder(int id, const OrderEntity& order) {
    std::ostringstream sql;
    sql << "UPDATE orders SET "
        << "table_number = " << order.tableNumber << ", "
        << "priority = '" << order.priority << "', "
        << "note = '" << escapeString(order.note) << "', "
        << "status = " << static_cast<int>(order.status) << ", "
        << "total = " << order.total << ", "
        << "updated_at = CURRENT_TIMESTAMP "
        << "WHERE id = " << id;
    
    execute(sql.str());
    return true;
}

bool Database::updateOrderStatus(int id, OrderStatus status) {
    std::ostringstream sql;
    sql << "UPDATE orders SET "
        << "status = " << static_cast<int>(status) << ", "
        << "updated_at = CURRENT_TIMESTAMP "
        << "WHERE id = " << id;
    
    execute(sql.str());
    return true;
}

bool Database::deleteOrder(int id) {
    std::string sql = "DELETE FROM orders WHERE id = " + std::to_string(id);
    execute(sql);
    return true;
}

int Database::insertOrderItem(const OrderItemEntity& item) {
    std::ostringstream sql;
    sql << "INSERT INTO order_items (order_id, menu_item_id, name, qty, price) VALUES ("
        << item.orderId << ", ";
    
    if (item.menuItemId > 0) {
        sql << item.menuItemId << ", ";
    } else {
        sql << "NULL, ";
    }
    
    sql << "'" << escapeString(item.name) << "', "
        << item.qty << ", "
        << item.price << ") "
        << "RETURNING id";
    
    PGresult* res = query(sql.str());
    
    if (!res || PQntuples(res) == 0) {
        if (res) PQclear(res);
        return -1;
    }
    
    int id = std::stoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    return id;
}

std::vector<OrderItemEntity> Database::getOrderItems(int orderId) {
    std::vector<OrderItemEntity> items;
    
    std::string sql = "SELECT id, order_id, menu_item_id, name, qty, price, created_at FROM order_items WHERE order_id = " + std::to_string(orderId);
    PGresult* res = query(sql);
    
    if (!res) return items;
    
    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        OrderItemEntity item;
        item.id = std::stoi(PQgetvalue(res, i, 0));
        item.orderId = std::stoi(PQgetvalue(res, i, 1));
        item.menuItemId = PQgetisnull(res, i, 2) ? 0 : std::stoi(PQgetvalue(res, i, 2));
        item.name = PQgetvalue(res, i, 3);
        item.qty = std::stoi(PQgetvalue(res, i, 4));
        item.price = std::stod(PQgetvalue(res, i, 5));
        item.createdAt = PQgetvalue(res, i, 6);
        items.push_back(item);
    }
    
    PQclear(res);
    return items;
}

std::vector<DailyStatEntity> Database::getDailyStats(const std::string& startDate, const std::string& endDate) {
    std::vector<DailyStatEntity> stats;
    
    std::string sql = "SELECT id, date, revenue, orders_count, created_at, updated_at FROM daily_stats "
                      "WHERE date BETWEEN '" + startDate + "' AND '" + endDate + "' "
                      "ORDER BY date";
    
    PGresult* res = query(sql);
    
    if (!res) return stats;
    
    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        DailyStatEntity stat;
        stat.id = std::stoi(PQgetvalue(res, i, 0));
        stat.date = PQgetvalue(res, i, 1);
        stat.revenue = std::stod(PQgetvalue(res, i, 2));
        stat.ordersCount = std::stoi(PQgetvalue(res, i, 3));
        stat.createdAt = PQgetvalue(res, i, 4);
        stat.updatedAt = PQgetvalue(res, i, 5);
        stats.push_back(stat);
    }
    
    PQclear(res);
    return stats;
}

DailyStatEntity Database::getDailyStatByDate(const std::string& date) {
    DailyStatEntity stat;
    stat.id = -1;
    
    std::string sql = "SELECT id, date, revenue, orders_count, created_at, updated_at FROM daily_stats WHERE date = '" + date + "'";
    PGresult* res = query(sql);
    
    if (!res || PQntuples(res) == 0) {
        if (res) PQclear(res);
        return stat;
    }
    
    stat.id = std::stoi(PQgetvalue(res, 0, 0));
    stat.date = PQgetvalue(res, 0, 1);
    stat.revenue = std::stod(PQgetvalue(res, 0, 2));
    stat.ordersCount = std::stoi(PQgetvalue(res, 0, 3));
    stat.createdAt = PQgetvalue(res, 0, 4);
    stat.updatedAt = PQgetvalue(res, 0, 5);
    
    PQclear(res);
    return stat;
}

bool Database::upsertDailyStat(const std::string& date, double revenue, int ordersCount) {
    std::ostringstream sql;
    sql << "INSERT INTO daily_stats (date, revenue, orders_count) VALUES ('" << date << "', " << revenue << ", " << ordersCount << ") "
        << "ON CONFLICT (date) DO UPDATE SET "
        << "revenue = daily_stats.revenue + " << revenue << ", "
        << "orders_count = daily_stats.orders_count + " << ordersCount << ", "
        << "updated_at = CURRENT_TIMESTAMP";
    
    execute(sql.str());
    return true;
}

std::vector<HourlyStatEntity> Database::getHourlyStats(const std::string& date) {
    std::vector<HourlyStatEntity> stats;
    
    std::string sql = "SELECT id, date, hour, revenue, orders_count, created_at, updated_at FROM hourly_stats WHERE date = '" + date + "' ORDER BY hour";
    PGresult* res = query(sql);
    
    if (!res) return stats;
    
    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        HourlyStatEntity stat;
        stat.id = std::stoi(PQgetvalue(res, i, 0));
        stat.date = PQgetvalue(res, i, 1);
        stat.hour = std::stoi(PQgetvalue(res, i, 2));
        stat.revenue = std::stod(PQgetvalue(res, i, 3));
        stat.ordersCount = std::stoi(PQgetvalue(res, i, 4));
        stat.createdAt = PQgetvalue(res, i, 5);
        stat.updatedAt = PQgetvalue(res, i, 6);
        stats.push_back(stat);
    }
    
    PQclear(res);
    return stats;
}

bool Database::upsertHourlyStat(const std::string& date, int hour, double revenue, int ordersCount) {
    std::ostringstream sql;
    sql << "INSERT INTO hourly_stats (date, hour, revenue, orders_count) VALUES ('" << date << "', " << hour << ", " << revenue << ", " << ordersCount << ") "
        << "ON CONFLICT (date, hour) DO UPDATE SET "
        << "revenue = hourly_stats.revenue + " << revenue << ", "
        << "orders_count = hourly_stats.orders_count + " << ordersCount << ", "
        << "updated_at = CURRENT_TIMESTAMP";
    
    execute(sql.str());
    return true;
}

std::vector<ItemStatEntity> Database::getItemStats(const std::string& date) {
    std::vector<ItemStatEntity> stats;
    
    std::string sql = "SELECT id, date, item_name, quantity, created_at, updated_at FROM item_stats WHERE date = '" + date + "' ORDER BY quantity DESC";
    PGresult* res = query(sql);
    
    if (!res) return stats;
    
    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        ItemStatEntity stat;
        stat.id = std::stoi(PQgetvalue(res, i, 0));
        stat.date = PQgetvalue(res, i, 1);
        stat.itemName = PQgetvalue(res, i, 2);
        stat.quantity = std::stoi(PQgetvalue(res, i, 3));
        stat.createdAt = PQgetvalue(res, i, 4);
        stat.updatedAt = PQgetvalue(res, i, 5);
        stats.push_back(stat);
    }
    
    PQclear(res);
    return stats;
}

bool Database::upsertItemStat(const std::string& date, const std::string& itemName, int quantity) {
    std::ostringstream sql;
    sql << "INSERT INTO item_stats (date, item_name, quantity) VALUES ('" << date << "', '" << escapeString(itemName) << "', " << quantity << ") "
        << "ON CONFLICT (date, item_name) DO UPDATE SET "
        << "quantity = item_stats.quantity + " << quantity << ", "
        << "updated_at = CURRENT_TIMESTAMP";
    
    execute(sql.str());
    return true;
}

std::vector<IngredientEntity> Database::getAllIngredients() {
    std::vector<IngredientEntity> ingredients;
    
    PGresult* res = query("SELECT id, name, unit, quantity, min_stock, cost_per_unit, category, created_at, updated_at FROM ingredients ORDER BY category, name");
    
    if (!res) return ingredients;
    
    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        IngredientEntity ingredient;
        ingredient.id = std::stoi(PQgetvalue(res, i, 0));
        ingredient.name = PQgetvalue(res, i, 1);
        ingredient.unit = PQgetvalue(res, i, 2);
        ingredient.quantity = std::stod(PQgetvalue(res, i, 3));
        ingredient.minStock = std::stod(PQgetvalue(res, i, 4));
        ingredient.costPerUnit = std::stod(PQgetvalue(res, i, 5));
        ingredient.category = PQgetvalue(res, i, 6);
        ingredient.createdAt = PQgetvalue(res, i, 7);
        ingredient.updatedAt = PQgetvalue(res, i, 8);
        ingredients.push_back(ingredient);
    }
    
    PQclear(res);
    return ingredients;
}

IngredientEntity Database::getIngredientById(int id) {
    IngredientEntity ingredient;
    ingredient.id = -1;
    
    std::string sql = "SELECT id, name, unit, quantity, min_stock, cost_per_unit, category, created_at, updated_at FROM ingredients WHERE id = " + std::to_string(id);
    PGresult* res = query(sql);
    
    if (!res || PQntuples(res) == 0) {
        if (res) PQclear(res);
        return ingredient;
    }
    
    ingredient.id = std::stoi(PQgetvalue(res, 0, 0));
    ingredient.name = PQgetvalue(res, 0, 1);
    ingredient.unit = PQgetvalue(res, 0, 2);
    ingredient.quantity = std::stod(PQgetvalue(res, 0, 3));
    ingredient.minStock = std::stod(PQgetvalue(res, 0, 4));
    ingredient.costPerUnit = std::stod(PQgetvalue(res, 0, 5));
    ingredient.category = PQgetvalue(res, 0, 6);
    ingredient.createdAt = PQgetvalue(res, 0, 7);
    ingredient.updatedAt = PQgetvalue(res, 0, 8);
    
    PQclear(res);
    return ingredient;
}

bool Database::insertIngredient(const IngredientEntity& ingredient) {
    std::ostringstream sql;
    sql << "INSERT INTO ingredients (name, unit, quantity, min_stock, cost_per_unit, category) VALUES ("
        << "'" << escapeString(ingredient.name) << "', "
        << "'" << escapeString(ingredient.unit) << "', "
        << ingredient.quantity << ", "
        << ingredient.minStock << ", "
        << ingredient.costPerUnit << ", "
        << "'" << escapeString(ingredient.category) << "')";
    
    execute(sql.str());
    return true;
}

bool Database::updateIngredient(int id, const IngredientEntity& ingredient) {
    std::ostringstream sql;
    sql << "UPDATE ingredients SET "
        << "name = '" << escapeString(ingredient.name) << "', "
        << "unit = '" << escapeString(ingredient.unit) << "', "
        << "quantity = " << ingredient.quantity << ", "
        << "min_stock = " << ingredient.minStock << ", "
        << "cost_per_unit = " << ingredient.costPerUnit << ", "
        << "category = '" << escapeString(ingredient.category) << "', "
        << "updated_at = CURRENT_TIMESTAMP "
        << "WHERE id = " << id;
    
    execute(sql.str());
    return true;
}

bool Database::deleteIngredient(int id) {
    std::string sql = "DELETE FROM ingredients WHERE id = " + std::to_string(id);
    execute(sql);
    return true;
}

bool Database::updateIngredientQuantity(int id, double quantity) {
    std::ostringstream sql;
    sql << "UPDATE ingredients SET "
        << "quantity = " << quantity << ", "
        << "updated_at = CURRENT_TIMESTAMP "
        << "WHERE id = " << id;
    
    execute(sql.str());
    return true;
}

std::vector<InventoryTransactionEntity> Database::getInventoryTransactions(int ingredientId) {
    std::vector<InventoryTransactionEntity> transactions;
    
    std::string sql = "SELECT id, ingredient_id, type, quantity, unit_price, note, created_at FROM inventory_transactions WHERE ingredient_id = " + std::to_string(ingredientId) + " ORDER BY created_at DESC";
    PGresult* res = query(sql);
    
    if (!res) return transactions;
    
    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        InventoryTransactionEntity transaction;
        transaction.id = std::stoi(PQgetvalue(res, i, 0));
        transaction.ingredientId = std::stoi(PQgetvalue(res, i, 1));
        transaction.type = PQgetvalue(res, i, 2);
        transaction.quantity = std::stod(PQgetvalue(res, i, 3));
        transaction.unitPrice = std::stod(PQgetvalue(res, i, 4));
        transaction.note = PQgetvalue(res, i, 5);
        transaction.createdAt = PQgetvalue(res, i, 6);
        transactions.push_back(transaction);
    }
    
    PQclear(res);
    return transactions;
}

int Database::insertInventoryTransaction(const InventoryTransactionEntity& transaction) {
    std::ostringstream sql;
    sql << "INSERT INTO inventory_transactions (ingredient_id, type, quantity, unit_price, note) VALUES ("
        << transaction.ingredientId << ", "
        << "'" << transaction.type << "', "
        << transaction.quantity << ", "
        << transaction.unitPrice << ", "
        << "'" << escapeString(transaction.note) << "') "
        << "RETURNING id";
    
    PGresult* res = query(sql.str());
    
    if (!res || PQntuples(res) == 0) {
        if (res) PQclear(res);
        return -1;
    }
    
    int id = std::stoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    return id;
}

int Database::getNextOrderId() {
    PGresult* res = query("SELECT COALESCE(MAX(id), 0) + 1 FROM orders");
    if (!res || PQntuples(res) == 0) {
        if (res) PQclear(res);
        return 1;
    }
    int id = std::stoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    return id;
}

int Database::getNextMenuItemId() {
    PGresult* res = query("SELECT COALESCE(MAX(id), 0) + 1 FROM menu_items");
    if (!res || PQntuples(res) == 0) {
        if (res) PQclear(res);
        return 1;
    }
    int id = std::stoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    return id;
}

int Database::getNextCustomerId() {
    PGresult* res = query("SELECT COALESCE(MAX(id), 0) + 1 FROM customers");
    if (!res || PQntuples(res) == 0) {
        if (res) PQclear(res);
        return 1;
    }
    int id = std::stoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    return id;
}

int Database::getNextTableId() {
    PGresult* res = query("SELECT COALESCE(MAX(id), 0) + 1 FROM tables");
    if (!res || PQntuples(res) == 0) {
        if (res) PQclear(res);
        return 1;
    }
    int id = std::stoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    return id;
}

void Database::initializeDatabase() {
    execute("CREATE TABLE IF NOT EXISTS menu_items (id SERIAL PRIMARY KEY, name VARCHAR(100) NOT NULL, price DECIMAL(10, 2) NOT NULL DEFAULT 0, category VARCHAR(50) NOT NULL DEFAULT 'coffee', emoji VARCHAR(10) DEFAULT '☕', bg_class VARCHAR(100) DEFAULT 'from-amber-900 to-amber-700', available BOOLEAN DEFAULT TRUE, created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");
    
    execute("CREATE TABLE IF NOT EXISTS tables (id SERIAL PRIMARY KEY, name VARCHAR(50) NOT NULL, status VARCHAR(20) NOT NULL DEFAULT 'available', current_order_id INTEGER DEFAULT NULL, created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");
    
    execute("CREATE TABLE IF NOT EXISTS customers (id SERIAL PRIMARY KEY, name VARCHAR(100) NOT NULL, phone VARCHAR(20) NOT NULL, email VARCHAR(100) DEFAULT '', visits INTEGER DEFAULT 0, total_spent DECIMAL(12, 2) DEFAULT 0, created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, last_visit TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");
    
    execute("CREATE TABLE IF NOT EXISTS orders (id SERIAL PRIMARY KEY, table_number INTEGER DEFAULT 0, priority VARCHAR(20) DEFAULT 'normal', note TEXT DEFAULT '', status INTEGER NOT NULL DEFAULT 0, total DECIMAL(12, 2) DEFAULT 0, created_by INTEGER DEFAULT 0, created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");
    
    execute("CREATE TABLE IF NOT EXISTS order_items (id SERIAL PRIMARY KEY, order_id INTEGER NOT NULL REFERENCES orders(id) ON DELETE CASCADE, menu_item_id INTEGER REFERENCES menu_items(id) ON DELETE SET NULL, name VARCHAR(100) NOT NULL, qty INTEGER NOT NULL DEFAULT 1, price DECIMAL(10, 2) NOT NULL DEFAULT 0, created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");
    
    execute("CREATE TABLE IF NOT EXISTS daily_stats (id SERIAL PRIMARY KEY, date DATE NOT NULL UNIQUE, revenue DECIMAL(12, 2) DEFAULT 0, orders_count INTEGER DEFAULT 0, created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");
    
    execute("CREATE TABLE IF NOT EXISTS hourly_stats (id SERIAL PRIMARY KEY, date DATE NOT NULL, hour INTEGER NOT NULL, revenue DECIMAL(12, 2) DEFAULT 0, orders_count INTEGER DEFAULT 0, UNIQUE(date, hour), created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");
    
    execute("CREATE TABLE IF NOT EXISTS item_stats (id SERIAL PRIMARY KEY, date DATE NOT NULL, item_name VARCHAR(100) NOT NULL, quantity INTEGER DEFAULT 0, UNIQUE(date, item_name), created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");

    execute("CREATE INDEX IF NOT EXISTS idx_orders_status ON orders(status)");
    execute("CREATE INDEX IF NOT EXISTS idx_orders_created_at ON orders(created_at)");
    execute("CREATE INDEX IF NOT EXISTS idx_orders_table ON orders(table_number)");
    execute("CREATE INDEX IF NOT EXISTS idx_order_items_order ON order_items(order_id)");
    execute("CREATE INDEX IF NOT EXISTS idx_daily_stats_date ON daily_stats(date)");
    execute("CREATE INDEX IF NOT EXISTS idx_hourly_stats_date ON hourly_stats(date)");
    execute("CREATE INDEX IF NOT EXISTS idx_item_stats_date ON item_stats(date)");
    execute("CREATE INDEX IF NOT EXISTS idx_customers_phone ON customers(phone)");
    
    execute("CREATE TABLE IF NOT EXISTS ingredients (id SERIAL PRIMARY KEY, name VARCHAR(100) NOT NULL, unit VARCHAR(20) NOT NULL DEFAULT 'pcs', quantity DECIMAL(10, 2) NOT NULL DEFAULT 0, min_stock DECIMAL(10, 2) NOT NULL DEFAULT 0, cost_per_unit DECIMAL(12, 2) NOT NULL DEFAULT 0, category VARCHAR(50) DEFAULT 'general', created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");
    
    execute("CREATE TABLE IF NOT EXISTS inventory_transactions (id SERIAL PRIMARY KEY, ingredient_id INTEGER NOT NULL REFERENCES ingredients(id) ON DELETE CASCADE, type VARCHAR(20) NOT NULL, quantity DECIMAL(10, 2) NOT NULL, unit_price DECIMAL(12, 2) DEFAULT 0, note TEXT DEFAULT '', created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");
    
    execute("CREATE INDEX IF NOT EXISTS idx_ingredients_category ON ingredients(category)");
    execute("CREATE INDEX IF NOT EXISTS idx_inventory_trans_ingredient ON inventory_transactions(ingredient_id)");
    
    Logger::info("Database schema initialized");
}

std::string Database::escapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    
    for (char c : str) {
        if (c == '\'') {
            result += "''";
        } else if (c == '\\') {
            result += "\\\\";
        } else {
            result += c;
        }
    }
    
    return result;
}
