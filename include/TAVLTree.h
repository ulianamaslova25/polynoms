#pragma once

#include <iostream>
#include <algorithm> 
#include <stdexcept> 
#include <utility>  

template<typename TKey, typename TValue>
class TAVLTree {
private:
    struct Node {
        TKey key;
        TValue value;
        Node* left;
        Node* right;
        Node* parent; 
        int height; 

        Node(TKey k, TValue v, Node* p = nullptr) 
            : key(std::move(k)), value(std::move(v)), left(nullptr), right(nullptr), parent(p), height(1) {
        }

        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;

        Node(Node&&) = default;
        Node& operator=(Node&&) = default;
    };

    Node* root;
    size_t tree_size; 

    int height(Node* node) const {
        if (!node) {
            return 0;
        }
        return node->height;
    }

    void updateHeight(Node* node) {
        if (node) {
            node->height = 1 + std::max(height(node->left), height(node->right));
        }
    }

    int balanceFactor(Node* node) const {
        if (!node) return 0;
        return height(node->left) - height(node->right);
    }

    Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;

        x->right = y;
        y->left = T2;

        x->parent = y->parent;
        y->parent = x;
        if (T2) T2->parent = y;

        updateHeight(y);
        updateHeight(x);

        return x; 
    }

    Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;

        y->left = x;
        x->right = T2;

        y->parent = x->parent;
        x->parent = y;
        if (T2) T2->parent = x;

        updateHeight(x);
        updateHeight(y);

        return y; 
    }

    Node* balance(Node* node) {
        if (!node) return nullptr;

        updateHeight(node);

        int bf = balanceFactor(node);

        if (bf > 1) {
            if (balanceFactor(node->left) < 0) {
                node->left = rotateLeft(node->left);
                if (node->left) { 
                    node->left->parent = node;
                } 
            }
            return rotateRight(node);
        }

        if (bf < -1) {
            if (balanceFactor(node->right) > 0) {
                node->right = rotateRight(node->right);
                if (node->right) { 
                    node->right->parent = node; 
                }
            }
            return rotateLeft(node);
        }
        return node;
    }

    Node* insert(Node* node, Node* parent, const TKey& key, const TValue& value) {
        if (!node) {
            tree_size++;
            return new Node(key, value, parent);
        }

        if (key < node->key) {
            node->left = insert(node->left, node, key, value);
        }
        else if (key > node->key) {
            node->right = insert(node->right, node, key, value);
        }
        else {
            node->value = value;
            return node;
        }

        return balance(node);
    }

    Node* findMin(Node* node) const {
        while (node && node->left) {
            node = node->left;
        }
        return node;
    }

    Node* findMax(Node* node) const {
        while (node && node->right) {
            node = node->right;
        }
        return node;
    }


    Node* erase(Node* node, const TKey& key) {
        if (!node) return nullptr;
        if (key < node->key) {
            node->left = erase(node->left, key);
            if (node->left) node->left->parent = node;
        }
        else if (key > node->key) {
            node->right = erase(node->right, key);
            if (node->right) node->right->parent = node;
        }
        else {
            if (!node->left || !node->right) {
                Node* temp;

                if (node->left) { 
                    temp = node->left;
                }
                else { 
                    temp = node->right;
                }
                Node* parent_of_deleted = node->parent;

                delete node;
                tree_size--;

                if (temp) { 
                    temp->parent = parent_of_deleted;
                }
                return temp;
            }
            else {
                Node* temp = findMin(node->right);
                node->key = std::move(temp->key);
                node->value = std::move(temp->value);
                node->right = erase(node->right, temp->key);
            }
        }
        return balance(node); 
    }

    void destroy(Node* node) {
        if (node) {
            destroy(node->left);
            destroy(node->right);
            delete node;
        }
    }

    class iterator;
    class const_iterator;

public:
    TAVLTree() : root(nullptr), tree_size(0) {}

    ~TAVLTree() {
        destroy(root);
        root = nullptr;
        tree_size = 0;
    }

    TAVLTree(const TAVLTree& other) = delete;
    TAVLTree& operator=(const TAVLTree& other) = delete;

    TAVLTree(TAVLTree&& other) noexcept = default;
    TAVLTree& operator=(TAVLTree&& other) noexcept = default;

    void insert(const TKey& key, const TValue& value) {
        root = insert(root, nullptr, key, value);
        if (root) root->parent = nullptr;
    }
    void erase(const TKey& key) {
        root = erase(root, key);
        if (root) root->parent = nullptr;
    }

    iterator find(const TKey& key) {
        Node* current = root;
        while (current) {
            if (key < current->key) {
                current = current->left;
            }
            else if (key > current->key) {
                current = current->right;
            }
            else {
                return iterator(current); 
            }
        }
        return end(); 
    }

    iterator begin() {
        return iterator(findMin(root));
    }

    iterator end() {
        return iterator(nullptr); 
    }

    bool empty() const { return root == nullptr; }

    size_t size() const { return tree_size; }

    class iterator {
        Node* current;
        friend class TAVLTree<TKey, TValue>;

    public:
        using value_type = std::pair<const TKey, TValue>; 
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        using iterator_category = std::bidirectional_iterator_tag;

        iterator(Node* node = nullptr) : current(node) {}

        value_type operator*() const {
            if (!current) throw std::runtime_error("Разыменование нулевого итератора");
            return { current->key, current->value }; 
        }

        value_type operator->() const {
            if (!current) throw std::runtime_error("Доступ через нулевой итератора");
            return { current->key, current->value }; 
        }

        iterator& operator++() { 
            if (!current) throw std::runtime_error("Инкремент end() итератора."); 

            if (current->right) {
                current = current->right;
                while (current->left) current = current->left;
            }
            else {
                Node* p = current->parent;
                while (p && current == p->right) {
                    current = p;
                    p = p->parent;
                }
                current = p; 
            }
            return *this;
        }

        iterator operator++(int) { 
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        iterator& operator--() { 
            if (!current) {
                throw std::runtime_error("Декремент от end() итератора не реализован без доступа к дереву.");
            }

            if (current->left) {
                current = current->left;
                while (current->right) current = current->right;
            }
            else {
                Node* p = current->parent;
                while (p && current == p->left) {
                    current = p;
                    p = p->parent;
                }
                current = p;
            }
            return *this;
        }

        iterator operator--(int) { 
            iterator tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const { return current == other.current; }
        bool operator!=(const iterator& other) const { return current != other.current; }

    };

};