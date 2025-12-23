#include "Sistem.h"
#include "Truba.h"
#include <iostream>
#include <locale.h>
#include <string>
#include <algorithm>

void showMenu() {
    std::cout << "\n=== ГЛАВНОЕ МЕНЮ ===\n";
    std::cout << "1. Добавить трубу\n";
    std::cout << "2. Добавить КС\n";
    std::cout << "3. Просмотр всех объектов\n";
    std::cout << "4. Редактировать трубу\n";
    std::cout << "5. Редактировать КС\n";
    std::cout << "6. Удалить трубу\n";
    std::cout << "7. Удалить КС\n";
    std::cout << "8. Поиск труб\n";
    std::cout << "9. Поиск КС\n";
    std::cout << "10. Пакетное редактирование труб\n";
    std::cout << "11. Пакетное удаление труб\n";
    std::cout << "12. Соединить КС\n";
    std::cout << "13. Отсоединить КС\n";
    std::cout << "14. Показать сеть\n";
    std::cout << "15. Топологическая сортировка\n";
    std::cout << "16. Расчет максимального потока\n"; 
    std::cout << "17. Поиск кратчайшего пути\n";  
    std::cout << "18. Сохранить\n";
    std::cout << "19. Загрузить\n";
    std::cout << "0. Выход\n";
    std::cout << "Выберите действие: ";
}

void menu() {
    setlocale(LC_ALL, "Russian");
    DataManager dataManager;

    while (true) {
        showMenu();
        std::string input;
        std::getline(std::cin, input);

        if (input.empty() || !std::all_of(input.begin(), input.end(), ::isdigit)) {
            std::cout << "Ошибка! Введите число от 0 до 19\n";
            continue;
        }

        int choice = std::stoi(input);

        switch (choice) {
        case 1:
            dataManager.addTube();
            break;
        case 2:
            dataManager.addStation();
            break;
        case 3:
            dataManager.displayAll();
            break;
        case 4: {
            int id = Tube::inputInt("Введите ID трубы для редактирования: ");
            dataManager.editTube(id);
            break;
        }
        case 5: {
            int id = Tube::inputInt("Введите ID КС для редактирования: ");
            dataManager.editStation(id);
            break;
        }
        case 6: {
            int id = Tube::inputInt("Введите ID трубы для удаления: ");
            dataManager.deleteTube(id);
            break;
        }
        case 7: {
            int id = Tube::inputInt("Введите ID КС для удаления: ");
            dataManager.deleteStation(id);
            break;
        }
        case 8: {
            std::string nameFilter;
            std::cout << "Введите название для фильтра (или Enter для пропуска): ";
            std::getline(std::cin, nameFilter);

            std::string repairFilter;
            std::cout << "Фильтровать по ремонту? (yes/no): ";
            std::getline(std::cin, repairFilter);
            bool filterRepair = (repairFilter == "yes");

            auto foundTubes = dataManager.findTubes(nameFilter, filterRepair);
            std::cout << "Найдено труб: " << foundTubes.size() << "\n";
            for (int id : foundTubes) {
                if (auto tube = dataManager.getTubeById(id)) {
                    tube->display();
                }
            }
            break;
        }
        case 9: {
            std::string nameFilter;
            std::cout << "Введите название для фильтра (или Enter для пропуска): ";
            std::getline(std::cin, nameFilter);

            double maxPercentage = 100.0;
            std::string percentageFilter;
            std::cout << "Введите максимальный процент незадействованных цехов (или Enter для 100%): ";
            std::getline(std::cin, percentageFilter);
            if (!percentageFilter.empty()) {
                maxPercentage = std::stod(percentageFilter);
            }

            auto foundStations = dataManager.findStations(nameFilter, maxPercentage);
            std::cout << "Найдено КС: " << foundStations.size() << "\n";
            for (int id : foundStations) {
                if (auto station = dataManager.getStationById(id)) {
                    station->display();
                }
            }
            break;
        }
        case 10: {
            std::string nameFilter;
            std::cout << "Введите название для фильтра (или Enter для пропуска): ";
            std::getline(std::cin, nameFilter);

            std::string repairFilter;
            std::cout << "Фильтровать по ремонту? (yes/no): ";
            std::getline(std::cin, repairFilter);
            bool filterRepair = (repairFilter == "yes");

            auto foundTubes = dataManager.findTubes(nameFilter, filterRepair);
            dataManager.batchEditTubes(foundTubes);
            break;
        }
        case 11: {
            std::string nameFilter;
            std::cout << "Введите название для фильтра (или Enter для пропуска): ";
            std::getline(std::cin, nameFilter);

            std::string repairFilter;
            std::cout << "Фильтровать по ремонту? (yes/no): ";
            std::getline(std::cin, repairFilter);
            bool filterRepair = (repairFilter == "yes");

            auto foundTubes = dataManager.findTubes(nameFilter, filterRepair);
            dataManager.batchDeleteTubes(foundTubes);
            break;
        }
        case 12:
            dataManager.connectStations();
            break;
        case 13:
            dataManager.disconnectStations();
            break;
        case 14:
            dataManager.showNetwork();
            break;
        case 15:
            dataManager.topologicalSort();
            break;
        case 16:  
            dataManager.calculateMaxFlow();
            break;
        case 17: 
            dataManager.findShortestPath();
            break;
        case 18: { 
            std::string filename;
            std::cout << "Введите имя файла для сохранения: ";
            std::getline(std::cin, filename);
            dataManager.saveToFile(filename);
            break;
        }
        case 19: { 
            std::string filename;
            std::cout << "Введите имя файла для загрузки: ";
            std::getline(std::cin, filename);
            dataManager.loadFromFile(filename);
            break;
        }
        case 0:
            std::cout << "Выход из программы...\n";
            std::cout << "Нажмите Enter для выхода...";
            std::cin.ignore();
            return;
        default:
            std::cout << "Неверный выбор! Попробуйте снова.\n";
        }
    }
} 

int main() {
    menu();
    return 0;
}