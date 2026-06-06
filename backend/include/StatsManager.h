#ifndef STATS_MANAGER_H
#define STATS_MANAGER_H

#include "nlohmann/json.hpp"
#include <vector>
#include <mutex>
#include <string>
#include <map>

using json = nlohmann::json;

struct DailyRevenue {
    std::string date;
    double revenue;
};

struct HourlyRevenue {
    int hour;
    double revenue;
};

struct ItemStats {
    std::string name;
    int count;
};

struct OrderStats {
    int total;
    int newOrders;
    int cooking;
    int done;
    int cancelled;
};

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
    std::vector<DailyRevenue> dailyRevenue;
    std::vector<HourlyRevenue> hourlyRevenue;
    std::vector<ItemStats> itemStats;
    std::map<std::string, int> orderStatusCounts;
    std::mutex mutex;

    std::string getCurrentDate();
    std::string getDateMinusDays(int days);
};

#endif
