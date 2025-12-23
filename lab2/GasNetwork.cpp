#include "GasNetwork.h"
#include "Sistem.h"
#include <iostream>
#include <algorithm>
#include <stack>
#include <queue>
#include <limits>
#include <cmath>

// Таблица производительности труб (м³/час)
static const std::map<int, int> DIAMETER_TO_CAPACITY_MAP = {
    {500, 3000},   // 500 мм -> 3000 м³/час
    {700, 7000},   // 700 мм -> 7000 м³/час
    {1000, 15000}, // 1000 мм -> 15000 м³/час
    {1400, 30000}  // 1400 мм -> 30000 м³/час
};

int GasNetwork::getCapacityByDiameter(int diameter) {
    auto it = DIAMETER_TO_CAPACITY_MAP.find(diameter);
    if (it != DIAMETER_TO_CAPACITY_MAP.end()) {
        return it->second;
    }
    return 0;
}

GasNetwork::GasNetwork() {
    // Инициализация структур для потока
}

bool GasNetwork::addConnection(int fromCsId, int toCsId, int tubeId, int diameter) {
    // Проверяем, нет ли уже такого соединения
    for (const auto& conn : connections) {
        if (conn.fromCsId == fromCsId && conn.toCsId == toCsId) {
            std::cout << "Соединение уже существует!\n";
            return false;
        }
    }

    Connection newConn{ fromCsId, toCsId, tubeId, diameter };
    connections.push_back(newConn);

    // Обновляем граф смежности
    adjacencyList[fromCsId].insert(toCsId);

    // Инициализируем capacity и flow
    capacity[fromCsId][toCsId] = calculateEdgeCapacity(newConn, DataManager());
    flow[fromCsId][toCsId] = 0;

    std::cout << "Соединение добавлено: КС " << fromCsId << " -> КС " << toCsId
        << " через трубу " << tubeId << " (диаметр " << diameter << " мм)\n";
    return true;
}

bool GasNetwork::removeConnection(int fromCsId, int toCsId) {
    auto it = std::remove_if(connections.begin(), connections.end(),
        [fromCsId, toCsId](const Connection& conn) {
            return conn.fromCsId == fromCsId && conn.toCsId == toCsId;
        });

    if (it != connections.end()) {
        connections.erase(it, connections.end());
        adjacencyList[fromCsId].erase(toCsId);

        // Удаляем из структур потока
        capacity[fromCsId].erase(toCsId);
        flow[fromCsId].erase(toCsId);

        std::cout << "Соединение удалено: КС " << fromCsId << " -> КС " << toCsId << "\n";
        return true;
    }

    std::cout << "Соединение не найдено!\n";
    return false;
}

int GasNetwork::findFreeTube(int diameter, const std::vector<Tube>& tubes) {
    // Допустимые диаметры
    std::set<int> allowedDiameters = { 500, 700, 1000, 1400 };

    // Проверяем, что диаметр допустим
    if (allowedDiameters.find(diameter) == allowedDiameters.end()) {
        std::cout << "Недопустимый диаметр! Допустимые: 500, 700, 1000, 1400 мм\n";
        return -1;
    }

    // Собираем ID уже используемых труб
    std::set<int> usedTubeIds;
    for (const auto& conn : connections) {
        usedTubeIds.insert(conn.tubeId);
    }

    // Ищем свободную трубу нужного диаметра
    for (const auto& tube : tubes) {
        if (tube.getDiameter() == diameter &&
            tube.isUnderRepair() == false &&  // не в ремонте
            usedTubeIds.find(tube.getId()) == usedTubeIds.end()) {  // не используется
            return tube.getId();
        }
    }

    return -1; // не найдена
}

