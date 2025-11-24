#pragma once
#include <string>

class Cs {
private:
    static int nextId;
    int id;
    std::string name;
    int totalWorkshops;
    int workingWorkshops;
    std::string stationClass;

public:
    Cs();

    // Геттеры
    int getId() const;
    std::string getName() const;
    int getTotalWorkshops() const;
    int getWorkingWorkshops() const;
    std::string getStationClass() const;
    double getUnusedPercentage() const;

    // Методы
    void input();
    void edit();
    void display() const;
    bool matchesFilter(const std::string& filterName, double maxUnusedPercentage) const;

    // Статические методы
    static int inputIntRange(const std::string& prompt, int minVal, int maxVal);
};