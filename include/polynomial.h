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