void GasNetwork::displayConnections(const DataManager& dm) const {
    std::cout << "=== СОЕДИНЕНИЯ ГАЗОТРАНСПОРТНОЙ СЕТИ ===\n";

    if (connections.empty()) {
        std::cout << "Нет соединений\n";
        return;
    }

    for (const auto& conn : connections) {
        const Cs* fromCs = dm.getStationById(conn.fromCsId);
        const Cs* toCs = dm.getStationById(conn.toCsId);
        const Tube* tube = dm.getTubeById(conn.tubeId);

        std::cout << "КС " << conn.fromCsId << " ("
            << (fromCs ? fromCs->getName() : "не найдена") << ") -> ";
        std::cout << "КС " << conn.toCsId << " ("
            << (toCs ? toCs->getName() : "не найдена") << ") ";
        std::cout << "через трубу " << conn.tubeId << " (диаметр " << conn.diameter << " мм)";

        // Показываем производительность
        int cap = calculateEdgeCapacity(conn, dm);
        std::cout << " [Производительность: " << cap << " м3/час]";  // Изменено с м³ на м3

        if (tube) {
            std::cout << " - " << (tube->isUnderRepair() ? "В РЕМОНТЕ" : "работает");
        }
        std::cout << "\n";
    }
    std::cout << "--------------------------------------\n";
}

std::vector<int> GasNetwork::topologicalSort() const {
    std::vector<int> result;
    std::map<int, int> inDegree;
    std::stack<int> zeroInDegree;

    // Инициализация степеней входа
    for (const auto& pair : adjacencyList) {
        inDegree[pair.first]; // гарантируем наличие
        for (int neighbor : pair.second) {
            inDegree[neighbor]++;
        }
    }

    // Находим вершины с нулевой степенью входа
    for (const auto& pair : inDegree) {
        if (pair.second == 0) {
            zeroInDegree.push(pair.first);
        }
    }

    // Алгоритм Кана
    while (!zeroInDegree.empty()) {
        int vertex = zeroInDegree.top();
        zeroInDegree.pop();
        result.push_back(vertex);

        if (adjacencyList.find(vertex) != adjacencyList.end()) {
            for (int neighbor : adjacencyList.at(vertex)) {
                inDegree[neighbor]--;
                if (inDegree[neighbor] == 0) {
                    zeroInDegree.push(neighbor);
                }
            }
        }
    }

    // Проверка на циклы
    if (result.size() != inDegree.size()) {
        std::cout << "Внимание: граф содержит циклы! Топологическая сортировка невозможна.\n";
        return {};
    }

    return result;
}

int Connection::getCapacity() const {
    return GasNetwork::getCapacityByDiameter(diameter);
}

double Connection::getWeight() const {
    // Базовый вес - производительность (чем больше производительность, тем меньше вес)
    int cap = getCapacity();
    if (cap == 0) return std::numeric_limits<double>::max();
    return 1.0 / cap;
}

const Connection* GasNetwork::getConnection(int fromId, int toId) const {
    for (const auto& conn : connections) {
        if (conn.fromCsId == fromId && conn.toCsId == toId) {
            return &conn;
        }
    }
    return nullptr;
}

int GasNetwork::calculateEdgeCapacity(const Connection& conn, const DataManager& dm) const {
    const Tube* tube = dm.getTubeById(conn.tubeId);
    if (!tube || tube->isUnderRepair()) {
        return 0; // Труба в ремонте или не найдена
    }

    // Используем таблицу производительности
    int tableCapacity = getCapacityByDiameter(conn.diameter);
    if (tableCapacity > 0) {
        return tableCapacity;
    }

    // Формула sqrt(d^5/l) с поправочным коэффициентом
    double length_km = tube->getLength();
    if (length_km < 0.001) length_km = 0.001; // избегаем деления на 0

    // d в метрах, l в километрах
    double d_m = conn.diameter / 1000.0; // мм -> метры
    double l_km = length_km;

    // Упрощенная формула с коэффициентом
    double capacity = std::sqrt(std::pow(d_m, 5) / l_km) * 10000;
    return static_cast<int>(capacity);
}

double GasNetwork::calculateEdgeWeight(const Connection& conn, const DataManager& dm) const {
    const Tube* tube = dm.getTubeById(conn.tubeId);
    if (!tube || tube->isUnderRepair()) {
        return std::numeric_limits<double>::max(); // Бесконечный вес для труб в ремонте
    }

    // Вес = длина трубы (в км)
    return tube->getLength();
}

