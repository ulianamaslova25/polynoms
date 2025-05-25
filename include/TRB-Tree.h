#pragma once
#include <string>
#include <iostream>
#include <stdexcept> 
#include <utility>   
#include <algorithm> 

template<class TKey, class TValue>
class RBTree {
public:
    enum class Color {
        RED,
        BLACK
    };

    struct Node {
        Node* left;
        Node* right;
        Node* parent;
        std::pair<TKey, TValue> data;
        Color color;

        Node(const TKey& key, const TValue& value,
            Color c = Color::RED, Node* p = nullptr,
            Node* l = nullptr, Node* r = nullptr)
            : left(l), right(r), parent(p), data(std::make_pair(key, value)), color(c) {
        }
    };

private:
    Node* root;
    Node* nil_node;

    void rotateLeft(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left != nil_node) {
            y->left->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == nullptr) {
            this->root = y;
        }
        else if (x == x->parent->left) {
            x->parent->left = y;
        }
        else {
            x->parent->right = y;
        }
        y->left = x;
        x->parent = y;
    }

    void rotateRight(Node* x) {
        Node* y = x->left;
        x->left = y->right;
        if (y->right != nil_node) {
            y->right->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == nullptr) {
            this->root = y;
        }
        else if (x == x->parent->right) {
            x->parent->right = y;
        }
        else {
            x->parent->left = y;
        }
        y->right = x;
        x->parent = y;
    }

    void insertFixup(Node* z) {
        Node* y;
        while (z->parent != nullptr && z->parent->color == Color::RED) {
            if (z->parent == z->parent->parent->left) {
                y = z->parent->parent->right;
                if (y->color == Color::RED) {
                    z->parent->color = Color::BLACK;
                    y->color = Color::BLACK;
                    z->parent->parent->color = Color::RED;
                    z = z->parent->parent;
                }
                else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        rotateLeft(z);
                    }
                    z->parent->color = Color::BLACK;
                    if (z->parent->parent) {
                        z->parent->parent->color = Color::RED;
                        rotateRight(z->parent->parent);
                    }
                }
            }
            else {
                y = z->parent->parent->left;
                if (y->color == Color::RED) {
                    z->parent->color = Color::BLACK;
                    y->color = Color::BLACK;
                    z->parent->parent->color = Color::RED;
                    z = z->parent->parent;
                }
                else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rotateRight(z);
                    }
                    z->parent->color = Color::BLACK;
                    if (z->parent->parent) {
                        z->parent->parent->color = Color::RED;
                        rotateLeft(z->parent->parent);
                    }
                }
            }
            if (z == root) break;
        }
        if (root != nil_node) {
            root->color = Color::BLACK;
        }
    }

    Node* findNodeRecursive(Node* node, const TKey& key) const {
        if (node == nil_node || key == node->data.first) {
            return node;
        }
        if (key < node->data.first) {
            return findNodeRecursive(node->left, key);
        }
        else {
            return findNodeRecursive(node->right, key);
        }
    }

    void transplant(Node* u, Node* v) {
        if (u->parent == nullptr) {
            root = v;
        }
        else if (u == u->parent->left) {
            u->parent->left = v;
        }
        else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }

    Node* treeMinimum(Node* node) const {
        if (node == nil_node) return nil_node;
        while (node->left != nil_node) {
            node = node->left;
        }
        return node;
    }

    Node* treeSuccessor(Node* x) const {
        if (x == nil_node) return nil_node;
        if (x->right != nil_node) {
            return treeMinimum(x->right);
        }
        Node* y = x->parent;
        while (y != nullptr && x == y->right) {
            x = y;
            y = y->parent;
        }
        return (y == nullptr) ? nil_node : y;
    }

    void deleteFixup(Node* x) {
        Node* s;
        while (x != root && x->color == Color::BLACK) {
            if (x == x->parent->left) {
                s = x->parent->right;
                if (s->color == Color::RED) {
                    s->color = Color::BLACK;
                    x->parent->color = Color::RED;
                    rotateLeft(x->parent);
                    s = x->parent->right;
                }
                if (s->left->color == Color::BLACK && s->right->color == Color::BLACK) {
                    s->color = Color::RED;
                    x = x->parent;
                }
                else {
                    if (s->right->color == Color::BLACK) {
                        s->left->color = Color::BLACK;
                        s->color = Color::RED;
                        rotateRight(s);
                        s = x->parent->right;
                    }
                    s->color = x->parent->color;
                    x->parent->color = Color::BLACK;
                    s->right->color = Color::BLACK;
                    rotateLeft(x->parent);
                    x = root;
                }
            }
            else {
                s = x->parent->left;
                if (s->color == Color::RED) {
                    s->color = Color::BLACK;
                    x->parent->color = Color::RED;
                    rotateRight(x->parent);
                    s = x->parent->left;
                }
                if (s->right->color == Color::BLACK && s->left->color == Color::BLACK) {
                    s->color = Color::RED;
                    x = x->parent;
                }
                else {
                    if (s->left->color == Color::BLACK) {
                        s->right->color = Color::BLACK;
                        s->color = Color::RED;
                        rotateLeft(s);
                        s = x->parent->left;
                    }
                    s->color = x->parent->color;
                    x->parent->color = Color::BLACK;
                    s->left->color = Color::BLACK;
                    rotateRight(x->parent);
                    x = root;
                }
            }
        }
        if (x != nil_node) {
            x->color = Color::BLACK;
        }
    }

    void clearRecursive(Node* node) {
        if (node != nil_node) {
            clearRecursive(node->left);
            clearRecursive(node->right);
            delete node;
        }
    }

    Node* copyRecursive(Node* node_to_copy, Node* parent_of_new_node, Node* ext_nil_node) {
        if (node_to_copy == ext_nil_node) {
            return this->nil_node;
        }
        Node* new_node = new Node(node_to_copy->data.first, node_to_copy->data.second,
            node_to_copy->color, parent_of_new_node,
            this->nil_node, this->nil_node);
        new_node->left = copyRecursive(node_to_copy->left, new_node, ext_nil_node);
        new_node->right = copyRecursive(node_to_copy->right, new_node, ext_nil_node);
        return new_node;
    }

