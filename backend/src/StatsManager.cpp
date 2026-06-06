#include "StatsManager.h"
#include "Database.h"
#include "logger.h"

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>

StatsManager::StatsManager() {
    Logger::info("StatsManager initialized with database support");
}

void StatsManager::recordOrder(int tableNumber, const std::vector<std::pair<std::string, int>>& items, double total) {
    Database& db = Database::getInstance();
    
    std::string today = getCurrentDate();
    
    for (const auto& item : items) {
        db.upsertItemStat(today, item.first, item.second);
    }
}

void StatsManager::recordPayment(int orderId, double amount) {
    Database& db = Database::getInstance();
    
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    int hour = localTime->tm_hour;
    std::string today = getCurrentDate();
    
    db.upsertDailyStat(today, amount, 1);
    db.upsertHourlyStat(today, hour, amount, 1);
}

void StatsManager::recordOrderStatusChange(const std::string& oldStatus, const std::string& newStatus) {
}

json StatsManager::getRevenueByPeriod(const std::string& period) {
    Database& db = Database::getInstance();
    json result = json::array();
    
    int days = (period == "day") ? 1 : (period == "week") ? 7 : 30;
    
    for (int i = days - 1; i >= 0; i--) {
        std::string date = getDateMinusDays(i);
        DailyStatEntity stat = db.getDailyStatByDate(date);
        
        double revenue = 0;
        if (stat.id > 0) {
            revenue = stat.revenue;
        }
        
        std::ostringstream oss;
        oss << date.substr(8, 2) << "/" << date.substr(5, 2);
        result.push_back({{"date", oss.str()}, {"value", revenue}});
    }
    
    return result;
}

json StatsManager::getTopItems(int limit) {
    Database& db = Database::getInstance();
    json result = json::array();
    
    std::string today = getCurrentDate();
    std::vector<ItemStatEntity> stats = db.getItemStats(today);
    
    std::sort(stats.begin(), stats.end(), [](const ItemStatEntity& a, const ItemStatEntity& b) {
        return a.quantity > b.quantity;
    });
    
    for (int i = 0; i < std::min((int)stats.size(), limit); i++) {
        result.push_back({{"name", stats[i].itemName}, {"count", stats[i].quantity}});
    }
    
    return result;
}

json StatsManager::getOrderStats() {
    Database& db = Database::getInstance();
    
    std::vector<OrderEntity> allOrders = db.getAllOrders();
    
    int total = 0;
    int newOrders = 0;
    int cooking = 0;
    int done = 0;
    int cancelled = 0;
    
    for (const auto& order : allOrders) {
        total++;
        switch (order.status) {
            case OrderStatus::PENDING: newOrders++; break;
            case OrderStatus::COOKING: cooking++; break;
            case OrderStatus::DONE: done++; break;
            case OrderStatus::CANCELLED: cancelled++; break;
            default: break;
        }
    }
    
    return {
        {"total", total},
        {"new", newOrders},
        {"cooking", cooking},
        {"done", done},
        {"cancelled", cancelled}
    };
}

json StatsManager::getRevenueByHour() {
    Database& db = Database::getInstance();
    json result = json::array();
    
    std::string today = getCurrentDate();
    std::vector<HourlyStatEntity> stats = db.getHourlyStats(today);
    
    for (int h = 0; h < 24; h++) {
        double revenue = 0;
        for (const auto& stat : stats) {
            if (stat.hour == h) {
                revenue = stat.revenue;
                break;
            }
        }
        std::ostringstream oss;
        oss << h << "h";
        result.push_back({{"h", oss.str()}, {"v", revenue}});
    }
    
    return result;
}

json StatsManager::getDailyStats(const std::string& date) {
    Database& db = Database::getInstance();
    DailyStatEntity stat = db.getDailyStatByDate(date);
    
    if (stat.id <= 0) {
        return {
            {"date", date},
            {"revenue", 0},
            {"orders", 0}
        };
    }
    
    return {
        {"date", date},
        {"revenue", stat.revenue},
        {"orders", stat.ordersCount}
    };
}

double StatsManager::getTodayRevenue() {
    Database& db = Database::getInstance();
    std::string today = getCurrentDate();
    DailyStatEntity stat = db.getDailyStatByDate(today);
    
    if (stat.id <= 0) {
        return 0;
    }
    return stat.revenue;
}

int StatsManager::getTodayOrderCount() {
    Database& db = Database::getInstance();
    std::string today = getCurrentDate();
    DailyStatEntity stat = db.getDailyStatByDate(today);
    
    if (stat.id <= 0) {
        return 0;
    }
    return stat.ordersCount;
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
