#ifndef STATS_MANAGER_H
#define STATS_MANAGER_H

#include "entity/StatsEntity.h"
#include "nlohmann/json.hpp"

#include <vector>
#include <mutex>
#include <string>
#include <map>

using json = nlohmann::json;

class StatsManager {
public:
    StatsManager();

    void recordOrder(int tableNumber, const std::vector<std::pair<std::string, int>>& items, double total);
    void recordPayment(int orderId, double amount);
    void recordOrderStatusChange(const std::string& oldStatus, const std::string& newStatus);

    json getRevenueByPeriod(const std::string& period);
    json getTopItems(int limit = 5);
    json getOrderStats();
    json getRevenueByHour();
    json getDailyStats(const std::string& date);

    double getTodayRevenue();
    int getTodayOrderCount();

private:
    std::string getCurrentDate();
    std::string getDateMinusDays(int days);
};

#endif
