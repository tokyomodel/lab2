#include "GasNetwork.h"
#include "Sistem.h"
#include <iostream>
#include <algorithm>
#include <stack>

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