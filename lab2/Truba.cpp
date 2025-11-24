#include "Truba.h"
#include "Loger.h"
#include <iostream>
#include <string>
#include <algorithm>

int Tube::nextId = 1;

Tube::Tube() : id(nextId++), length(0), diameter(0), underRepair(false) {}

// Геттеры
int Tube::getId() const { return id; }
std::string Tube::getName() const { return name; }
double Tube::getLength() const { return length; }
int Tube::getDiameter() const { return diameter; }
bool Tube::isUnderRepair() const { return underRepair; }

// Сеттеры
void Tube::setName(const std::string& n) { name = n; }
void Tube::setLength(double l) { length = l; }
void Tube::setDiameter(int d) { diameter = d; }
void Tube::setUnderRepair(bool repair) { underRepair = repair; }

void Tube::input() {
    std::cout << "Введите километровую отметку трубы: ";
    std::getline(std::cin, name);

    std::cout << "Если хотите ввести нецелое число - используйте точку.\n";
    length = inputDouble("Введите длину трубы (км): ");
    diameter = inputInt("Введите диаметр трубы (мм): ");

    underRepair = false;

    std::string logMsg = "Добавлена труба ID: " + std::to_string(id) + ", название: " + name;
    logger.log(logMsg);
}

void Tube::edit() {
    underRepair = !underRepair;
    std::string status = underRepair ? "в ремонт" : "в работу";
    std::cout << "Статус ремонта трубы изменен на: " << (underRepair ? "В ремонте" : "Работает") << "\n";

    std::string logMsg = "Изменен статус трубы ID: " + std::to_string(id) + " - " + status;
    logger.log(logMsg);
}

void Tube::display() const {
    std::cout << "Труба ID: " << id
        << ", Название: " << name
        << ", Длина: " << length << " км"
        << ", Диаметр: " << diameter << " мм"
        << ", Статус: " << (underRepair ? "в ремонте" : "работает") << "\n";
}

bool Tube::matchesFilter(const std::string& filterName, bool filterRepair) const {
    bool nameMatch = filterName.empty() || (name.find(filterName) != std::string::npos);
    bool repairMatch = !filterRepair || underRepair;
    return nameMatch && repairMatch;
}

int Tube::inputInt(const std::string& prompt, int minVal) {
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
                if (value >= minVal) {
                    return value;
                }
                else {
                    std::cout << "Ошибка! Число должно быть не меньше " << minVal << ". Попробуйте снова: ";
                }
            }
            catch (...) {
                std::cout << "Ошибка! Введите целое число: ";
            }
        }
        else {
            std::cout << "Ошибка! Введите целое положительное число: ";
        }
    }
}

double Tube::inputDouble(const std::string& prompt, double minVal) {
    std::string input;
    double value;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, input);

        if (input.empty()) {
            std::cout << "Ошибка! Введите число: ";
            continue;
        }

        bool hasDecimal = false;
        bool isValid = true;

        for (size_t i = 0; i < input.length(); i++) {
            if (input[i] == '.') {
                if (hasDecimal) {
                    isValid = false;
                    break;
                }
                hasDecimal = true;
            }
            else if (!isdigit(input[i])) {
                isValid = false;
                break;
            }
        }

        if (isValid) {
            try {
                value = std::stod(input);
                if (value >= minVal) {
                    return value;
                }
                else {
                    std::cout << "Ошибка! Число должно быть не меньше " << minVal << ". Попробуйте снова: ";
                }
            }
            catch (...) {
                std::cout << "Ошибка! Введите число: ";
            }
        }
        else {
            std::cout << "Ошибка! Введите число (например: 12.5): ";
        }
    }
}