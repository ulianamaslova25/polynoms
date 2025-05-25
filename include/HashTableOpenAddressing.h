#pragma once
#include <vector>
#include <functional> 
#include <stdexcept>
#include <optional>   

template<class TKey, class TValue, class THash = std::hash<TKey>>
class HashTableOpenAddressing {
private:
    enum class SlotState {
        EMPTY,
        OCCUPIED,
        DELETED
    };

    struct Slot {
        std::pair<TKey, TValue> data;
        SlotState state;

        Slot() : state(SlotState::EMPTY) {}
    };

    std::vector<Slot> table;
    size_t current_size;
    size_t capacity;
    THash hasher;

    const double MAX_LOAD_FACTOR = 0.7;

    size_t getHash(const TKey& key, size_t attempt) const {
        return (hasher(key) + attempt) % capacity;
    }

    void rehash() {
        size_t old_capacity = capacity;
        std::vector<Slot> old_table = std::move(table);

        capacity = (capacity == 0) ? 16 : capacity * 2;

        table.assign(capacity, Slot());
        current_size = 0;

        for (size_t i = 0; i < old_capacity; ++i) {
            if (old_table[i].state == SlotState::OCCUPIED) {
                insert(old_table[i].data.first, old_table[i].data.second);
            }
        }
    }

public:
    class iterator {
    private:
        typename std::vector<Slot>::iterator current_slot_iterator;
        typename std::vector<Slot>::iterator table_end_iterator; 
        const HashTableOpenAddressing* owner_table; 

        void advanceToNextOccupied() {
            while (current_slot_iterator != table_end_iterator &&
                current_slot_iterator->state != SlotState::OCCUPIED) {
                ++current_slot_iterator;
            }
        }

    public:
        iterator(typename std::vector<Slot>::iterator start_it,
            typename std::vector<Slot>::iterator end_it)
            : current_slot_iterator(start_it), table_end_iterator(end_it) {
            advanceToNextOccupied(); 
        }

        iterator(typename std::vector<Slot>::iterator end_it_marker)
            : current_slot_iterator(end_it_marker), table_end_iterator(end_it_marker) {
        }


        std::pair<const TKey, TValue>& operator*() const {
            if (current_slot_iterator == table_end_iterator || current_slot_iterator->state != SlotState::OCCUPIED) {
                throw std::out_of_range("Dereferencing invalid or end iterator for HashTable");
            }
            return reinterpret_cast<std::pair<const TKey, TValue>&>(current_slot_iterator->data);
        }

        std::pair<const TKey, TValue>* operator->() const {
            if (current_slot_iterator == table_end_iterator || current_slot_iterator->state != SlotState::OCCUPIED) {
                throw std::out_of_range("Accessing through invalid or end iterator for HashTable");
            }
            return reinterpret_cast<std::pair<const TKey, TValue>*>(&(current_slot_iterator->data));
        }

        iterator& operator++() { 
            if (current_slot_iterator != table_end_iterator) {
                ++current_slot_iterator;
                advanceToNextOccupied();
            }
            return *this;
        }

        iterator operator++(int) { 
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const iterator& other) const {
            return current_slot_iterator == other.current_slot_iterator;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }
    };

     HashTableOpenAddressing(size_t initial_capacity = 16)
        : current_size(0), capacity(initial_capacity) {
        if (capacity == 0) capacity = 16;
        table.resize(capacity);
    }

    HashTableOpenAddressing(const HashTableOpenAddressing& other)
        : current_size(other.current_size), capacity(other.capacity), hasher(other.hasher) {
        table = other.table; 
    }

    HashTableOpenAddressing& operator=(const HashTableOpenAddressing& other) {
        if (this == &other) {
            return *this;
        }
        current_size = other.current_size;
        capacity = other.capacity;
        hasher = other.hasher;
        table = other.table; 
        return *this;
    }

    iterator begin() {
        return iterator(table.begin(), table.end());
    }

    iterator end() {
        return iterator(table.end()); 
    }

    bool insert(const TKey& key, const TValue& value) {
        if (capacity == 0 || (double)(current_size + 1) / capacity >= MAX_LOAD_FACTOR) { 
            rehash();
        }

        size_t attempt = 0;
        size_t first_deleted_index = static_cast<size_t>(-1);

        while (attempt < capacity) {
            size_t index = getHash(key, attempt);

            if (table[index].state == SlotState::OCCUPIED) {
                if (table[index].data.first == key) {
                    table[index].data.second = value;
                    return false;
                }
            }
            else if (table[index].state == SlotState::EMPTY) {
                size_t insert_idx = (first_deleted_index != static_cast<size_t>(-1)) ? first_deleted_index : index;

                table[insert_idx].data = std::make_pair(key, value);
                table[insert_idx].state = SlotState::OCCUPIED;
                current_size++;
                return true;
            }
            else { 
                if (first_deleted_index == static_cast<size_t>(-1)) {
                    first_deleted_index = index;
                }
            }
            attempt++;
        }

        if (first_deleted_index != static_cast<size_t>(-1)) {
            table[first_deleted_index].data = std::make_pair(key, value);
            table[first_deleted_index].state = SlotState::OCCUPIED;
            current_size++;
            return true;
        }
        rehash(); 
        return insert(key, value); 
    }

    TValue* find(const TKey& key) {
        size_t attempt = 0;
        while (attempt < capacity) {
            size_t index = getHash(key, attempt);
            if (table[index].state == SlotState::EMPTY) {
                return nullptr;
            }
            if (table[index].state == SlotState::OCCUPIED && table[index].data.first == key) {
                return &table[index].data.second;
            }
            attempt++;
        }
        return nullptr;
    }

    const TValue* find(const TKey& key) const {
        size_t attempt = 0;
        while (attempt < capacity) {
            size_t index = getHash(key, attempt);
            if (table[index].state == SlotState::EMPTY) {
                return nullptr;
            }
            if (table[index].state == SlotState::OCCUPIED && table[index].data.first == key) {
                return &table[index].data.second;
            }
            attempt++;
        }
        return nullptr;
    }

    bool erase(const TKey& key) {
        size_t attempt = 0;
        while (attempt < capacity) {
            size_t index = getHash(key, attempt);
            if (table[index].state == SlotState::EMPTY) {
                return false;
            }
            if (table[index].state == SlotState::OCCUPIED && table[index].data.first == key) {
                table[index].state = SlotState::DELETED;
                current_size--;
                return true;
            }
            attempt++;
        }
        return false;
    }

    size_t size() const {
        return current_size;
    }

    bool empty() const {
        return current_size == 0;
    }

    TValue& operator[](const TKey& key) {
        TValue* found_value = find(key);
        if (found_value) {
            return *found_value;
        }
        else {
            insert(key, TValue{});
            found_value = find(key);
            if (!found_value) {
                throw std::logic_error("HashTable: Failed to find element after insertion in operator[]");
            }
            return *found_value;
        }
    }

    void clear() {
        size_t initial_cap = 16; 
        if (capacity > 0) initial_cap = capacity;

        table.clear(); 
        capacity = (this->capacity == 0) ? 16 : this->capacity; 
        table.assign(this->capacity, Slot());
        current_size = 0;
    }
};