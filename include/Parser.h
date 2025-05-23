#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <vector>
#include <stdexcept>
#include <algorithm> // Для std::min
#include <cmath>     // Для std::fabs

// Включаем основной файл с определениями Monomial и Polynomial
#include "polynomial.h" // Нужен для Monomial, Polynomial, MIN/MAX_DEGREE, EPSILON

// Вспомогательная функция для удаления пробелов по краям строки
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (std::string::npos == first) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

// Функция для парсинга одного члена полинома (монома)
// Возвращает Monomial. Может выбрасывать исключения std::runtime_error или std::out_of_range.
Monomial parseTerm(const std::string& term_str) {
    std::string s = trim(term_str);
    if (s.empty()) {
        // Пустая строка после trim - ошибка формата на этом этапе
        throw std::runtime_error("Ошибка парсинга члена: Пустая строка монома после знака/trim.");
    }

    double sign_multiplier = 1.0;
    size_t current_pos = 0;

    if (s[current_pos] == '+' || s[current_pos] == '-') {
        if (s[current_pos] == '-') sign_multiplier = -1.0;
        current_pos++;
    }
    s = s.substr(current_pos);
    s = trim(s);

    if (s.empty()) {
        throw std::runtime_error("Ошибка парсинга члена: Неполный моном после знака в '" + term_str + "'.");
    }

    double coeff_val = 1.0;
    int px = 0, py = 0, pz = 0;
    bool x_seen = false, y_seen = false, z_seen = false;

    current_pos = 0;
    const char* start_ptr = s.c_str();
    char* end_ptr;

    double parsed_coeff = std::strtod(start_ptr, &end_ptr);
    size_t processed_len = end_ptr - start_ptr;

    bool number_was_parsed = false;
    if (processed_len > 0) {
        if (processed_len == s.length()) {
            coeff_val = parsed_coeff;
            number_was_parsed = true;
            s = "";
        }
        else {
            size_t pos_after_number = processed_len;
            while (pos_after_number < s.length() && std::isspace(s[pos_after_number])) {
                pos_after_number++;
            }
            if (pos_after_number < s.length()) {
                char next_char = s[pos_after_number];
                if (next_char == '*' || next_char == 'x' || next_char == 'y' || next_char == 'z') {
                    coeff_val = parsed_coeff;
                    number_was_parsed = true;
                    s = s.substr(pos_after_number);
                    s = trim(s);
                }
                else {
                    if (std::isdigit(s[0]) || (s[0] == '.' && processed_len > 1)) {
                        throw std::runtime_error("Ошибка парсинга члена: Неожиданные символы '" + s.substr(pos_after_number) + "' после коэффициента в '" + term_str + "'.");
                    }
                    processed_len = 0;
                    coeff_val = 1.0;
                }
            }
            else {
                coeff_val = parsed_coeff;
                number_was_parsed = true;
                s = "";
            }
        }
    }
    else {
        coeff_val = 1.0;
    }

    if (!s.empty() && s[0] == '*') {
        s = s.substr(1);
        s = trim(s);
    }

    while (!s.empty()) {
        size_t var_pos_in_s = std::string::npos;
        size_t pos_x = s.find('x');
        size_t pos_y = s.find('y');
        size_t pos_z = s.find('z');

        if (pos_x != std::string::npos) var_pos_in_s = pos_x;
        if (pos_y != std::string::npos) {
            if (var_pos_in_s == std::string::npos || pos_y < var_pos_in_s) var_pos_in_s = pos_y;
        }
        if (pos_z != std::string::npos) {
            if (var_pos_in_s == std::string::npos || pos_z < var_pos_in_s) var_pos_in_s = pos_z;
        }


        if (var_pos_in_s == std::string::npos) {
            bool only_asterisks_or_spaces = true;
            for (char ch : s) if (ch != '*' && !std::isspace(ch)) only_asterisks_or_spaces = false;

            if (!s.empty() && !only_asterisks_or_spaces) {
                throw std::runtime_error("Ошибка парсинга члена: Нераспознанные символы '" + s + "' после переменных в '" + term_str + "'.");
            }
            break;
        }

        std::string prefix = s.substr(0, var_pos_in_s);
        prefix = trim(prefix);
        bool only_asterisks_in_prefix = true;
        for (char ch : prefix) if (ch != '*') only_asterisks_in_prefix = false;
        if (!prefix.empty() && !only_asterisks_in_prefix) {
            throw std::runtime_error("Ошибка парсинга члена: Нераспознанные символы '" + prefix + "' перед переменной '" + std::string(1, s[var_pos_in_s]) + "' в '" + term_str + "'.");
        }

        char var_char = s[var_pos_in_s];
        s = s.substr(var_pos_in_s + 1);

        if ((var_char == 'x' && x_seen) || (var_char == 'y' && y_seen) || (var_char == 'z' && z_seen)) {
            throw std::runtime_error("Ошибка парсинга члена: Дублирование переменной '" + std::string(1, var_char) + "' в мономе '" + term_str + "'.");
        }

        int power = 1;
        s = trim(s);

        if (!s.empty() && s[0] == '^') {
            s = s.substr(1);
            s = trim(s);
            if (s.empty() || !std::isdigit(s[0])) {
                throw std::runtime_error("Ошибка парсинга члена: Отсутствует или неверная степень после '^' для '" + std::string(1, var_char) + "' в '" + term_str + "'.");
            }
            power = s[0] - '0';
            if (power < MIN_DEGREE || power > MAX_DEGREE) {
                throw std::out_of_range("Ошибка парсинга члена: Степень '" + std::string(1, s[0]) + "' вне допустимого диапазона (" + std::to_string(MIN_DEGREE) + "-" + std::to_string(MAX_DEGREE) + ") для '" + std::string(1, var_char) + "' в '" + term_str + "'.");
            }
            s = s.substr(1);
            s = trim(s);
        }

        if (var_char == 'x') { px = power; x_seen = true; }
        else if (var_char == 'y') { py = power; y_seen = true; }
        else if (var_char == 'z') { pz = power; z_seen = true; }

        if (!s.empty() && s[0] == '*') {
            s = s.substr(1);
            s = trim(s);
        }
    }
    return { sign_multiplier * coeff_val, px, py, pz };
}

