#include "Cs.h"
#include "Loger.h"
#include <iostream>
#include <string>
#include <algorithm>

int Cs::nextId = 1;

Cs::Cs() : id(nextId++), totalWorkshops(0), workingWorkshops(0) {}

// Геттеры
int Cs::getId() const { return id; }
std::string Cs::getName() const { return name; }
int Cs::getTotalWorkshops() const { return totalWorkshops; }
int Cs::getWorkingWorkshops() const { return workingWorkshops; }
std::string Cs::getStationClass() const { return stationClass; }

double Cs::getUnusedPercentage() const {
    if (totalWorkshops == 0) return 0.0;
    return ((totalWorkshops - workingWorkshops) * 100.0) / totalWorkshops;
}

void Cs::input() {
    std::cout << "Введите название КС: ";
    std::getline(std::cin, name);

    totalWorkshops = inputIntRange("Введите количество цехов: ", 1, 1000);
    workingWorkshops = inputIntRange("Введите количество работающих цехов: ", 0, totalWorkshops);

    std::cout << "Введите класс станции: ";
    std::getline(std::cin, stationClass);

    std::string logMsg = "Добавлена КС ID: " + std::to_string(id) + ", название: " + name;
    logger.log(logMsg);
}

void Cs::edit() {
    int choice = inputIntRange("1. Запустить цех \n2. Остановить цех \nВыберите действие: ", 1, 2);

    if (choice == 1) {
        if (workingWorkshops < totalWorkshops) {
            workingWorkshops++;
            std::cout << "Цех запущен. Работающих цехов: " << workingWorkshops << "\n";
            logger.log("Запущен цех на КС ID: " + std::to_string(id));
        }
        else {
            std::cout << "Все цехи уже работают!\n";
        }
    }
    else if (choice == 2) {
        if (workingWorkshops > 0) {
            workingWorkshops--;
            std::cout << "Цех остановлен. Работающих цехов: " << workingWorkshops << "\n";
            logger.log("Остановлен цех на КС ID: " + std::to_string(id));
        }
        else {
            std::cout << "Работающих цехов нет!\n";
        }
    }
}

void Cs::display() const {
    std::cout << "КС ID: " << id
        << ", Название: " << name
        << ", Цехов: " << totalWorkshops
        << ", Работает: " << workingWorkshops
        << ", Незадействовано: " << getUnusedPercentage() << "%"
        << ", Класс: " << stationClass << "\n";
}

bool Cs::matchesFilter(const std::string& filterName, double maxUnusedPercentage) const {
    bool nameMatch = filterName.empty() || (name.find(filterName) != std::string::npos);
    bool percentageMatch = getUnusedPercentage() <= maxUnusedPercentage;
    return nameMatch && percentageMatch;
}

int Cs::inputIntRange(const std::string& prompt, int minVal, int maxVal) {
    std::string input;
    int value;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, input);

        if (input.empty()) {
            std::cout << "Ошибка! Введите число: ";
            continue;
        }

        bool isValid = true;
        for (char c : input) {
            if (!isdigit(c)) {
                isValid = false;
                break;
            }
        }

        if (isValid) {
            try {
                value = std::stoi(input);
                if (value >= minVal && value <= maxVal) {
                    return value;
                }
                else {
                    std::cout << "Ошибка! Число должно быть от " << minVal << " до " << maxVal << ". Попробуйте снова: ";
                }
            }
            catch (...) {
                std::cout << "Ошибка! Введите целое число: ";
            }
        }
        else {
            std::cout << "Ошибка! Введите целое число: ";
        }
    }
}