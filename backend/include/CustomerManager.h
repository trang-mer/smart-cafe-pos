#ifndef CUSTOMER_MANAGER_H
#define CUSTOMER_MANAGER_H

#include "nlohmann/json.hpp"
#include <vector>
#include <mutex>
#include <string>

using json = nlohmann::json;

struct Customer {
    int id;
    std::string name;
    std::string phone;
    std::string email;
    int visits;
    double totalSpent;
    std::string createdAt;
    std::string lastVisit;
};

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
    std::vector<Customer> customers;
    int nextId;
    std::mutex mutex;
    std::string getCurrentTime();
    json customerToJson(const Customer& customer);
};

#endif
