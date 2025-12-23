#pragma once
#include "Truba.h"
#include "Cs.h"
#include <vector>
#include <map>
#include <set>
#include <string>

// Предварительное объявление
class DataManager;

struct Connection {
    int fromCsId;
    int toCsId;
    int tubeId;
    int diameter;
};

class GasNetwork {
private:
    std::vector<Connection> connections;
    std::map<int, std::set<int>> adjacencyList;

public:
    // Добавить соединение
    bool addConnection(int fromCsId, int toCsId, int tubeId, int diameter);

    // Удалить соединение
    bool removeConnection(int fromCsId, int toCsId);

    // Найти свободную трубу по диаметру
    int findFreeTube(int diameter, const std::vector<Tube>& tubes);

    // Показать все соединения
    void displayConnections(const DataManager& dm) const;

    // Топологическая сортировка
    std::vector<int> topologicalSort() const;

    // Получить все соединения
    const std::vector<Connection>& getConnections() const { return connections; }
};