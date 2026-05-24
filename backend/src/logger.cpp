#include "Logger.h"
#include <iostream>
#include <ctime>

std::string currentTime() {
    time_t now = time(nullptr);
    char buffer[26];
    ctime_s(buffer, sizeof(buffer), &now);

    std::string timeStr(buffer);
    timeStr.pop_back();
    return timeStr;
}

void Logger::info(const std::string& message) {
    std::cout << "[INFO][" << currentTime() << "] " << message << std::endl;
}

void Logger::error(const std::string& message) {
    std::cerr << "[ERROR][" << currentTime() << "] " << message << std::endl;
}