#include "Sistem.h"
#include "GasNetwork.h"
#include "Loger.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <set>

DataManager::DataManager() {
    network = new GasNetwork();
}

DataManager::~DataManager() {
    delete network;
}

// Методы для труб
void DataManager::addTube() {
    Tube tube;
    tube.input();
    tubes.push_back(tube);
}

void DataManager::editTube(int id) {
    auto it = std::find_if(tubes.begin(), tubes.end(), [id](const Tube& t) { return t.getId() == id; });
    if (it != tubes.end()) {
        it->edit();
    }
    else {
        std::cout << "Труба с ID " << id << " не найдена!\n";
    }
}

void DataManager::deleteTube(int id) {
    auto it = std::find_if(tubes.begin(), tubes.end(), [id](const Tube& t) { return t.getId() == id; });
    if (it != tubes.end()) {
        logger.log("Удалена труба ID: " + std::to_string(id));
        tubes.erase(it);
        std::cout << "Труба удалена!\n";
    }
    else {
        std::cout << "Труба с ID " << id << " не найдена!\n";
    }
}

// Методы для КС
void DataManager::addStation() {
    Cs station;
    station.input();
    stations.push_back(station);
}

void DataManager::editStation(int id) {
    auto it = std::find_if(stations.begin(), stations.end(), [id](const Cs& s) { return s.getId() == id; });
    if (it != stations.end()) {
        it->edit();
    }
    else {
        std::cout << "КС с ID " << id << " не найдена!\n";
    }
}

void DataManager::deleteStation(int id) {
    auto it = std::find_if(stations.begin(), stations.end(), [id](const Cs& s) { return s.getId() == id; });
    if (it != stations.end()) {
        logger.log("Удалена КС ID: " + std::to_string(id));
        stations.erase(it);
        std::cout << "КС удалена!\n";
    }
    else {
        std::cout << "КС с ID " << id << " не найдена!\n";
    }
}

// Поиск
std::vector<int> DataManager::findTubes(const std::string& nameFilter, bool repairFilter) {
    std::vector<int> result;
    for (const auto& tube : tubes) {
        if (tube.matchesFilter(nameFilter, repairFilter)) {
            result.push_back(tube.getId());
        }
    }
    return result;
}

std::vector<int> DataManager::findStations(const std::string& nameFilter, double maxUnusedPercentage) {
    std::vector<int> result;
    for (const auto& station : stations) {
        if (station.matchesFilter(nameFilter, maxUnusedPercentage)) {
            result.push_back(station.getId());
        }
    }
    return result;
}

// Пакетные операции
void DataManager::batchEditTubes(const std::vector<int>& tubeIds) {
    if (tubeIds.empty()) {
        std::cout << "Нет труб для редактирования!\n";
        return;
    }

    std::cout << "Найдено труб: " << tubeIds.size() << "\n";
    for (int id : tubeIds) {
        auto it = std::find_if(tubes.begin(), tubes.end(), [id](const Tube& t) { return t.getId() == id; });
        if (it != tubes.end()) {
            it->display();
        }
    }

    int choice = Cs::inputIntRange("1. Редактировать все\n2. Выбрать конкретные\nВыберите действие: ", 1, 2);

    if (choice == 1) {
        for (int id : tubeIds) {
            editTube(id);
        }
        logger.log("Пакетное редактирование всех найденных труб (" + std::to_string(tubeIds.size()) + " шт.)");
    }
    else {
        std::cout << "Введите ID труб для редактирования (через пробел, 0 для завершения): ";
        std::string input;
        std::getline(std::cin, input);
        std::stringstream ss(input);
        int id;
        std::set<int> selectedIds;

        while (ss >> id && id != 0) {
            if (std::find(tubeIds.begin(), tubeIds.end(), id) != tubeIds.end()) {
                selectedIds.insert(id);
            }
        }

        for (int id : selectedIds) {
            editTube(id);
        }
        logger.log("Пакетное редактирование выбранных труб (" + std::to_string(selectedIds.size()) + " шт.)");
    }
}

