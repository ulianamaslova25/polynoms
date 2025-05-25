#pragma once

#include <vector>     
#include <list>       
#include <utility>    
#include <functional> 
#include <iostream>   
#include <cmath>      
#include <stdexcept>  

inline size_t find_next_prime(size_t n) {
    if (n <= 1) return 2;
    if (n == 2) return 3;
    size_t prime = (n % 2 == 0) ? n + 1 : n;

    while (true) {
        bool found = true;
        size_t limit = static_cast<size_t>(std::sqrt(static_cast<double>(prime)));
        for (size_t i = 3; i <= limit; i += 2) {
            if (prime % i == 0) {
                found = false;
                break;
            }
        }
        if (found) return prime;
        prime += 2;
    }
}

template<typename K, typename V>
class ChainHashTable {
public:
    class iterator;
private:
    std::vector<std::list<std::pair<const K, V>>> table;

    size_t capacity;     
    size_t num_elements; 

    std::hash<K> hasher;

    size_t hash_index(const K& key) const {
        return hasher(key) % capacity;
    }

public:
    class iterator {
        friend class ChainHashTable<K, V>;

    private:
        ChainHashTable<K, V>* table_ptr;
        size_t bucket_index;
        typename std::list<std::pair<const K, V>>::iterator list_iterator;

        iterator(ChainHashTable<K, V>* ptr, size_t b_idx, typename std::list<std::pair<const K, V>>::iterator l_it)
            : table_ptr(ptr), bucket_index(b_idx), list_iterator(l_it) {
        }

        void increment() {
            if (table_ptr == nullptr || bucket_index >= table_ptr->capacity) {
                return;
            }
            ++list_iterator;
            if (list_iterator == table_ptr->table[bucket_index].end()) {
                bucket_index++;
                while (bucket_index < table_ptr->capacity && table_ptr->table[bucket_index].empty()) {
                    bucket_index++;
                }
                if (bucket_index < table_ptr->capacity) {
                    list_iterator = table_ptr->table[bucket_index].begin();
                }
                else {
                }
            }
        }

    public:
        iterator() : table_ptr(nullptr), bucket_index(0) {}

        std::pair<const K, V>& operator*() const {
            if (table_ptr == nullptr || bucket_index >= table_ptr->capacity) {
                throw std::out_of_range("Разыменование end-итератора или невалидного итератора");
            }
            return *list_iterator;
        }

        std::pair<const K, V>* operator->() const {
            if (table_ptr == nullptr || bucket_index >= table_ptr->capacity) {
                throw std::out_of_range("Разыменование end-итератора или невалидного итератора");
            }
            return &*list_iterator;
        }

        iterator& operator++() {
            increment();
            return *this;
        }

        iterator operator++(int) {
            iterator temp = *this;
            increment();
            return temp;
        }

        bool operator==(const iterator& other) const {
            if (table_ptr != other.table_ptr) return false;

            bool this_is_end = (table_ptr != nullptr && bucket_index >= table_ptr->capacity) || table_ptr == nullptr;
            bool other_is_end = (other.table_ptr != nullptr && other.bucket_index >= other.table_ptr->capacity) || other.table_ptr == nullptr;

            if (this_is_end && other_is_end) {
                return true; 
            }
            if (this_is_end || other_is_end) {
                return false; 
            }

            return bucket_index == other.bucket_index && list_iterator == other.list_iterator;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }
    };

    explicit ChainHashTable(size_t initial_capacity = 11)
        : capacity(find_next_prime(initial_capacity > 0 ? initial_capacity : 1)),
        num_elements(0), 
        hasher()         
    {
        table.resize(capacity);
    }
    ~ChainHashTable() = default;

    iterator insert(const K& key, const V& value) {
        size_t index = hash_index(key); 
        auto& bucket = table[index];    

        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->first == key) {
                throw "the element with this key already exists";
            }
        }
        bucket.push_back({ key, value });
        num_elements++; 
        auto inserted_it = --bucket.end();
        return iterator(this, index, inserted_it);
    }

    iterator find(const K& key) {
        if (empty()) return end(); 

        size_t index = hash_index(key); 
        auto& bucket = table[index]; 

        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->first == key) {
                return iterator(this, index, it); 
            }
        }

        return end(); 
    }

    iterator erase(const K& key) {
        iterator it = this->find(key);
        if (it == end()) {
            throw std::out_of_range("Попытка удалить с использованием end-итератора");
        }
        if (it.table_ptr != this) {
            throw std::invalid_argument("Попытка удалить с использованием итератора из другой таблицы");
        }

        size_t index = it.bucket_index; 
        auto& bucket = table[index];    
        typename std::list<std::pair<const K, V>>::iterator list_it = it.list_iterator; 

        auto next_list_it = bucket.erase(list_it); 
        num_elements--; 

        if (next_list_it != bucket.end()) {
            return iterator(this, index, next_list_it);
        }
        else {
            size_t next_bucket_index = index + 1;
            while (next_bucket_index < capacity && table[next_bucket_index].empty()) {
                next_bucket_index++;
            }
            if (next_bucket_index < capacity) {
                return iterator(this, next_bucket_index, table[next_bucket_index].begin());
            }
            else {
                return end();
            }
        }
    }

    V& operator[](const K& key) {
        if (this->find(key) == this->end()) {
            return (*this->insert(key, V())).second;
        }
        return (*(this->find(key))).second;
    }

    V operator[](const K& key) const {
        if (this->find(key) == this->end()) throw "an element with such key doesnt exist";
        return (*(this->find(key))).second;
    }

    iterator begin() {
        size_t first_bucket = 0; 
        while (first_bucket < capacity && table[first_bucket].empty()) {
            first_bucket++; 
        }
        if (first_bucket < capacity) {
            return iterator(this, first_bucket, table[first_bucket].begin());
        }
        else {
            return end(); 
        }
    }

    iterator end() {
        return iterator(this, capacity, typename std::list<std::pair<const K, V>>::iterator());
    }

    size_t size() const {
        return num_elements;
    }

    bool empty() const {
        return num_elements == 0;
    }

    size_t getCapacity() const {
        return capacity;
    }

    void print() const {
        std::cout << "--- Базовая Хеш-таблица с итератором (Размер: " << size() << ", Емкость: " << getCapacity() << ") ---\n";
        for (size_t i = 0; i < capacity; ++i) {
            std::cout << "Корзина " << i << ": ";
            if (table[i].empty()) {
                std::cout << "пусто\n";
            }
            else {
                bool first_in_bucket = true;
                for (const auto& pair : table[i]) {
                    if (!first_in_bucket) std::cout << " -> ";
                    std::cout << "[" << pair.first << ": " << pair.second << "]";
                    first_in_bucket = false;
                }
                std::cout << " -> nullptr\n";
            }
        }
        std::cout << "---------------------------------------------------------\n";
    }
};