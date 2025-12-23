#pragma once
#include "Truba.h"
#include "Cs.h"
#include <vector>
#include <map>
#include <set>
#include <string>
#include <queue>
#include <limits>

// Предварительное объявление
class DataManager;

struct Connection {
    int fromCsId;
    int toCsId;
    int tubeId;
    int diameter;

    // Добавляем для удобства
    int getCapacity() const;  // производительность трубы
    double getWeight() const; // вес ребра для пути
};

class GasNetwork {
private:
    std::vector<Connection> connections;
    std::map<int, std::set<int>> adjacencyList;

    // Для алгоритма максимального потока
    std::map<int, std::map<int, int>> capacity;  // пропускная способность
    std::map<int, std::map<int, int>> flow;      // текущий поток

public:
    GasNetwork();

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

    // Расчет максимального потока
    int calculateMaxFlow(int sourceId, int sinkId, const DataManager& dm);

    // Поиск кратчайшего пути (Дейкстра)
    std::vector<int> findShortestPath(int startId, int endId, const DataManager& dm);

    // Получить все соединения
    const std::vector<Connection>& getConnections() const { return connections; }

    // Получить соединение между двумя КС
    const Connection* getConnection(int fromId, int toId) const;

    // Статический метод для получения производительности по диаметру
    static int getCapacityByDiameter(int diameter);

private:
    // Вспомогательные методы для алгоритма Эдмондса-Карпа
    bool bfs(int source, int sink, std::map<int, int>& parent, const DataManager& dm);

    // Вспомогательные методы для Дейкстры
    double calculateEdgeWeight(const Connection& conn, const DataManager& dm) const;
    int calculateEdgeCapacity(const Connection& conn, const DataManager& dm) const;
};