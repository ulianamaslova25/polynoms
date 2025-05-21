#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <cmath>    
#include <cctype>   
#include <sstream>  
#include <limits>   
#include <cstdlib>  

const int MIN_DEGREE = 0;
const int MAX_DEGREE = 9;
const double EPSILON = 1e-9;

struct Monomial {
    double coeff;
    int px, py, pz; 

    Monomial(double c = 0.0, int x = 0, int y = 0, int z = 0) : coeff(c), px(x), py(y), pz(z) {
        if (px < MIN_DEGREE || px > MAX_DEGREE ||
            py < MIN_DEGREE || py > MAX_DEGREE ||
            pz < MIN_DEGREE || pz > MAX_DEGREE) {
            throw std::out_of_range("Степень монома вне допустимого диапазона (0-9).");
        }
    }

    bool operator<(const Monomial& other) const {
        if (px != other.px) return px > other.px;
        if (py != other.py) return py > other.py;
        if (pz != other.pz) return pz > other.pz;
        return false; 
    }

    bool operator==(const Monomial& other) const {
        if (px != other.px || py != other.py || pz != other.pz) {
            return false;
        }
        return std::fabs(coeff - other.coeff) < EPSILON;
    }

    bool EqXYZ(const Monomial& other) const {
        return px == other.px && py == other.py && pz == other.pz;
    }

    Monomial operator*(const Monomial& other) const {
        if (px + other.px > MAX_DEGREE ||
            py + other.py > MAX_DEGREE ||
            pz + other.pz > MAX_DEGREE) {
            throw std::out_of_range("Умножение мономов приводит к степеням вне допустимого диапазона (0-9).");
        }
        return { coeff * other.coeff, px + other.px, py + other.py, pz + other.pz };
    }

    bool operator!=(const Monomial& other) const {
        return !(*this == other);
    }
};

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (std::string::npos == first) {
        return ""; 
    }
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

Monomial parseTerm(const std::string& term_str) {
    std::string s = trim(term_str);
    if (s.empty()) {
        throw std::runtime_error("Ошибка парсинга: Пустая строка монома.");
    }

    double sign_multiplier = 1.0;
    size_t current_pos = 0;

    if (s[current_pos] == '+' || s[current_pos] == '-') {
        if (s[current_pos] == '-') sign_multiplier = -1.0;
        current_pos++;
    }
    s = s.substr(current_pos); 
    s = trim(s); 

    double coeff_val = 1.0; 
    int px = 0, py = 0, pz = 0;
    bool x_seen = false, y_seen = false, z_seen = false; 

    current_pos = 0; 
    const char* start_ptr = s.c_str();
    char* end_ptr;
    coeff_val = std::strtod(start_ptr, &end_ptr);
    size_t processed_len = end_ptr - start_ptr; 

    bool number_was_parsed = false;
    if (processed_len > 0) {
        size_t pos_after_number_parse = processed_len;
        while (pos_after_number_parse < s.length() && std::isspace(s[pos_after_number_parse])) {
            pos_after_number_parse++;
        }
        if (pos_after_number_parse < s.length()) {
            char next_char = s[pos_after_number_parse];
            if (next_char == '*' || next_char == 'x' || next_char == 'y' || next_char == 'z') {
                number_was_parsed = true;
            }
            else {
                if (std::isdigit(s[0]) || (s[0] == '.' && processed_len > 1)) {
                    throw std::runtime_error("Ошибка парсинга: Неожиданные символы '" + s.substr(pos_after_number_parse) + "' после коэффициента в '" + term_str + "'.");
                }
                processed_len = 0; 
                coeff_val = 1.0; 
            }
        }
        else {
            if (s.length() > 0 && (std::isdigit(s[0]) || (s[0] == '.' && s.length() > 1 && std::isdigit(s[1])))) {
                number_was_parsed = true;
            }
            else {
                processed_len = 0; 
                coeff_val = 1.0;
            }
        }
    }


    if (number_was_parsed) {
        s = s.substr(processed_len);
        s = trim(s); 
        if (!s.empty() && s[0] == '*') {
            s = s.substr(1);
            s = trim(s); 
        }

    }
    else { 
        size_t first_var_char = s.find_first_of("xyz");
        if (first_var_char == 0) {
        }
        else if (s.empty()) {
            throw std::runtime_error("Ошибка парсинга: Строка монома пуста после знака/trim в '" + term_str + "'.");
        }
        else {
            throw std::runtime_error("Ошибка парсинга: Неверный формат начала монома после знака в '" + term_str + "'. Остаток: '" + s + "'");
        }
    }


    while (!s.empty()) {
        size_t var_pos_in_s = s.find_first_of("xyz");

        if (var_pos_in_s == std::string::npos) {
            bool only_asterisks_or_spaces = true;
            for (char ch : s) if (ch != '*' && !std::isspace(ch)) only_asterisks_or_spaces = false;

            if (!s.empty() && !only_asterisks_or_spaces) {
                throw std::runtime_error("Ошибка парсинга: Нераспознанные символы после переменных в '" + term_str + "'. Остаток: '" + s + "'");
            }
            break; 
        }

        std::string prefix = s.substr(0, var_pos_in_s);
        prefix = trim(prefix);
        bool only_asterisks_in_prefix = true;
        for (char ch : prefix) if (ch != '*') only_asterisks_in_prefix = false;
        if (!prefix.empty() && !only_asterisks_in_prefix) {
            throw std::runtime_error("Ошибка парсинга: Нераспознанные символы '" + prefix + "' перед переменной '" + std::string(1, s[var_pos_in_s]) + "' в '" + term_str + "'.");
        }

        char var_char = s[var_pos_in_s];
        s = s.substr(var_pos_in_s); 

        if ((var_char == 'x' && x_seen) || (var_char == 'y' && y_seen) || (var_char == 'z' && z_seen)) {
            throw std::runtime_error("Ошибка парсинга: Дублирование переменной '" + std::string(1, var_char) + "' в мономе '" + term_str + "'.");
        }

        int power = 1; 
        if (s.length() > 1 && s[1] == '^') {
            if (s.length() < 3 || !std::isdigit(s[2])) {
                throw std::runtime_error("Ошибка парсинга: Отсутствует или неверная степень после '^' для '" + std::string(1, var_char) + "' в '" + term_str + "'.");
            }
            power = s[2] - '0'; 
            if (power < MIN_DEGREE || power > MAX_DEGREE) { 
                throw std::out_of_range("Ошибка парсинга: Степень '" + std::string(1, s[2]) + "' вне допустимого диапазона (0-" + std::to_string(MAX_DEGREE) + ") для '" + std::string(1, var_char) + "' в '" + term_str + "'.");
            }
            s = s.substr(3); 
        }
        else {
            s = s.substr(1); 
        }

        if (var_char == 'x') { px = power; x_seen = true; }
        else if (var_char == 'y') { py = power; y_seen = true; }
        else if (var_char == 'z') { pz = power; z_seen = true; }

        s = trim(s); 

        if (!s.empty() && s[0] == '*') {
            s = s.substr(1);
            s = trim(s);
        }
    } 
    return { sign_multiplier * coeff_val, px, py, pz };
}


