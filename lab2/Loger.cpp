#include "Loger.h"
#include <iostream>

Logger::Logger() {
    logFile.open("log.txt", std::ios::app);
    log("=== Сеанс начат ===");
}

Logger::~Logger() {
    log("=== Сеанс завершен ===");
    logFile.close();
}

void Logger::log(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    time_t time = std::chrono::system_clock::to_time_t(now);
    char timeStr[26];
    ctime_s(timeStr, sizeof(timeStr), &time);
    logFile << timeStr << ": " << message << std::endl;
}

Logger logger;