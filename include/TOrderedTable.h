#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <utility> 

template<class TKey, class TValue>

class TOrderedTable {
    std::vector<std::pair<TKey, TValue>> table;

public:

    class iterator {
        std::pair<TKey, TValue>* it;

    public:
        iterator(std::pair<TKey, TValue>* ptr = nullptr) : it(ptr) {}
        std::pair<TKey, TValue>& operator*() const {
            if (!it) throw std::runtime_error("Разыменование нулевого итератора");
            return *it;
        }
        std::pair<TKey, TValue>* operator->() const {
            if (!it) throw std::runtime_error("Доступ через нулевой итератор");
            return it;
        }

        const TKey& key() const {
            if (!it) throw std::runtime_error("Доступ к ключу через нулевой итератор");
            return it->first;
        }

        TValue& value() const {
            if (!it) throw std::runtime_error("Доступ к значению через нулевой итератор");
            return it->second;
        }

        iterator& operator++() {
            if (it) it++;
            return *this; 
        }

        iterator operator++(int) {
            iterator tmp = *this;
            if (it) it++;
            return tmp;
        }

        bool operator==(const iterator& other) const {
            return it == other.it;
        }
        bool operator!=(const iterator& other) const {
            return it != other.it;
        }
        bool operator<(const iterator& other) const { return it < other.it; }
    };

    iterator begin() {
        return iterator(table.data());
    }

    iterator end() {
        return iterator(table.data() + table.size());
    }

    iterator find(const TKey& key) {
        for (size_t i = 0; i < table.size(); i++) {
            if (table[i].first == key) {
                return iterator(&table[i]);
            }
        }
        return end();
    };

    iterator insert(const TKey& key, const TValue& value) {
        size_t i = 0;
        while (i < table.size() && table[i].first < key) {
            i++;
        }
        if (i < table.size() && table[i].first == key) {
            table[i].second = value;
            return iterator(&table[i]);
        }
        table.emplace_back(key, value);
        std::move_backward(table.begin() + i, table.begin() + table.size() - 1, table.begin() + table.size());
        table[i] = std::pair<TKey, TValue>(key, value);
        return iterator(&table[i]);
    }

    iterator erase(const TKey& key) {
        if (table.empty()) {
            return end();
        }
        size_t i = 0;
        while (i < table.size()) {
            if (table[i].first == key) {
                std::move(table.begin() + i + 1, table.end(), table.begin() + i);
                table.pop_back();
                if (i < table.size()) {
                    return iterator(&table[i]);
                }
                else {
                    return end();
                }
            }
            if (table[i].first > key) {
                break;
            }
            i++;
        }
        return end();
    }

    bool empty() const {
        return table.empty();
    }

    size_t size() const {
        return table.size();
    }

    TValue& operator[](const TKey& key) {
        auto it = find(key); 
        if (it == end()) {
            throw std::out_of_range("Ключ не найден в TOrderedTable");
        }
        return it.value();
    }
    void print() const {
        std::cout << "--- Упорядоченная таблица (Размер: " << size() << ") ---\n";
        if (table.empty()) {
            std::cout << "Table is empty.\n";
        }
        else {
            bool first = true;
            for (const auto& pair : table) {
                if (!first) {
                    std::cout << ", ";
                }
                std::cout << "[" << pair.first << ": " << pair.second << "]";
                first = false;
            }
            std::cout << "\n"; 
        }
        std::cout << "-------------------------------------------\n";
    }
};



