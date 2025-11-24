#pragma once
#include <string>

class Tube {
private:
    static int nextId;
    int id;
    std::string name;
    double length;
    int diameter;
    bool underRepair;

public:
    Tube();

    // Геттеры
    int getId() const;
    std::string getName() const;
    double getLength() const;
    int getDiameter() const;
    bool isUnderRepair() const;

    // Сеттеры
    void setName(const std::string& n);
    void setLength(double l);
    void setDiameter(int d);
    void setUnderRepair(bool repair);

    // Методы
    void input();
    void edit();
    void display() const;
    bool matchesFilter(const std::string& filterName, bool filterRepair) const;

    // Статические методы валидации
    static int inputInt(const std::string& prompt, int minVal = 1);
    static double inputDouble(const std::string& prompt, double minVal = 0.001);
};