void DataManager::batchDeleteTubes(const std::vector<int>& tubeIds) {
    if (tubeIds.empty()) {
        std::cout << "Нет труб для удаления!\n";
        return;
    }

    std::cout << "Будет удалено труб: " << tubeIds.size() << "\n";
    std::string confirm;
    std::cout << "Подтвердите удаление (yes/no): ";
    std::getline(std::cin, confirm);

    if (confirm == "yes") {
        for (int id : tubeIds) {
            deleteTube(id);
        }
        logger.log("Пакетное удаление труб (" + std::to_string(tubeIds.size()) + " шт.)");
    }
}

// НОВЫЕ МЕТОДЫ ДЛЯ СЕТИ
void DataManager::connectStations() {
    std::cout << "=== СОЕДИНЕНИЕ КС ===\n";

    if (stations.size() < 2) {
        std::cout << "Недостаточно КС для соединения! Нужно минимум 2.\n";
        return;
    }

    int fromId = Tube::inputInt("Введите ID КС входа: ", 1);
    int toId = Tube::inputInt("Введите ID КС выхода: ", 1);

    // Проверяем существование КС
    if (!getStationById(fromId) || !getStationById(toId)) {
        std::cout << "Одна или обе КС не найдены!\n";
        return;
    }

    if (fromId == toId) {
        std::cout << "Нельзя соединить КС саму с собой!\n";
        return;
    }

    // Запрашиваем диаметр
    std::cout << "Введите диаметр трубы (500, 700, 1000 или 1400 мм): ";
    int diameter = Tube::inputInt("", 500);

    // Ищем свободную трубу
    int tubeId = network->findFreeTube(diameter, tubes);

    if (tubeId == -1) {
        std::cout << "Свободной трубы нужного диаметра не найдено.\n";
        std::cout << "Создать новую трубу? (yes/no): ";
        std::string answer;
        std::getline(std::cin, answer);

        if (answer == "yes") {
            Tube newTube;
            newTube.input();
            newTube.setDiameter(diameter);
            newTube.setUnderRepair(false);
            tubes.push_back(newTube);
            tubeId = newTube.getId();
            std::cout << "Создана новая труба ID: " << tubeId << "\n";
        }
        else {
            return;
        }
    }

    // Добавляем соединение
    network->addConnection(fromId, toId, tubeId, diameter);

    // Помечаем трубу как используемую
    auto tube = getTubeById(tubeId);
    if (tube) {
        tube->setUnderRepair(false);
    }
}

void DataManager::disconnectStations() {
    std::cout << "=== ОТСОЕДИНЕНИЕ КС ===\n";

    int fromId = Tube::inputInt("Введите ID КС входа: ", 1);
    int toId = Tube::inputInt("Введите ID КС выхода: ", 1);

    network->removeConnection(fromId, toId);
}

void DataManager::showNetwork() {
    network->displayConnections(*this);
}

void DataManager::topologicalSort() {
    std::cout << "=== ТОПОЛОГИЧЕСКАЯ СОРТИРОВКА ===\n";

    auto sorted = network->topologicalSort();

    if (sorted.empty()) {
        std::cout << "Граф пуст или содержит циклы.\n";
        return;
    }

    std::cout << "Порядок обработки КС:\n";
    for (size_t i = 0; i < sorted.size(); i++) {
        const Cs* station = getStationById(sorted[i]);
        std::cout << i + 1 << ". КС " << sorted[i];
        if (station) {
            std::cout << " (" << station->getName() << ")";
        }
        std::cout << "\n";
    }
}

// Отображение
void DataManager::displayAll() const {
    std::cout << "=== ТРУБЫ ===\n";
    if (tubes.empty()) {
        std::cout << "Трубы не добавлены\n";
    }
    else {
        for (const auto& tube : tubes) {
            tube.display();
        }
    }

    std::cout << "=== КОМПРЕССОРНЫЕ СТАНЦИИ ===\n";
    if (stations.empty()) {
        std::cout << "КС не добавлены\n";
    }
    else {
        for (const auto& station : stations) {
            station.display();
        }
    }

    std::cout << "----------------------\n";
}

