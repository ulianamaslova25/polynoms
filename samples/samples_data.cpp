#include <iostream>
#include <clocale>

#include "TOrderedTable.h"
#include "TUnorderedTable.h"
#include "TAVLTree.h"
#include "TRB-Tree.h"
#include "ChainHashTable.h"
#include "HashTableOpenAddressing.h"

#include <polynomial.h>
#include "Parser.h"
#include "transl.h"

void main()
{
    std::setlocale(LC_ALL, "");
    std::cout << "Выберите тип структуры данных для хранения полиномов:" << std::endl;
    std::cout << std::endl;
    std::cout << "1. Упорядоченная таблица (TOrderedTable)" << std::endl;
    std::cout << "2. Неупорядоченная таблица (TUnorderedTable)" << std::endl;
    std::cout << "3. AVL дерево (TAVLTree)" << std::endl;
    std::cout << "4. RB дерево (TRB-Tree)" << std::endl;
    std::cout << "5. Хэш-таблица метод цепочек (ChainHashTable)" << std::endl;
    std::cout << "6. Хэш-таблица с открытой адресацией (HashTableOpenAddressing)" << std::endl;
    std::cout << std::endl;
    std::cout << "Введите номер типа: ";
    int table_choice = 0;
    std::string choice_str;
    std::cin >> choice_str; // Считываем выбор как строку для безопасности

    try {
        table_choice = std::stoi(choice_str); // Преобразуем в int
    }
    catch (const std::invalid_argument& ia) {
        table_choice = 0; // Неверный ввод
    }
    catch (const std::out_of_range& oor) {
        table_choice = 0; // Число вне диапазона
    }

    if (table_choice != 1 && table_choice != 2 && table_choice != 3 && table_choice != 4 && table_choice != 5 && table_choice != 6) {
        std::cerr << "Ошибка: Выбран недоступный тип структуры данных или неверный ввод." << std::endl;
    }
    std::cout << std::endl;

    //---------------------------------------------------

    if (table_choice == 1) {
        std::cout << "Используется Упорядоченная таблица." << std::endl;
        std::cout << std::endl;
        TOrderedTable<char, Polynomial> table;
        int command = 0;
        while (command != 5) {
            std::cout << "Выберите действие:" << std::endl;
            std::cout << "1. Добавить/Обновить полином по ключу" << std::endl;
            std::cout << "2. Найти и вывести полином по ключу" << std::endl;
            std::cout << "3. Удалить полином по ключу" << std::endl;
            std::cout << "4. Вывести все полиномы" << std::endl;
            std::cout << "5. Выход" << std::endl;
            std::cout << std::endl;
            std::cout << "Введите номер действия: ";

            std::string command_str;
            std::cin >> command_str;

            try {
                command = std::stoi(command_str); 
            }
            catch (const std::invalid_argument& ia) {
                command = 0; 
            }
            catch (const std::out_of_range& oor) {
                command = 0; 
            }
            if (command != 1 && command != 2 && command != 3 && command != 4 && command != 5) {
                std::cerr << "Ошибка: Такого номера действия не существует." << std::endl;
            }
            if (command == 1) {
                std::cout << "Введите ключ (один символ): ";
                char k;
                std::cin >> k;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << std::endl;
                std::cout << "Ключ: '" << k << "'" << std::endl;
                std::cout << "Введите полином в строковом формате (например, x^2*y - 5*z + 3): ";
                std::string poly_string;
                std::getline(std::cin, poly_string);
                std::cout << std::endl;
                try {
                    PolynomialTranslyator translator(poly_string);
                    Polynomial poly_obj = translator.calculate();
                    table.insert(k, poly_obj);

                    std::cout << "Полином успешно добавлен/обновлен по ключу '" << k << "'." << std::endl;
                }
                catch (const std::exception& e) {
                    std::cerr << "Ошибка при парсинге или обработке полинома: " << e.what() << std::endl;
                    break;
                }
            }
            if (command == 2) {
                std::cout << "Введите ключ (один символ): ";
                char k;
                std::cin >> k;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << std::endl;
                std::cout << "Ключ: '" << k << "'" << std::endl;
                auto it = table.find(k);
                if (it != table.end()) { 
                    std::cout << "Текущее значение для ключа '" << k << "': ";
                    std::cout << it->second << std::endl; 
                }
                else {
                    std::cerr << "Ошибка: Не удалось найти только что вставленный полином по ключу '" << k << "'." << std::endl;
                }
            }
            if (command == 3) {
                std::cout << "Введите ключ (один символ): ";
                char k;
                std::cin >> k;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << std::endl;
                std::cout << "Ключ: '" << k << "'" << std::endl;
                table.erase(k);
            }
            if (command == 4) {
                table.print();
            }
        }
    }
    //---------------------------------------------------

    if (table_choice == 2) {
        std::cout << "Используется Неупорядоченная таблица." << std::endl;
        std::cout << std::endl;
        TUnorderedTable<char, Polynomial> table;
        int command = 0;
        while (command != 5) {
            std::cout << "Выберите действие:" << std::endl;
            std::cout << "1. Добавить/Обновить полином по ключу" << std::endl;
            std::cout << "2. Найти и вывести полином по ключу" << std::endl;
            std::cout << "3. Удалить полином по ключу" << std::endl;
            std::cout << "4. Вывести все полиномы" << std::endl;
            std::cout << "5. Выход" << std::endl;
            std::cout << std::endl;
            std::cout << "Введите номер действия: ";

            std::string command_str;
            std::cin >> command_str;

            try {
                command = std::stoi(command_str);
            }
            catch (const std::invalid_argument& ia) {
                command = 0;
            }
            catch (const std::out_of_range& oor) {
                command = 0;
            }
            if (command != 1 && command != 2 && command != 3 && command != 4 && command != 5) {
                std::cerr << "Ошибка: Такого номера действия не существует." << std::endl;
            }
            if (command == 1) {
                std::cout << "Введите ключ (один символ): ";
                char k;
                std::cin >> k;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << std::endl;
                std::cout << "Ключ: '" << k << "'" << std::endl;
                std::cout << "Введите полином в строковом формате (например, x^2*y - 5*z + 3): ";
                std::string poly_string;
                std::getline(std::cin, poly_string);
                std::cout << std::endl;
                try {
                    PolynomialTranslyator translator(poly_string);
                    Polynomial poly_obj = translator.calculate();
                    table.insert(k, poly_obj);

                    std::cout << "Полином успешно добавлен/обновлен по ключу '" << k << "'." << std::endl;
                }
                catch (const std::exception& e) {
                    std::cerr << "Ошибка при парсинге или обработке полинома: " << e.what() << std::endl;
                    break;
                }
            }
            if (command == 2) {
                std::cout << "Введите ключ (один символ): ";
                char k;
                std::cin >> k;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << std::endl;
                std::cout << "Ключ: '" << k << "'" << std::endl;
                auto it = table.find(k);
                if (it != table.end()) {
                    std::cout << "Текущее значение для ключа '" << k << "': ";
                    std::cout << it->second << std::endl;
                }
                else {
                    std::cerr << "Ошибка: Не удалось найти только что вставленный полином по ключу '" << k << "'." << std::endl;
                }
            }
            if (command == 3) {
                std::cout << "Введите ключ (один символ): ";
                char k;
                std::cin >> k;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << std::endl;
                std::cout << "Ключ: '" << k << "'" << std::endl;
                table.erase(k);
            }
            if (command == 4) {
                table.print();
            }
        }
    }
    //---------------------------------------------------

    if (table_choice == 3) {
        std::cout << "Используется ALV дерево." << std::endl;
        std::cout << std::endl;
        TAVLTree<char, Polynomial> table;
        int command = 0;
        while (command != 5) {
            std::cout << "Выберите действие:" << std::endl;
            std::cout << "1. Добавить/Обновить полином по ключу" << std::endl;
            std::cout << "2. Найти и вывести полином по ключу" << std::endl;
            std::cout << "3. Удалить полином по ключу" << std::endl;
            std::cout << "4. Вывести все полиномы" << std::endl;
            std::cout << "5. Выход" << std::endl;
            std::cout << std::endl;
            std::cout << "Введите номер действия: ";

            std::string command_str;
            std::cin >> command_str;

            try {
                command = std::stoi(command_str);
            }
            catch (const std::invalid_argument& ia) {
                command = 0;
            }
            catch (const std::out_of_range& oor) {
                command = 0;
            }
            if (command != 1 && command != 2 && command != 3 && command != 4 && command != 5) {
                std::cerr << "Ошибка: Такого номера действия не существует." << std::endl;
            }
            if (command == 1) {
                std::cout << "Введите ключ (один символ): ";
                char k;
                std::cin >> k;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << std::endl;
                std::cout << "Ключ: '" << k << "'" << std::endl;
                std::cout << "Введите полином в строковом формате (например, x^2*y - 5*z + 3): ";
                std::string poly_string;
                std::getline(std::cin, poly_string);
                std::cout << std::endl;
                try {
                    PolynomialTranslyator translator(poly_string);
                    Polynomial poly_obj = translator.calculate();
                    table.insert(k, poly_obj);

                    std::cout << "Полином успешно добавлен/обновлен по ключу '" << k << "'." << std::endl;
                }
                catch (const std::exception& e) {
                    std::cerr << "Ошибка при парсинге или обработке полинома: " << e.what() << std::endl;
                    break;
                }
            }
            if (command == 2) {
                std::cout << "Введите ключ (один символ): ";
                char k;
                std::cin >> k;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << std::endl;
                std::cout << "Ключ: '" << k << "'" << std::endl;
                auto it = table.find(k);
                if (it != table.end()) {
                    std::cout << "Текущее значение для ключа '" << k << "': ";
                    std::cout << (*it).second << std::endl;
                }
                else {
                    std::cerr << "Ошибка: Не удалось найти только что вставленный полином по ключу '" << k << "'." << std::endl;
                }
            }
            if (command == 3) {
                std::cout << "Введите ключ (один символ): ";
                char k;
                std::cin >> k;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << std::endl;
                std::cout << "Ключ: '" << k << "'" << std::endl;
                table.erase(k);
            }
            if (command == 4) {
                table.print();
            }
        }
    }
    //---------------------------------------------------

    if (table_choice == 4) {
        std::cout << "Используется RB дерево." << std::endl;
        std::cout << std::endl;
        RBTree<char, Polynomial> table;
        int command = 0;
        while (command != 5) {
            std::cout << "Выберите действие:" << std::endl;
            std::cout << "1. Добавить/Обновить полином по ключу" << std::endl;
            std::cout << "2. Найти и вывести полином по ключу" << std::endl;
            std::cout << "3. Удалить полином по ключу" << std::endl;
            std::cout << "4. Вывести все полиномы" << std::endl;
            std::cout << "5. Выход" << std::endl;
            std::cout << std::endl;
            std::cout << "Введите номер действия: ";

            std::string command_str;
            std::cin >> command_str;

            try {
                command = std::stoi(command_str);
            }
            catch (const std::invalid_argument& ia) {
                command = 0;
            }
            catch (const std::out_of_range& oor) {
                command = 0;
            }
            if (command != 1 && command != 2 && command != 3 && command != 4 && command != 5) {
                std::cerr << "Ошибка: Такого номера действия не существует." << std::endl;
            }
            if (command == 1) {
                std::cout << "Введите ключ (один символ): ";
                char k;
                std::cin >> k;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << std::endl;
                std::cout << "Ключ: '" << k << "'" << std::endl;
                std::cout << "Введите полином в строковом формате (например, x^2*y - 5*z + 3): ";
                std::string poly_string;
                std::getline(std::cin, poly_string);
                std::cout << std::endl;
                try {
                    PolynomialTranslyator translator(poly_string);
                    Polynomial poly_obj = translator.calculate();
                    table.insert(k, poly_obj);

                    std::cout << "Полином успешно добавлен/обновлен по ключу '" << k << "'." << std::endl;
                }
                catch (const std::exception& e) {
                    std::cerr << "Ошибка при парсинге или обработке полинома: " << e.what() << std::endl;
                    break;
                }
            }
            if (command == 2) {
                std::cout << "Введите ключ (один символ): ";
                char k;
                std::cin >> k;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << std::endl;
                std::cout << "Ключ: '" << k << "'" << std::endl;
                auto it = table.find(k);
                if (it != table.end()) {
                    std::cout << "Текущее значение для ключа '" << k << "': ";
                    std::cout << it->second << std::endl;
                }
                else {
                    std::cerr << "Ошибка: Не удалось найти только что вставленный полином по ключу '" << k << "'." << std::endl;
                }
            }
            if (command == 3) {
                std::cout << "Введите ключ (один символ): ";
                char k;
                std::cin >> k;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << std::endl;
                std::cout << "Ключ: '" << k << "'" << std::endl;
                table.erase(k);
            }
            if (command == 4) {
                table.print();
            }
        }
    }
    //---------------------------------------------------

    if (table_choice == 5) {
        std::cout << "Используется Хэш-таблица метод цепочек." << std::endl;
        std::cout << std::endl;
        ChainHashTable<char, Polynomial> table;
        int command = 0;
        while (command != 5) {
            std::cout << "Выберите действие:" << std::endl;
            std::cout << "1. Добавить/Обновить полином по ключу" << std::endl;
            std::cout << "2. Найти и вывести полином по ключу" << std::endl;
            std::cout << "3. Удалить полином по ключу" << std::endl;
            std::cout << "4. Вывести все полиномы" << std::endl;
            std::cout << "5. Выход" << std::endl;
            std::cout << std::endl;
            std::cout << "Введите номер действия: ";

            std::string command_str;
            std::cin >> command_str;

            try {
                command = std::stoi(command_str);
            }
            catch (const std::invalid_argument& ia) {
                command = 0;
            }
            catch (const std::out_of_range& oor) {
                command = 0;
            }
            if (command != 1 && command != 2 && command != 3 && command != 4 && command != 5) {
                std::cerr << "Ошибка: Такого номера действия не существует." << std::endl;
            }
            if (command == 1) {
                std::cout << "Введите ключ (один символ): ";
                char k;
                std::cin >> k;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << std::endl;
                std::cout << "Ключ: '" << k << "'" << std::endl;
                std::cout << "Введите полином в строковом формате (например, x^2*y - 5*z + 3): ";
                std::string poly_string;
                std::getline(std::cin, poly_string);
                std::cout << std::endl;
                try {
                    PolynomialTranslyator translator(poly_string);
                    Polynomial poly_obj = translator.calculate();
                    table.insert(k, poly_obj);

                    std::cout << "Полином успешно добавлен/обновлен по ключу '" << k << "'." << std::endl;
                }
                catch (const std::exception& e) {
                    std::cerr << "Ошибка при парсинге или обработке полинома: " << e.what() << std::endl;
                    break;
                }
            }
            if (command == 2) {
                std::cout << "Введите ключ (один символ): ";
                char k;
                std::cin >> k;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << std::endl;
                std::cout << "Ключ: '" << k << "'" << std::endl;
                auto it = table.find(k);
                if (it != table.end()) {
                    std::cout << "Текущее значение для ключа '" << k << "': ";
                    std::cout << it->second << std::endl;
                }
                else {
                    std::cerr << "Ошибка: Не удалось найти только что вставленный полином по ключу '" << k << "'." << std::endl;
                }
            }
            if (command == 3) {
                std::cout << "Введите ключ (один символ): ";
                char k;
                std::cin >> k;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << std::endl;
                std::cout << "Ключ: '" << k << "'" << std::endl;
                table.erase(k);
            }
            if (command == 4) {
                table.print();
            }
        }
    }
    //---------------------------------------------------

    if (table_choice == 6) {
        std::cout << "Используется Хэш-таблица с открытой адресацией." << std::endl;
        std::cout << std::endl;
        HashTableOpenAddressing<char, Polynomial> table;
        int command = 0;
        while (command != 5) {
            std::cout << "Выберите действие:" << std::endl;
            std::cout << "1. Добавить/Обновить полином по ключу" << std::endl;
            std::cout << "2. Найти и вывести полином по ключу" << std::endl;
            std::cout << "3. Удалить полином по ключу" << std::endl;
            std::cout << "4. Вывести все полиномы" << std::endl;
            std::cout << "5. Выход" << std::endl;
            std::cout << std::endl;
            std::cout << "Введите номер действия: ";

            std::string command_str;
            std::cin >> command_str;

            try {
                command = std::stoi(command_str);
            }
            catch (const std::invalid_argument& ia) {
                command = 0;
            }
            catch (const std::out_of_range& oor) {
                command = 0;
            }
            if (command != 1 && command != 2 && command != 3 && command != 4 && command != 5) {
                std::cerr << "Ошибка: Такого номера действия не существует." << std::endl;
            }
            if (command == 1) {
                std::cout << "Введите ключ (один символ): ";
                char k;
                std::cin >> k;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << std::endl;
                std::cout << "Ключ: '" << k << "'" << std::endl;
                std::cout << "Введите полином в строковом формате (например, x^2*y - 5*z + 3): ";
                std::string poly_string;
                std::getline(std::cin, poly_string);
                std::cout << std::endl;
                try {
                    PolynomialTranslyator translator(poly_string);
                    Polynomial poly_obj = translator.calculate();
                    table.insert(k, poly_obj);

                    std::cout << "Полином успешно добавлен/обновлен по ключу '" << k << "'." << std::endl;
                }
                catch (const std::exception& e) {
                    std::cerr << "Ошибка при парсинге или обработке полинома: " << e.what() << std::endl;
                    break;
                }
            }
            if (command == 2) {
                std::cout << "Введите ключ (один символ): ";
                char k;
                std::cin >> k;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << std::endl;
                std::cout << "Ключ: '" << k << "'" << std::endl;
                Polynomial* found_polynomial_ptr = table.find(k);
                if (found_polynomial_ptr != nullptr) {
                    std::cout << "Текущее значение для ключа '" << k << "': ";
                    std::cout << *found_polynomial_ptr << std::endl;
                }   
                else {
                    std::cerr << "Ошибка: Элемент с ключом '" << k << "' не найден после вставки." << std::endl;
                }
            }
            if (command == 3) {
                std::cout << "Введите ключ (один символ): ";
                char k;
                std::cin >> k;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << std::endl;
                std::cout << "Ключ: '" << k << "'" << std::endl;
                table.erase(k);
            }
            if (command == 4) {
                table.print();
            }
        }
    }
}