public:
    class iterator {
    private:
        Node* current_node;
        const RBTree* tree_owner;

    public:
        iterator(Node* node, const RBTree* owner) : current_node(node), tree_owner(owner) {}

        std::pair<const TKey, TValue>& operator*() const {
            if (current_node == tree_owner->nil_node || current_node == nullptr) {
                throw std::out_of_range("Dereferencing end or null iterator");
            }
            return reinterpret_cast<std::pair<const TKey, TValue>&>(current_node->data);
        }

        std::pair<const TKey, TValue>* operator->() const {
            if (current_node == tree_owner->nil_node || current_node == nullptr) {
                throw std::out_of_range("Accessing through end or null iterator");
            }
            return reinterpret_cast<std::pair<const TKey, TValue>*>(&(current_node->data));
        }

        iterator& operator++() {
            if (current_node != tree_owner->nil_node && current_node != nullptr) {
                current_node = tree_owner->treeSuccessor(current_node);
            }
            return *this;
        }

        iterator operator++(int) {
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const iterator& other) const {
            return current_node == other.current_node && tree_owner == other.tree_owner;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }
    };

    RBTree() {
        TKey defaultKey{};
        TValue defaultValue{};
        nil_node = new Node(defaultKey, defaultValue);
        nil_node->color = Color::BLACK;
        nil_node->left = nil_node;
        nil_node->right = nil_node;
        nil_node->parent = nullptr;
        root = nil_node;
    }

    RBTree(const RBTree& other) : nil_node(nullptr), root(nullptr) {
        TKey defaultKey{};
        TValue defaultValue{};
        nil_node = new Node(defaultKey, defaultValue);
        nil_node->color = Color::BLACK;
        nil_node->left = nil_node;
        nil_node->right = nil_node;
        nil_node->parent = nullptr;
        if (other.root == other.nil_node) {
            root = this->nil_node;
        }
        else {
            root = copyRecursive(other.root, nullptr, other.nil_node);
        }
    }

    RBTree& operator=(const RBTree& other) {
        if (this == &other) {
            return *this;
        }
        clearRecursive(this->root);
        if (other.root == other.nil_node) {
            this->root = this->nil_node;
        }
        else {
            this->root = copyRecursive(other.root, nullptr, other.nil_node);
        }
        return *this;
    }

    ~RBTree() {
        clearRecursive(root);
        delete nil_node;
    }

    iterator begin() const {
        return iterator(treeMinimum(root), this);
    }

    iterator end() const {
        return iterator(nil_node, this);
    }

    std::pair<iterator, bool> insert(const TKey& key, const TValue& value) {
        Node* z = new Node(key, value, Color::RED, nullptr, nil_node, nil_node);
        Node* y = nullptr;
        Node* x = this->root;
        bool inserted_new = true;

        while (x != nil_node) {
            y = x;
            if (z->data.first < x->data.first) {
                x = x->left;
            }
            else if (z->data.first > x->data.first) {
                x = x->right;
            }
            else {
                x->data.second = value;
                delete z;
                inserted_new = false;
                return { iterator(x, this), false };
            }
        }
        z->parent = y;
        if (y == nullptr) {
            this->root = z;
        }
        else if (z->data.first < y->data.first) {
            y->left = z;
        }
        else {
            y->right = z;
        }
        insertFixup(z);
        return { iterator(z, this), inserted_new };
    }

    iterator find(const TKey& key) const {
        Node* node = findNodeRecursive(root, key);
        return iterator(node, this);
    }

    bool erase(const TKey& key) {
        Node* z = findNodeRecursive(root, key);
        if (z == nil_node) {
            return false;
        }

        Node* y = z;
        Node* x;
        Color y_original_color = y->color;

        if (z->left == nil_node) {
            x = z->right;
            transplant(z, z->right);
        }
        else if (z->right == nil_node) {
            x = z->left;
            transplant(z, z->left);
        }
        else {
            y = treeMinimum(z->right);
            y_original_color = y->color;
            x = y->right;
            if (y->parent == z) {
                x->parent = y;
            }
            else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        delete z;

        if (y_original_color == Color::BLACK) {
            deleteFixup(x);
        }
        return true;
    }

    TValue& operator[](const TKey& key) {
        iterator it = find(key);
        if (it == end()) {
            return (*insert(key, TValue{}).first).second;
        }
        return (*it).second;
    }

    bool empty() const {
        return root == nil_node;
    }

    void print() const {
        printBT("", root, false);
    }

private:
    void printBT(const std::string& prefix, const Node* node, bool isLeft) const {
        if (node != nil_node) {
            std::cout << prefix;
            std::cout << (isLeft ? "|--" : "+--");

            std::cout << node->data.first << "(";
            if (node->color == Color::RED) std::cout << "R"; else std::cout << "B";
            std::cout << ")" << std::endl;

            printBT(prefix + (isLeft ? "|   " : "    "), node->left, true);
            printBT(prefix + (isLeft ? "|   " : "    "), node->right, false);
        }
    }
};