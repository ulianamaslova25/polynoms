#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <algorithm>
#include <stdexcept>
#include <cctype> 
#include <cmath> 

// Включаем заголовочный файл с определением Polynomial и Monomial
// (а он, в свою очередь, включает PolynomialParser.h с parsePolynomial)
#include "polynomial.h"
#include "Parser.h"

class PolynomialTranslyator {
private:
    std::vector<std::string> data; // Токены: либо строки операторов, либо строковое представление полинома

    const std::string operators = "+-*/()";

    // Вспомогательная функция для проверки, является ли токен одним из определенных операторов или скобок
    bool isOperator(const std::string& str) const {
        return str.length() == 1 && operators.find(str[0]) != std::string::npos;
    }

    // Вспомогательная функция для проверки, является ли токен бинарным оператором (+, -, *, /)
    bool isBinaryOperator(const std::string& str) const {
        return str.length() == 1 && (str == "+" || str == "-" || str == "*" || str == "/");
    }

    // Вспомогательная функция для проверки, является ли токен оператором умножения или деления (*, /)
    bool isMultDivOperator(const std::string& str) const {
        return str.length() == 1 && (str == "*" || str == "/");
    }


    // Вспомогательная функция для проверки, является ли токен строковым представлением полинома
    // Это любой токен, который не является распознанным оператором/скобкой
    bool isPolynomialToken(const std::string& str) const {
        return !isOperator(str);
    }

    // Возвращает приоритет оператора
    int imp(const std::string& op) const {
        if (op == "+" || op == "-") return 1;
        if (op == "*" || op == "/") return 2;
        return 0; // Для скобок или других не-операторов
    }

    // Вспомогательная функция для токенизации входной строки
    void tokenize(const std::string& str) {
        std::string currentToken;
        // Отслеживаем, был ли предыдущий НЕ пробельный символ оператором или открывающей скобкой.
        // Это нужно для определения унарного плюса/минуса.
        bool lastNonSpaceCharWasOperatorOrOpenParen = true;

        for (char c : str) {
            if (std::isspace(c)) {
                // Игнорируем пробелы, не меняя состояние lastNonSpaceCharWasOperatorOrOpenParen
                continue;
            }

            if (isOperator(std::string(1, c))) {
                // Это оператор или скобка
                if (!currentToken.empty()) {
                    // Завершаем текущий токен (это должно быть строковое представление полинома)
                    data.push_back(currentToken);
                    currentToken.clear();
                }
                // Добавляем оператор как отдельный токен
                data.push_back(std::string(1, c));

                // Обновляем состояние для следующего символа
                if (c == '(' || c == '+' || c == '-') { // + и - могут быть унарными после '('
                    lastNonSpaceCharWasOperatorOrOpenParen = true;
                }
                else { // *, /, ) не могут предварять унарные операторы или начало полинома без знака
                    lastNonSpaceCharWasOperatorOrOpenParen = false;
                }
            }
            else {
                // Это часть строкового представления полинома (цифра, буква, точка, степень...)
                currentToken += c;
                lastNonSpaceCharWasOperatorOrOpenParen = false; // Предыдущий символ не был оператором/скобкой
            }
        }
        // Добавляем последний токен после завершения цикла
        if (!currentToken.empty()) {
            data.push_back(currentToken);
        }
    }