class Polynomial {
private:
    std::vector<Monomial> terms; 
    void canonicalize() {
        if (terms.empty()) {
            return;
        }
        std::sort(terms.begin(), terms.end());

        std::vector<Monomial> canonical_terms;

        canonical_terms.push_back(terms[0]);

        for (size_t i = 1; i < terms.size(); ++i) {
            if (terms[i].EqXYZ(canonical_terms.back())) {
                canonical_terms.back().coeff += terms[i].coeff;
            }
            else {
                canonical_terms.push_back(terms[i]);
            }
        }

        terms.clear(); 
        for (const auto& term : canonical_terms) {
            if (std::abs(term.coeff) > EPSILON) {
                terms.push_back(term); 
            }
        }
    }


public:
    Polynomial() {}

    Polynomial(std::vector<Monomial> mono_list) : terms(std::move(mono_list)) {
        canonicalize(); 
    }

    Polynomial(const Polynomial& other) = default;

    ~Polynomial() = default;

    Polynomial& operator=(const Polynomial& other) = default;
    Polynomial& operator=(Polynomial&& other) = default;

    bool operator==(const Polynomial& other) const {
        if (this->terms == other.terms) return true;
        return false;
    }

    // Оператор не равенства для полиномов
    bool operator!=(const Polynomial& other) const {
        return !(*this == other);
    }

    Polynomial operator+(const Polynomial& other) const {
        Polynomial result;
        result.terms.reserve(terms.size() + other.terms.size());
        result.terms.insert(result.terms.end(), terms.begin(), terms.end());
        result.terms.insert(result.terms.end(), other.terms.begin(), other.terms.end());
        result.canonicalize();
        return result; 
    }

    Polynomial operator-(const Polynomial& other) const {
        Polynomial result;
        result.terms.reserve(terms.size() + other.terms.size());
        result.terms.insert(result.terms.end(), terms.begin(), terms.end());
        for (const auto& term : other.terms) {
            result.terms.push_back({ -term.coeff, term.px, term.py, term.pz });
        }
        result.canonicalize();
        return result; 
    }

