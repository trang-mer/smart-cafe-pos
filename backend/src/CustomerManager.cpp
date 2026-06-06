#include "CustomerManager.h"
#include "Database.h"
#include "logger.h"

#include <algorithm>
#include <ctime>
#include <iomanip>

CustomerManager::CustomerManager() {
    initializeDefaultCustomers();
}

void CustomerManager::initializeDefaultCustomers() {
    Database& db = Database::getInstance();
    
    std::vector<CustomerEntity> customers = db.getAllCustomers();
    
    if (customers.empty()) {
        std::vector<CustomerEntity> defaultCustomers = {
            {0, "Nguyễn Văn A", "0901234567", "", 24, 2400000, "", ""},
            {0, "Trần Thị B", "0907654321", "", 18, 1800000, "", ""},
            {0, "Lê Văn C", "0912345678", "", 12, 980000, "", ""},
            {0, "Phạm Thị D", "0934567890", "", 8, 720000, "", ""},
        };
        
        for (const auto& customer : defaultCustomers) {
            db.insertCustomer(customer);
        }
        
        Logger::info("Default customers initialized: " + std::to_string(defaultCustomers.size()) + " customers");
    } else {
        Logger::info("Customers loaded from database: " + std::to_string(customers.size()) + " customers");
    }
}

json CustomerManager::getAllCustomers() {
    Database& db = Database::getInstance();
    std::vector<CustomerEntity> customers = db.getAllCustomers();
    
    json result = json::array();
    for (const auto& customer : customers) {
        result.push_back(customerEntityToJson(customer));
    }
    return result;
}

json CustomerManager::getCustomerById(int id) {
    Database& db = Database::getInstance();
    CustomerEntity customer = db.getCustomerById(id);
    
    if (customer.id <= 0) {
        return json{{"error", "Customer not found"}};
    }
    return customerEntityToJson(customer);
}

json CustomerManager::searchCustomers(const std::string& query) {
    Database& db = Database::getInstance();
    std::vector<CustomerEntity> customers = db.searchCustomers(query);
    
    json result = json::array();
    for (const auto& customer : customers) {
        result.push_back(customerEntityToJson(customer));
    }
    return result;
}

int CustomerManager::createCustomer(const std::string& name, const std::string& phone, const std::string& email) {
    Database& db = Database::getInstance();
    
    CustomerEntity customer;
    customer.name = name;
    customer.phone = phone;
    customer.email = email;
    customer.visits = 0;
    customer.totalSpent = 0;
    customer.createdAt = getCurrentTime();
    customer.lastVisit = getCurrentTime();
    
    return db.insertCustomer(customer);
}

bool CustomerManager::updateCustomer(int id, const json& data) {
    Database& db = Database::getInstance();
    CustomerEntity customer = db.getCustomerById(id);
    
    if (customer.id <= 0) return false;
    
    if (data.contains("name")) customer.name = data["name"];
    if (data.contains("phone")) customer.phone = data["phone"];
    if (data.contains("email")) customer.email = data["email"];
    
    return db.updateCustomer(id, customer);
}

bool CustomerManager::deleteCustomer(int id) {
    Database& db = Database::getInstance();
    return db.deleteCustomer(id);
}

bool CustomerManager::incrementVisit(int id, double amount) {
    Database& db = Database::getInstance();
    return db.incrementCustomerVisit(id, amount);
}

json CustomerManager::getTopCustomers(int limit) {
    Database& db = Database::getInstance();
    std::vector<CustomerEntity> customers = db.getAllCustomers();
    
    std::sort(customers.begin(), customers.end(), [](const CustomerEntity& a, const CustomerEntity& b) {
        return a.totalSpent > b.totalSpent;
    });
    
    json result = json::array();
    for (int i = 0; i < std::min((int)customers.size(), limit); i++) {
        result.push_back(customerEntityToJson(customers[i]));
    }
    return result;
}

json CustomerManager::getCustomerStats() {
    Database& db = Database::getInstance();
    std::vector<CustomerEntity> customers = db.getAllCustomers();
    
    double totalSpent = 0;
    int totalVisits = 0;
    for (const auto& customer : customers) {
        totalSpent += customer.totalSpent;
        totalVisits += customer.visits;
    }
    
    return {
        {"totalCustomers", (int)customers.size()},
        {"totalSpent", totalSpent},
        {"totalVisits", totalVisits},
        {"averageSpent", customers.empty() ? 0 : totalSpent / customers.size()}
    };
}

std::string CustomerManager::getCurrentTime() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

json CustomerManager::customerEntityToJson(const CustomerEntity& customer) {
    return {
        {"id", std::to_string(customer.id)},
        {"name", customer.name},
        {"phone", customer.phone},
        {"email", customer.email},
        {"visits", customer.visits},
        {"spent", customer.totalSpent},
        {"createdAt", customer.createdAt},
        {"lastVisit", customer.lastVisit}
    };
}