    // Вспомогательная функция для валидации токенизированного выражения
    void validateTokens() const {
        if (data.empty()) {
            throw std::runtime_error("Неверное выражение: Отсутствуют токены.");
        }

        // Проверка 1: Начальный/Конечный токены
        // Выражение не может начинаться с бинарного оператора (*, /) или закрывающей скобки
        if (isBinaryOperator(data.front()) || data.front() == ")") {
            throw std::runtime_error("Неверное выражение: Начинается с недопустимого токена '" + data.front() + "'.");
        }
        // Выражение не может заканчиваться бинарным оператором (*, /) или открывающей скобкой
        if (isBinaryOperator(data.back()) || data.back() == "(") {
            throw std::runtime_error("Неверное выражение: Заканчивается недопустимым токеном '" + data.back() + "'.");
        }


        // Проверка 2: Баланс скобок
        int cnt_balance = 0;
        for (const std::string& s : data) {
            if (s == "(") cnt_balance++;
            else if (s == ")") cnt_balance--;
            if (cnt_balance < 0) throw std::runtime_error("Неверное выражение: Несоответствие скобок (лишняя закрывающая скобка).");
        }
        if (cnt_balance != 0) throw std::runtime_error("Неверное выражение: Несоответствие скобок (незакрытая открывающая скобка).");

        // Проверка 3: Пустые скобки ()
        for (size_t i = 0; i + 1 < data.size(); ++i) {
            if (data[i] == "(" && data[i + 1] == ")") {
                throw std::runtime_error("Неверное выражение: Пустые скобки.");
            }
        }

        // Проверка 4: Недопустимые последовательности токенов
        for (size_t i = 0; i + 1 < data.size(); ++i) {
            const std::string& current = data[i];
            const std::string& next = data[i + 1];

            // Много-символьные токены (представления полиномов) могут содержать знаки +/-, но не как отдельные токены рядом с бинарными операторами.
            // Наша токенизация уже должна была разделить их.
            // Теперь проверяем сочетания типов токенов.

            // Полином за открывающей скобкой (неявное умножение 5(x+y) - не поддерживается)
            if (isPolynomialToken(current) && next == "(") {
                throw std::runtime_error("Неверное выражение: Полином за открывающей скобкой (неявное умножение не поддерживается).");
            }

            // Закрывающая скобка за полиномом (неявное умножение (x+y)5 - не поддерживается)
            if (current == ")" && isPolynomialToken(next)) {
                throw std::runtime_error("Неверное выражение: Закрывающая скобка за полиномом (неявное умножение не поддерживается).");
            }

            // Последовательные полиномы без оператора (неявное умножение x^2 y^3 - не поддерживается)
            if (isPolynomialToken(current) && isPolynomialToken(next)) {
                throw std::runtime_error("Неверное выражение: Последовательные полиномы без оператора (неявное умножение не поддерживается).");
            }


            // Последовательные бинарные операторы (например, +* , /- )
            if (isBinaryOperator(current) && isBinaryOperator(next)) {
                // Специально для операторов + и -: ++, +-, --, -+ разрешены, если рассматриваются как унарные знаки в составе полинома.
                // НО, если они были отделены токенизацией, как у нас, то последовательные бинарные операторы - это ошибка.
                throw std::runtime_error("Неверное выражение: Последовательные бинарные операторы '" + current + next + "'.");
            }

            // Открывающая скобка, за которой следует бинарный оператор (*, /)
            if (current == "(" && isMultDivOperator(next)) {
                throw std::runtime_error("Неверное выражение: Открывающая скобка за оператором умножения/деления.");
            }
            // Открывающая скобка, за которой следует знак + или - - это унарный знак, он должен быть частью следующего полином-токена
            // Наша токенизация этого не позволит.
            // if (current == "(" && (next == "+" || next == "-")) { // Эта проверка не нужна при текущей токенизации
            //    throw std::runtime_error("Неверное выражение: Открывающая скобка за унарным оператором.");
            // }

           // Бинарный оператор за закрывающей скобкой (например, (3 + ))
            if (isBinaryOperator(current) && next == ")") {
                throw std::runtime_error("Неверное выражение: Бинарный оператор за закрывающей скобкой.");
            }

            // Закрывающая скобка за открывающей скобкой (()) - уже проверено в п.3
            // if (current == ")" && next == "(") { ... } // Неявное умножение - уже проверено выше
        }

        // Проверка 5: Токены-полиномы должны быть парсибельны
        // Это будет проверено непосредственно при вычислении методом calculate(),
        // вызывая parsePolynomial для каждого полином-токена.
    }


public:
    // Конструктор принимает строку и парсит/валидирует её
    explicit PolynomialTranslyator(const std::string& str) {
        std::string trimmed_str = str; // std::string str = trim(input_str); // Если нужна функция trim

        // Проверка на пустую строку или строку только из пробелов перед токенизацией
        if (trimmed_str.empty() || trimmed_str.find_first_not_of(" \t") == std::string::npos) {
            // Допустимо для создания пустого объекта, но не для вычисления
            // Валидация calculate() поймает это при попытке вычисления
            // Или можно выбросить исключение здесь, если пустая строка считается неверным выражением.
            // Давайте разрешим пустую строку для создания объекта, но запретим вычисление.
        }
        else {
            // Парсим строку на токены
            tokenize(trimmed_str);
            // Валидируем последовательность токенов
            validateTokens();
        }
    }

    // Конструктор по умолчанию для создания пустого транслятора
    PolynomialTranslyator() = default;


