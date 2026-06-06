#include "CustomerManager.h"
#include "logger.h"

#include <algorithm>
#include <ctime>
#include <iomanip>

CustomerManager::CustomerManager() : nextId(1) {
    initializeDefaultCustomers();
}

void CustomerManager::initializeDefaultCustomers() {
    std::lock_guard<std::mutex> lock(mutex);
    
    customers = {
        {nextId++, "Nguyễn Văn A", "0901234567", "", 24, 2400000, getCurrentTime(), getCurrentTime()},
        {nextId++, "Trần Thị B", "0907654321", "", 18, 1800000, getCurrentTime(), getCurrentTime()},
        {nextId++, "Lê Văn C", "0912345678", "", 12, 980000, getCurrentTime(), getCurrentTime()},
        {nextId++, "Phạm Thị D", "0934567890", "", 8, 720000, getCurrentTime(), getCurrentTime()},
    };
    
    Logger::info("Customers initialized: " + std::to_string(customers.size()) + " customers");
}

json CustomerManager::getAllCustomers() {
    std::lock_guard<std::mutex> lock(mutex);
    json result = json::array();
    for (const auto& customer : customers) {
        result.push_back(customerToJson(customer));
    }
    return result;
}

json CustomerManager::getCustomerById(int id) {
    std::lock_guard<std::mutex> lock(mutex);
    for (const auto& customer : customers) {
        if (customer.id == id) {
            return customerToJson(customer);
        }
    }
    return json{{"error", "Customer not found"}};
}

json CustomerManager::searchCustomers(const std::string& query) {
    std::lock_guard<std::mutex> lock(mutex);
    json result = json::array();
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    for (const auto& customer : customers) {
        std::string lowerName = customer.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        if (lowerName.find(lowerQuery) != std::string::npos || 
            customer.phone.find(query) != std::string::npos) {
            result.push_back(customerToJson(customer));
        }
    }
    return result;
}

int CustomerManager::createCustomer(const std::string& name, const std::string& phone, const std::string& email) {
    std::lock_guard<std::mutex> lock(mutex);
    Customer customer;
    customer.id = nextId++;
    customer.name = name;
    customer.phone = phone;
    customer.email = email;
    customer.visits = 0;
    customer.totalSpent = 0;
    customer.createdAt = getCurrentTime();
    customer.lastVisit = getCurrentTime();
    customers.push_back(customer);
    return customer.id;
}

bool CustomerManager::updateCustomer(int id, const json& data) {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& customer : customers) {
        if (customer.id == id) {
            if (data.contains("name")) customer.name = data["name"];
            if (data.contains("phone")) customer.phone = data["phone"];
            if (data.contains("email")) customer.email = data["email"];
            return true;
        }
    }
    return false;
}

bool CustomerManager::deleteCustomer(int id) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = std::remove_if(customers.begin(), customers.end(), [id](const Customer& c) {
        return c.id == id;
    });
    if (it != customers.end()) {
        customers.erase(it);
        return true;
    }
    return false;
}

bool CustomerManager::incrementVisit(int id, double amount) {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& customer : customers) {
        if (customer.id == id) {
            customer.visits++;
            customer.totalSpent += amount;
            customer.lastVisit = getCurrentTime();
            return true;
        }
    }
    return false;
}

json CustomerManager::getTopCustomers(int limit) {
    std::lock_guard<std::mutex> lock(mutex);
    json result = json::array();
    auto sorted = customers;
    std::sort(sorted.begin(), sorted.end(), [](const Customer& a, const Customer& b) {
        return a.totalSpent > b.totalSpent;
    });
    for (int i = 0; i < std::min((int)sorted.size(), limit); i++) {
        result.push_back(customerToJson(sorted[i]));
    }
    return result;
}

json CustomerManager::getCustomerStats() {
    std::lock_guard<std::mutex> lock(mutex);
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

json CustomerManager::customerToJson(const Customer& customer) {
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
