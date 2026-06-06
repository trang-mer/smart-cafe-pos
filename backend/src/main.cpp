#include "Server.h"
#include "ApiServer.h"
#include "Logger.h"
#include "Database.h"

#include <thread>

int main() {
    int httpPort = 8080;
    int tcpPort = 8081;

    Database& db = Database::getInstance();
    std::string conninfo = "host=localhost port=5432 dbname=smart_cafe_pos user=postgres password=postgres sslmode=disable";
    
    if (!db.connect(conninfo)) {
        Logger::error("Cannot connect to database. Please check your PostgreSQL connection.");
        return 1;
    }
    
    db.initializeDatabase();
    Logger::info("Database connected successfully");

    Server tcpServer(tcpPort);
    
    ApiServer apiServer(httpPort);
    apiServer.setMenuManager(&tcpServer.getMenuManager());
    apiServer.setOrderManager(&tcpServer.getOrderManager());
    apiServer.setTableManager(&tcpServer.getTableManager());
    apiServer.setCustomerManager(&tcpServer.getCustomerManager());
    apiServer.setStatsManager(&tcpServer.getStatsManager());
    apiServer.setInventoryManager(&tcpServer.getInventoryManager());
    
    apiServer.start();
    Logger::info("HTTP API Server started on port " + std::to_string(httpPort));

    std::thread tcpThread([&tcpServer]() {
        if (tcpServer.start()) {
            Logger::info("TCP Server started on port " + std::to_string(8081));
            tcpServer.run();
        }
    });

    Logger::info("Smart Cafe POS Server is running!");
    Logger::info("HTTP API: http://localhost:" + std::to_string(httpPort));
    Logger::info("TCP Socket: localhost:" + std::to_string(tcpPort));
    Logger::info("Press Ctrl+C to stop...");

    tcpThread.join();

    apiServer.stop();
    db.disconnect();

    return 0;
}
