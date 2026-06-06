#ifndef CUSTOMER_MANAGER_H
#define CUSTOMER_MANAGER_H

#include "entity/CustomerEntity.h"
#include "nlohmann/json.hpp"

#include <vector>
#include <mutex>
#include <string>

using json = nlohmann::json;

class CustomerManager {
public:
    CustomerManager();
    void initializeDefaultCustomers();

    json getAllCustomers();
    json getCustomerById(int id);
    json searchCustomers(const std::string& query);
    int createCustomer(const std::string& name, const std::string& phone, const std::string& email = "");
    bool updateCustomer(int id, const json& data);
    bool deleteCustomer(int id);
    bool incrementVisit(int id, double amount);

    json getTopCustomers(int limit = 10);
    json getCustomerStats();

private:
    std::string getCurrentTime();
    json customerEntityToJson(const CustomerEntity& customer);
};

#endif