// Функция для парсинга строки, содержащей представление полинома.
// Возвращает объект Polynomial. Может выбрасывать исключения.
Polynomial parsePolynomial(const std::string& input_string) {
    std::string line = trim(input_string);

    if (line.empty()) {
        return Polynomial(); // Пустая строка соответствует нулевому полиному
    }

    std::vector<Monomial> parsed_monomials;
    size_t current_pos = 0;

    size_t first_term_start = line.find_first_not_of(" \t");
    if (first_term_start == std::string::npos) {
        return Polynomial(); // Строка только из пробелов
    }

    current_pos = first_term_start;

    while (current_pos < line.length()) {

        size_t term_end_pos = std::string::npos;
        size_t search_pos = current_pos + 1;

        while (search_pos < line.length()) {
            size_t next_plus = line.find('+', search_pos);
            size_t next_minus = line.find('-', search_pos);

            size_t potential_sep_pos = std::string::npos;
            if (next_plus == std::string::npos) potential_sep_pos = next_minus;
            else if (next_minus == std::string::npos) potential_sep_pos = next_plus;
            else potential_sep_pos = std::min(next_plus, next_minus);

            if (potential_sep_pos == std::string::npos) {
                term_end_pos = line.length();
                break;
            }

            if (line[potential_sep_pos] == '-') {
                if (potential_sep_pos > 0) {
                    char char_before = line[potential_sep_pos - 1];
                    if (char_before == 'e' || char_before == 'E') {
                        search_pos = potential_sep_pos + 1;
                        continue;
                    }
                }
            }

            term_end_pos = potential_sep_pos;
            break;
        }

        if (term_end_pos == std::string::npos) {
            term_end_pos = line.length();
        }

        std::string term_str = line.substr(current_pos, term_end_pos - current_pos);

        try {
            Monomial m = parseTerm(term_str);
            if (std::fabs(m.coeff) > EPSILON) {
                parsed_monomials.push_back(m);
            }
        }
        catch (const std::runtime_error& e) {
            // Перебрасываем исключение парсинга члена с добавлением контекста
            throw std::runtime_error("Ошибка при парсинге члена '" + term_str + "': " + std::string(e.what()));
        }
        catch (const std::out_of_range& e) {
            // Перебрасываем исключение диапазона степеней
            throw std::out_of_range("Ошибка диапазона степеней при парсинге члена '" + term_str + "': " + std::string(e.what()));
        }
        catch (const std::exception& e) {
            // Ловим любые другие неожиданные исключения
            throw std::runtime_error("Неизвестная ошибка при парсинге члена '" + term_str + "': " + std::string(e.what()));
        }


        current_pos = term_end_pos;
    }

    // Создаем полином из разобранных мономов.
    // Конструктор Polynomial(vector<Monomial>) автоматически вызывает canonicalize.
    return Polynomial(parsed_monomials);
}


// Оператор ввода для полинома.
// Остается здесь, т.к. это стандартный способ парсинга из потоков.
// Он вызывает parsePolynomial и обрабатывает ошибки потока.
std::istream& operator>>(std::istream& istr, Polynomial& poly) {
    std::string line;
    // Читаем всю строку до символа новой строки
    if (!std::getline(istr, line)) {
        // Если чтение из потока не удалось (конец файла, ошибка), устанавливаем failbit.
        istr.setstate(std::ios_base::failbit);
        return istr;
    }

    try {
        // Используем новую функцию parsePolynomial для парсинга строки
        poly = parsePolynomial(line);
        // Если парсинг успешен, полином poly обновлен, поток в goodbit состоянии (если был до getline).
    }
    catch (const std::runtime_error& e) {
        // Если parsePolynomial выбросил исключение (ошибка формата),
        // устанавливаем failbit в потоке и выводим сообщение об ошибке.
        istr.setstate(std::ios_base::failbit);
        std::cerr << "Ошибка ввода: " << e.what() << std::endl;
    }
    catch (const std::out_of_range& e) {
        // Ошибка диапазона степеней
        istr.setstate(std::ios_base::failbit);
        std::cerr << "Ошибка ввода (диапазон степеней): " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        // Любое другое исключение из parsePolynomial
        istr.setstate(std::ios_base::failbit);
        std::cerr << "Ошибка ввода: " << e.what() << std::endl;
    }


    return istr;
}