// Алгоритм Эдмондса-Карпа для максимального потока
bool GasNetwork::bfs(int source, int sink, std::map<int, int>& parent, const DataManager& dm) {
    std::queue<int> q;
    std::set<int> visited;

    q.push(source);
    visited.insert(source);
    parent[source] = -1;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        // Проверяем все исходящие ребра
        if (adjacencyList.find(u) != adjacencyList.end()) {
            for (int v : adjacencyList.at(u)) {
                if (visited.find(v) == visited.end() && capacity[u][v] > flow[u][v]) {
                    q.push(v);
                    parent[v] = u;
                    visited.insert(v);
                    if (v == sink) {
                        return true;
                    }
                }
            }
        }

        // Также проверяем обратные ребра (для остаточного графа)
        for (const auto& conn : connections) {
            if (conn.toCsId == u) {
                int v = conn.fromCsId;
                if (visited.find(v) == visited.end() && flow[v][u] > 0) {
                    q.push(v);
                    parent[v] = u;
                    visited.insert(v);
                }
            }
        }
    }

    return false;
}

int GasNetwork::calculateMaxFlow(int sourceId, int sinkId, const DataManager& dm) {
    // Обнуляем поток
    for (auto& fromMap : flow) {
        for (auto& toFlow : fromMap.second) {
            toFlow.second = 0;
        }
    }

    // Инициализируем capacity для всех ребер
    for (const auto& conn : connections) {
        capacity[conn.fromCsId][conn.toCsId] = calculateEdgeCapacity(conn, dm);
    }

    int maxFlow = 0;
    std::map<int, int> parent;

    // Алгоритм Эдмондса-Карпа
    while (bfs(sourceId, sinkId, parent, dm)) {
        // Находим минимальную остаточную пропускную способность на пути
        int pathFlow = std::numeric_limits<int>::max();

        for (int v = sinkId; v != sourceId; v = parent[v]) {
            int u = parent[v];

            // Определяем направление ребра
            const Connection* conn = getConnection(u, v);
            if (conn) {
                // Прямое ребро
                pathFlow = std::min(pathFlow, capacity[u][v] - flow[u][v]);
            }
            else {
                // Обратное ребро
                pathFlow = std::min(pathFlow, flow[v][u]);
            }
        }

        // Обновляем поток вдоль пути
        for (int v = sinkId; v != sourceId; v = parent[v]) {
            int u = parent[v];

            const Connection* conn = getConnection(u, v);
            if (conn) {
                // Прямое ребро
                flow[u][v] += pathFlow;
            }
            else {
                // Обратное ребро
                flow[v][u] -= pathFlow;
            }
        }

        maxFlow += pathFlow;
    }

    return maxFlow;
}

// Алгоритм Дейкстры для кратчайшего пути
std::vector<int> GasNetwork::findShortestPath(int startId, int endId, const DataManager& dm) {
    std::map<int, double> dist;
    std::map<int, int> prev;
    std::set<int> visited;

    // Инициализация расстояний
    for (const auto& pair : adjacencyList) {
        dist[pair.first] = std::numeric_limits<double>::max();
    }
    for (const auto& conn : connections) {
        dist[conn.fromCsId] = std::numeric_limits<double>::max();
        dist[conn.toCsId] = std::numeric_limits<double>::max();
    }

    dist[startId] = 0;

    // Приоритетная очередь для вершин
    using Pair = std::pair<double, int>;
    std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> pq;
    pq.push({ 0, startId });

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        if (visited.find(u) != visited.end()) continue;
        visited.insert(u);

        // Если достигли конечной вершины
        if (u == endId) break;

        // Проверяем соседей
        if (adjacencyList.find(u) != adjacencyList.end()) {
            for (int v : adjacencyList.at(u)) {
                const Connection* conn = getConnection(u, v);
                if (!conn) continue;

                double weight = calculateEdgeWeight(*conn, dm);

                if (dist[v] > dist[u] + weight) {
                    dist[v] = dist[u] + weight;
                    prev[v] = u;
                    pq.push({ dist[v], v });
                }
            }
        }
    }

    // Восстанавливаем путь
    std::vector<int> path;
    if (dist[endId] == std::numeric_limits<double>::max()) {
        // Путь не найден
        return path;
    }

    for (int at = endId; at != startId; at = prev[at]) {
        path.push_back(at);
    }
    path.push_back(startId);
    std::reverse(path.begin(), path.end());

    return path;
}