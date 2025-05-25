#pragma once
#include <vector>
#include <string>
#include <algorithm> 
#include <stdexcept>
#include <utility>

template<class TKey, class TValue>
class TUnorderedTable {
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

        TValue& value() {
            if (!it) throw std::runtime_error("Доступ к значению через нулевой итератор");
            return it->second;
        }

        const TValue& value() const {
            if (!it) throw std::runtime_error("Доступ к значению через нулевой итератор");
            return it->second;
        }

        iterator& operator++() {
            if (it) ++it;
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            if (it) ++it; 
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

private:
    std::pair<TKey, TValue>* find_ptr(const TKey& key) {
        for (size_t i = 0; i < table.size(); ++i) {
            if (table[i].first == key) {
                return &table[i];
            }
        }
        return nullptr;
    }

    const std::pair<TKey, TValue>* find_ptr(const TKey& key) const {
        for (size_t i = 0; i < table.size(); ++i) {
            if (table[i].first == key) {
                return &table[i];
            }
        }
        return nullptr;
    }

public:
    iterator find(const TKey& key) {
        auto vec_iterator = std::find_if(
            this->table.begin(), 
            this->table.end(),   
            [&key](const std::pair<TKey, TValue>& elem) {
                return elem.first == key;
            }
        );

        if (vec_iterator != this->table.end()) { 
            return iterator(&(*vec_iterator));
        }
        return this->end(); 
    }

    iterator insert(const TKey& key, const TValue& value) {
        iterator existing_it = find(key); 
        if (existing_it != end()) {     
            existing_it.value() = value;
            return existing_it;
        }

        table.emplace_back(key, value);
        return iterator(&table.back());
    }

    iterator erase(const TKey& key) {
        auto vec_it_to_erase = std::find_if(
            this->table.begin(), 
            this->table.end(),   
            [&key](const std::pair<TKey, TValue>& elem) {
                return elem.first == key;
            });

        if (vec_it_to_erase != this->table.end()) { 
            std::pair<TKey, TValue>* current_element_ptr = &(*vec_it_to_erase);
            std::pair<TKey, TValue>* last_element_ptr = &table.back();

            bool was_last = (current_element_ptr == last_element_ptr);

            if (!was_last) {
                std::swap(*current_element_ptr, *last_element_ptr);
            }
            table.pop_back();

            if (table.empty()) {
                return this->end();
            }
            if (was_last) { 
                return this->end();
            }
            else {
                return iterator(current_element_ptr);
            }
        }
        return this->end(); 
    }

    bool empty() const {
        return table.empty();
    }

    size_t size() const {
        return table.size();
    }

    TValue& operator[](const TKey& key) {
        iterator it = find(key); 
        if (it == end()) {       
            throw std::out_of_range("Ключ не найден в TUnorderedTable");
        }
        return it.value();
    }

    void print() const {
        std::cout << "--- Неупорядоченная таблица (Размер: " << size() << ") ---\n";
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