// Работа с файлами
void DataManager::saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Ошибка открытия файла для записи!\n";
        return;
    }

    // Сохраняем трубы
    file << tubes.size() << "\n";
    for (const auto& tube : tubes) {
        file << tube.getId() << "\n"
            << tube.getName() << "\n"
            << tube.getLength() << "\n"
            << tube.getDiameter() << "\n"
            << tube.isUnderRepair() << "\n";
    }

    // Сохраняем КС
    file << stations.size() << "\n";
    for (const auto& station : stations) {
        file << station.getId() << "\n"
            << station.getName() << "\n"
            << station.getTotalWorkshops() << "\n"
            << station.getWorkingWorkshops() << "\n"
            << station.getStationClass() << "\n";
    }

    // Сохраняем соединения
    const auto& conns = network->getConnections();
    file << conns.size() << "\n";
    for (const auto& conn : conns) {
        file << conn.fromCsId << "\n"
            << conn.toCsId << "\n"
            << conn.tubeId << "\n"
            << conn.diameter << "\n";
    }

    file.close();
    std::cout << "Данные сохранены в файл: " << filename << "\n";
    logger.log("Сохранение данных в файл: " + filename);
}

void DataManager::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Ошибка открытия файла для чтения!\n";
        return;
    }

    tubes.clear();
    stations.clear();
    delete network;
    network = new GasNetwork();

    // Загружаем трубы
    int tubeCount;
    file >> tubeCount;
    file.ignore();
    for (int i = 0; i < tubeCount; i++) {
        Tube tube;
        int id;
        std::string name;
        double length;
        int diameter;
        bool underRepair;

        file >> id;
        file.ignore();
        std::getline(file, name);
        file >> length >> diameter >> underRepair;
        file.ignore();

        tube.setName(name);
        tube.setLength(length);
        tube.setDiameter(diameter);
        tube.setUnderRepair(underRepair);
        tubes.push_back(tube);
    }

    // Загружаем КС
    int stationCount;
    file >> stationCount;
    file.ignore();
    for (int i = 0; i < stationCount; i++) {
        int id;
        std::string name;
        int totalWorkshops, workingWorkshops;
        std::string stationClass;

        file >> id;
        file.ignore();
        std::getline(file, name);
        file >> totalWorkshops >> workingWorkshops;
        file.ignore();
        std::getline(file, stationClass);

        Cs station;
        station.setId(id);
        station.setName(name);
        station.setTotalWorkshops(totalWorkshops);
        station.setWorkingWorkshops(workingWorkshops);
        station.setStationClass(stationClass);

        stations.push_back(station);
    }

    // Загружаем соединения
    int connCount;
    file >> connCount;
    file.ignore();
    for (int i = 0; i < connCount; i++) {
        int fromId, toId, tubeId, diameter;
        file >> fromId >> toId >> tubeId >> diameter;
        file.ignore();
        network->addConnection(fromId, toId, tubeId, diameter);
    }

    file.close();
    std::cout << "Данные загружены из файла: " << filename << "\n";
    logger.log("Загрузка данных из файла: " + filename);
}

// Получение объектов по ID
Tube* DataManager::getTubeById(int id) {
    auto it = std::find_if(tubes.begin(), tubes.end(), [id](const Tube& t) { return t.getId() == id; });
    return it != tubes.end() ? &(*it) : nullptr;
}

const Tube* DataManager::getTubeById(int id) const {
    auto it = std::find_if(tubes.begin(), tubes.end(), [id](const Tube& t) { return t.getId() == id; });
    return it != tubes.end() ? &(*it) : nullptr;
}

Cs* DataManager::getStationById(int id) {
    auto it = std::find_if(stations.begin(), stations.end(), [id](const Cs& s) { return s.getId() == id; });
    return it != stations.end() ? &(*it) : nullptr;
}

const Cs* DataManager::getStationById(int id) const {
    auto it = std::find_if(stations.begin(), stations.end(), [id](const Cs& s) { return s.getId() == id; });
    return it != stations.end() ? &(*it) : nullptr;
}

// Геттер для сети
GasNetwork* DataManager::getNetwork() {
    return network;
}

const GasNetwork* DataManager::getNetwork() const {
    return network;
}