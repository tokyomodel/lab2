#pragma once
#include <fstream>
#include <string>
#include <chrono>

class Logger {
private:
    std::ofstream logFile;

public:
    Logger();
    ~Logger();
    void log(const std::string& message);
};

extern Logger logger;