    // Вычисляет значение выражения - возвращает Полином
    Polynomial calculate() const {
        if (data.empty()) {
            // Выбрасываем ошибку для пустого выражения при попытке вычисления
            return Polynomial(); // Можно вернуть нулевой полином для пустого выражения
        }

        std::stack<Polynomial> operands; // Стек операндов (Полиномов)
        std::stack<std::string> operators; // Стек операторов

        // Лямбда-функция для выполнения операции над Полиномами
        auto counter = [&]() {
            if (operands.size() < 2) {
                throw std::runtime_error("Ошибка вычисления: Недостаточно операндов для оператора.");
            }
            Polynomial operand2 = operands.top(); operands.pop(); // Второй операнд (сверху стека)
            Polynomial operand1 = operands.top(); operands.pop(); // Первый операнд
            if (operators.empty()) {
                throw std::runtime_error("Ошибка вычисления: Стек операторов пуст при попытке выполнения операции.");
            }
            std::string op = operators.top(); operators.pop(); // Оператор

            // Выполнение операции над Полиномами
            if (op == "+") operands.push(operand1 + operand2);
            else if (op == "-") operands.push(operand1 - operand2);
            else if (op == "*") operands.push(operand1 * operand2);
            else if (op == "/") {
                // Деление полиномов не поддерживается
                throw std::runtime_error("Ошибка вычисления: Оператор деления '/' не поддерживается для полиномов.");
            }
            else {
                // Не должно произойти при валидных входных данных
                throw std::runtime_error("Ошибка вычисления: Неизвестный оператор '" + op + "' в стеке.");
            }
            };

        // Алгоритм Shunting-Yard для вычисления инфиксного выражения
        for (const std::string& i : data) {
            if (isPolynomialToken(i)) { // Если токен - строковое представление полинома
                try {
                    // Парсим строку в объект Polynomial и помещаем в стек операндов
                    operands.push(parsePolynomial(i));
                }
                catch (const std::runtime_error& e) {
                    // Перехватываем ошибки парсинга полинома и перебрасываем с контекстом
                    throw std::runtime_error("Ошибка вычисления: Ошибка парсинга полинома '" + i + "': " + std::string(e.what()));
                }
                catch (const std::out_of_range& e) {
                    // Перехватываем ошибки диапазона степеней при парсинге
                    throw std::out_of_range("Ошибка вычисления: Ошибка диапазона степеней в полиноме '" + i + "': " + std::string(e.what()));
                }
                catch (const std::exception& e) {
                    // Ловим другие возможные исключения из parsePolynomial
                    throw std::runtime_error("Ошибка вычисления: Неизвестная ошибка при парсинге полинома '" + i + "': " + std::string(e.what()));
                }
            }
            else if (i == "(") { // Если открывающая скобка
                operators.push(i); // Помещаем в стек операторов
            }
            else if (i == ")") { // Если закрывающая скобка
                // Выполняем операции из стека операторов до первой открывающей скобки
                while (!operators.empty() && operators.top() != "(") {
                    counter();
                }
                if (operators.empty()) {
                    // Этот случай должен быть перехвачен валидацией баланса скобок, но это страховка
                    throw std::runtime_error("Ошибка вычисления: Несоответствие скобок (нет соответствующей открывающей скобки в стеке).");
                }
                operators.pop(); // Удаляем открывающую скобку из стека
            }
            else if (isBinaryOperator(i)) { // Если бинарный оператор (+, -, *, /)
                if (i == "/") {
                    // Запрещаем оператор деления
                    throw std::runtime_error("Ошибка вычисления: Оператор деления '/' не поддерживается для полиномов.");
                }
                // Выполняем операции из стека операторов с более высоким или равным приоритетом
                while (!operators.empty() && operators.top() != "(" && imp(operators.top()) >= imp(i)) {
                    counter();
                }
                operators.push(i); // Помещаем текущий оператор в стек
            }
            else {
                // Этот случай должен быть перехвачен валидацией в конструкторе
                throw std::runtime_error("Ошибка вычисления: Неожиданный токен '" + i + "' в процессе Shunting-Yard.");
            }
        }

        // После обработки всех токенов, выполняем оставшиеся операции в стеке операторов
        while (!operators.empty()) {
            if (operators.top() == "(") {
                // Этот случай должен быть перехвачен валидацией баланса скобок
                throw std::runtime_error("Ошибка вычисления: Несоответствие скобок (незакрытая открывающая скобка осталась в стеке операторов).");
            }
            counter();
        }

        // В конце в стеке операндов должен остаться ровно один результат (Полином)
        if (operands.size() != 1 || !operators.empty()) {
            throw std::runtime_error("Ошибка вычисления: Неверный формат выражения, приведший к некорректному состоянию стеков в конце.");
        }

        return operands.top(); // Возвращаем финальный результат (Полином)
    }

    // Перегрузка оператора << для вывода токенизированного выражения
    friend std::ostream& operator<<(std::ostream& ostr, const PolynomialTranslyator& other) {
        if (other.data.empty()) {
            ostr << "[Выражение пустое/невалидное]";
        }
        else {
            for (const auto& item : other.data) {
                ostr << "\"" << item << "\" "; // Выводим токены в кавычках для наглядности
            }
        }
        return ostr;
    }

    // Метод для вывода результата вычисления с обработкой ошибок
    void printResult() const {
        try {
            Polynomial result = calculate();
            std::cout << result << std::endl; // Используем operator<< для Polynomial
        }
        catch (const std::exception& error) {
            // Перехватываем все исключения, выбрасываемые из calculate()
            std::cerr << "Ошибка вычисления: " << error.what() << std::endl;
        }
    }

    // Опционально: Добавить геттер для токенизированных данных для тестирования/проверки
    const std::vector<std::string>& getTokens() const {
        return data;
    }
};


