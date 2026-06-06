#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <memory>
#include <mutex>

#include "entity/MenuItemEntity.h"
#include "entity/TableEntity.h"
#include "entity/CustomerEntity.h"
#include "entity/OrderEntity.h"
#include "entity/StatsEntity.h"
#include "entity/InventoryEntity.h"

#ifdef _WIN32
#include <winsock2.h>
#endif
#include <libpq-fe.h>

class Database {
public:
    static Database& getInstance();

    bool connect(const std::string& conninfo);
    void disconnect();
    bool isConnected() const;

    void execute(const std::string& sql);
    PGresult* query(const std::string& sql);

    std::vector<MenuItemEntity> getAllMenuItems();
    MenuItemEntity getMenuItemById(int id);
    bool insertMenuItem(const MenuItemEntity& item);
    bool updateMenuItem(int id, const MenuItemEntity& item);
    bool deleteMenuItem(int id);

    std::vector<TableEntity> getAllTables();
    TableEntity getTableById(int id);
    bool insertTable(const TableEntity& table);
    bool updateTable(int id, const TableEntity& table);

    std::vector<CustomerEntity> getAllCustomers();
    CustomerEntity getCustomerById(int id);
    std::vector<CustomerEntity> searchCustomers(const std::string& query);
    int insertCustomer(const CustomerEntity& customer);
    bool updateCustomer(int id, const CustomerEntity& customer);
    bool deleteCustomer(int id);
    bool incrementCustomerVisit(int id, double amount);

    std::vector<OrderEntity> getAllOrders();
    OrderEntity getOrderById(int id);
    std::vector<OrderEntity> getOrdersByStatus(OrderStatus status);
    std::vector<OrderEntity> getOrdersByTable(int tableNumber);
    int insertOrder(const OrderEntity& order);
    bool updateOrder(int id, const OrderEntity& order);
    bool updateOrderStatus(int id, OrderStatus status);
    bool deleteOrder(int id);

    int insertOrderItem(const OrderItemEntity& item);

    std::vector<DailyStatEntity> getDailyStats(const std::string& startDate, const std::string& endDate);
    DailyStatEntity getDailyStatByDate(const std::string& date);
    bool upsertDailyStat(const std::string& date, double revenue, int ordersCount);

    std::vector<HourlyStatEntity> getHourlyStats(const std::string& date);
    bool upsertHourlyStat(const std::string& date, int hour, double revenue, int ordersCount);

    std::vector<ItemStatEntity> getItemStats(const std::string& date);
    bool upsertItemStat(const std::string& date, const std::string& itemName, int quantity);

    std::vector<IngredientEntity> getAllIngredients();
    IngredientEntity getIngredientById(int id);
    bool insertIngredient(const IngredientEntity& ingredient);
    bool updateIngredient(int id, const IngredientEntity& ingredient);
    bool deleteIngredient(int id);
    bool updateIngredientQuantity(int id, double quantity);

    std::vector<InventoryTransactionEntity> getInventoryTransactions(int ingredientId);
    int insertInventoryTransaction(const InventoryTransactionEntity& transaction);

    int getNextOrderId();
    int getNextMenuItemId();
    int getNextCustomerId();
    int getNextTableId();

    void initializeDatabase();

private:
    Database();
    ~Database();

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    bool connectDefault();
    std::string escapeString(const std::string& str);

    PGconn* conn;
    bool connected;
    std::mutex mutex;

    std::string dbHost;
    std::string dbPort;
    std::string dbName;
    std::string dbUser;
    std::string dbPassword;
};

#endif
