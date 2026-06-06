#include "StatsManager.h"
#include "logger.h"

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>

StatsManager::StatsManager() {
    for (int i = 0; i < 24; i++) {
        hourlyRevenue.push_back({i, 0});
    }
    orderStatusCounts["new"] = 0;
    orderStatusCounts["cooking"] = 0;
    orderStatusCounts["done"] = 0;
    orderStatusCounts["paid"] = 0;
    orderStatusCounts["cancelled"] = 0;
    Logger::info("StatsManager initialized");
}

void StatsManager::recordOrder(int tableNumber, const std::vector<std::pair<std::string, int>>& items, double total) {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::string today = getCurrentDate();
    
    bool found = false;
    for (auto& day : dailyRevenue) {
        if (day.date == today) {
            day.revenue += 0;
            found = true;
            break;
        }
    }
    if (!found) {
        dailyRevenue.push_back({today, 0});
    }
    
    for (const auto& item : items) {
        bool itemFound = false;
        for (auto& stats : itemStats) {
            if (stats.name == item.first) {
                stats.count += item.second;
                itemFound = true;
                break;
            }
        }
        if (!itemFound) {
            itemStats.push_back({item.first, item.second});
        }
    }
    
    orderStatusCounts["new"]++;
}

void StatsManager::recordPayment(int orderId, double amount) {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    int hour = localTime->tm_hour;
    
    for (auto& hr : hourlyRevenue) {
        if (hr.hour == hour) {
            hr.revenue += amount;
            break;
        }
    }
    
    std::string today = getCurrentDate();
    bool found = false;
    for (auto& day : dailyRevenue) {
        if (day.date == today) {
            day.revenue += amount;
            found = true;
            break;
        }
    }
    if (!found) {
        dailyRevenue.push_back({today, amount});
    }
    
    orderStatusCounts["done"]--;
    orderStatusCounts["paid"]++;
}

void StatsManager::recordOrderStatusChange(const std::string& oldStatus, const std::string& newStatus) {
    std::lock_guard<std::mutex> lock(mutex);
    if (orderStatusCounts.find(oldStatus) != orderStatusCounts.end()) {
        orderStatusCounts[oldStatus]--;
    }
    if (orderStatusCounts.find(newStatus) != orderStatusCounts.end()) {
        orderStatusCounts[newStatus]++;
    }
}

json StatsManager::getRevenueByPeriod(const std::string& period) {
    std::lock_guard<std::mutex> lock(mutex);
    json result = json::array();
    
    int days = (period == "day") ? 1 : (period == "week") ? 7 : 30;
    
    for (int i = days - 1; i >= 0; i--) {
        std::string date = getDateMinusDays(i);
        double revenue = 0;
        for (const auto& day : dailyRevenue) {
            if (day.date == date) {
                revenue = day.revenue;
                break;
            }
        }
        std::ostringstream oss;
        oss << date.substr(8, 2) << "/" << date.substr(5, 2);
        result.push_back({{"date", oss.str()}, {"value", revenue}});
    }
    
    return result;
}

json StatsManager::getTopItems(int limit) {
    std::lock_guard<std::mutex> lock(mutex);
    json result = json::array();
    
    auto sorted = itemStats;
    std::sort(sorted.begin(), sorted.end(), [](const ItemStats& a, const ItemStats& b) {
        return a.count > b.count;
    });
    
    for (int i = 0; i < std::min((int)sorted.size(), limit); i++) {
        result.push_back({{"name", sorted[i].name}, {"count", sorted[i].count}});
    }
    
    return result;
}

json StatsManager::getOrderStats() {
    std::lock_guard<std::mutex> lock(mutex);
    int total = 0;
    for (const auto& pair : orderStatusCounts) {
        total += pair.second;
    }
    
    return {
        {"total", total},
        {"new", orderStatusCounts["new"]},
        {"cooking", orderStatusCounts["cooking"]},
        {"done", orderStatusCounts["done"]},
        {"cancelled", orderStatusCounts["cancelled"]}
    };
}

json StatsManager::getRevenueByHour() {
    std::lock_guard<std::mutex> lock(mutex);
    json result = json::array();
    
    for (const auto& hr : hourlyRevenue) {
        std::ostringstream oss;
        oss << hr.hour << "h";
        result.push_back({{"h", oss.str()}, {"v", hr.revenue}});
    }
    
    return result;
}

json StatsManager::getDailyStats(const std::string& date) {
    std::lock_guard<std::mutex> lock(mutex);
    double revenue = 0;
    for (const auto& day : dailyRevenue) {
        if (day.date == date) {
            revenue = day.revenue;
            break;
        }
    }
    
    int orders = 0;
    for (const auto& pair : orderStatusCounts) {
        orders += pair.second;
    }
    
    return {
        {"date", date},
        {"revenue", revenue},
        {"orders", orders}
    };
}

double StatsManager::getTodayRevenue() {
    std::lock_guard<std::mutex> lock(mutex);
    std::string today = getCurrentDate();
    for (const auto& day : dailyRevenue) {
        if (day.date == today) {
            return day.revenue;
        }
    }
    return 0;
}

int StatsManager::getTodayOrderCount() {
    std::lock_guard<std::mutex> lock(mutex);
    return orderStatusCounts["new"] + orderStatusCounts["cooking"] + orderStatusCounts["done"];
}

std::string StatsManager::getCurrentDate() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d");
    return oss.str();
}

std::string StatsManager::getDateMinusDays(int days) {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    localTime->tm_mday -= days;
    std::mktime(localTime);
    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d");
    return oss.str();
}