    Polynomial operator*(const Polynomial& other) const {
        Polynomial result;
        if (terms.empty() || other.terms.empty()) return Polynomial(); 
        result.terms.reserve(terms.size() * other.terms.size()); 

        for (const auto& term1 : terms) {
            if (std::abs(term1.coeff) < EPSILON) continue;
            for (const auto& term2 : other.terms) {
                if (std::abs(term2.coeff) < EPSILON) continue;
                try {
                    result.terms.push_back(term1 * term2); 
                }
                catch (const std::out_of_range& e) {

                    throw std::out_of_range("Ошибка умножения полиномов: " + std::string(e.what()));
                }
            }
        }
        result.canonicalize(); 
        return result; 
    }

    Polynomial operator*(double constant) const {
        Polynomial result;
        if (std::abs(constant) < EPSILON) return Polynomial(); 

        result.terms.reserve(terms.size());
        for (const auto& term : terms) {
            if (std::abs(term.coeff) > EPSILON) {
                result.terms.push_back({ term.coeff * constant, term.px, term.py, term.pz });
            }
        }
        result.canonicalize(); 
        return result; 
    }

    friend std::ostream& operator<<(std::ostream& ostr, const Polynomial& poly);
    friend std::istream& operator>>(std::istream& istr, Polynomial& poly);

    const std::vector<Monomial>& getTerms() const {
        return terms;
    }
};

std::ostream& operator<<(std::ostream& ostr, const Polynomial& poly) {
    if (poly.terms.empty()) {
        return ostr << "0";
    }
    bool first_term_printed = false;

    for (const auto& term : poly.terms) {
        if (std::abs(term.coeff) < EPSILON) continue;
        if (first_term_printed) {
            if (term.coeff > 0) ostr << " + ";
            else ostr << " - "; 
        }
        else {
            if (term.coeff < 0) ostr << "-";
        }

        double abs_coeff = std::fabs(term.coeff); 
        bool has_variables = (term.px > 0 || term.py > 0 || term.pz > 0);
        bool is_coeff_one_abs = std::fabs(abs_coeff - 1.0) < EPSILON;

        if (has_variables) { 
            if (!is_coeff_one_abs) {
                std::ostringstream coeff_ss;
                coeff_ss.precision(std::numeric_limits<double>::max_digits10);
                coeff_ss << abs_coeff;
                ostr << coeff_ss.str();
                ostr << "*";
            }
        }
        else { 
            std::ostringstream coeff_ss;
            coeff_ss.precision(std::numeric_limits<double>::max_digits10);
            coeff_ss << abs_coeff;
            ostr << coeff_ss.str();
        }

        bool first_var_printed = false; 
        if (term.px > 0) {
            ostr << "x";
            if (term.px > 1) ostr << "^" << term.px; 
            first_var_printed = true;
        }
        if (term.py > 0) {
            if (first_var_printed) ostr << "*"; 
            ostr << "y";
            if (term.py > 1) ostr << "^" << term.py;
            first_var_printed = true;
        }
        if (term.pz > 0) {
            if (first_var_printed) ostr << "*"; 
            ostr << "z";
            if (term.pz > 1) ostr << "^" << term.pz;
            first_var_printed = true;
        }
        first_term_printed = true;
    }
    return ostr;
}

std::istream& operator>>(std::istream& istr, Polynomial& poly) {
    std::string line;
    std::getline(istr, line);

    poly = Polynomial(); 

    if (line.empty() || line.find_first_not_of(" \t") == std::string::npos) {
        istr.clear();
        return istr;
    }

    std::vector<Monomial> parsed_monomials;
    size_t current_pos = 0;

    while (current_pos < line.length()) {
        current_pos = line.find_first_not_of(" \t", current_pos);
        if (current_pos == std::string::npos) break;

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
                if (potential_sep_pos > 0 && (line[potential_sep_pos - 1] == 'e' || line[potential_sep_pos - 1] == 'E')) {
                    search_pos = potential_sep_pos + 1;
                    continue;
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
            parsed_monomials.push_back(parseTerm(term_str));
        }
        catch (const std::runtime_error& e) {
            istr.setstate(std::ios_base::failbit);
            std::cerr << "Ошибка ввода: " << e.what() << std::endl;
            return istr;
        }
        catch (const std::out_of_range& e) {
            istr.setstate(std::ios_base::failbit);
            std::cerr << "Ошибка ввода (диапазон степеней): " << e.what() << std::endl;
            return istr;
        }


        current_pos = term_end_pos;
    }

    poly.terms = parsed_monomials;
    try {
        poly.canonicalize();
    }
    catch (const std::runtime_error& e) {
        istr.setstate(std::ios_base::failbit);
        std::cerr << "Внутренняя ошибка при канонизации после ввода: " << e.what() << std::endl;
    }

    return istr;
}


