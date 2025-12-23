#pragma once
#include "Truba.h"
#include "Cs.h"
#include <vector>
#include <string>
#include <set>

class GasNetwork;

class DataManager {
private:
    std::vector<Tube> tubes;
    std::vector<Cs> stations;
    GasNetwork* network;

public:
    DataManager();
    ~DataManager();

    // Методы для труб
    void addTube();
    void editTube(int id);
    void deleteTube(int id);

    // Методы для КС
    void addStation();
    void editStation(int id);
    void deleteStation(int id);

    // Поиск
    std::vector<int> findTubes(const std::string& nameFilter = "", bool repairFilter = false);
    std::vector<int> findStations(const std::string& nameFilter = "", double maxUnusedPercentage = 100.0);

    // Пакетные операции
    void batchEditTubes(const std::vector<int>& tubeIds);
    void batchDeleteTubes(const std::vector<int>& tubeIds);

    // Методы для сети
    void connectStations();
    void disconnectStations();
    void showNetwork();
    void topologicalSort();

    void calculateMaxFlow();
    void findShortestPath();

    // Отображение
    void displayAll() const;

    // Работа с файлами
    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);

    // Получение объектов по ID
    Tube* getTubeById(int id);
    const Tube* getTubeById(int id) const;

    Cs* getStationById(int id);
    const Cs* getStationById(int id) const;

    // Геттер для сети
    GasNetwork* getNetwork();
    const GasNetwork* getNetwork() const;
};