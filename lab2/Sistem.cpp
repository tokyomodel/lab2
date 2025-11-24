#include "Sistem.h"
#include "Loger.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <set>

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
        Cs station;
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

        // В реальном приложении нужно установить значения через сеттеры
        stations.push_back(station);
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

Cs* DataManager::getStationById(int id) {
    auto it = std::find_if(stations.begin(), stations.end(), [id](const Cs& s) { return s.getId() == id; });
    return it != stations.end() ? &(*it) : nullptr;
}