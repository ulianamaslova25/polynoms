#include "gtest.h"
#include <string>
#include <vector>
#include <algorithm> 
#include <map>       
#include "../include/TRB-Tree.h"

// --- Тесты для RBTree<int, int> и RBTree<std::string, int> ---

TEST(RBTreeGenericTest, ConstructionAndEmptyState_IntInt) {
    RBTree<int, int> tree;
    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.begin(), tree.end());
}

TEST(RBTreeGenericTest, SingleInsertAndFind_IntInt) {
    RBTree<int, int> tree;
    auto result = tree.insert(10, 100);

    EXPECT_FALSE(tree.empty());
    ASSERT_NE(result.first, tree.end());
    EXPECT_TRUE(result.second);
    EXPECT_EQ(result.first->first, 10);
    EXPECT_EQ(result.first->second, 100);

    RBTree<int, int>::iterator find_it = tree.find(10);
    ASSERT_NE(find_it, tree.end());
    EXPECT_EQ(find_it->first, 10);
    EXPECT_EQ(find_it->second, 100);
    EXPECT_EQ(result.first, find_it);
}

TEST(RBTreeGenericTest, FindNonExisting_IntInt) {
    RBTree<int, int> tree;
    tree.insert(10, 100);
    EXPECT_EQ(tree.find(20), tree.end());
}

TEST(RBTreeGenericTest, InsertExistingUpdatesValue_IntInt) {
    RBTree<int, int> tree;
    tree.insert(10, 100);
    auto result = tree.insert(10, 200);

    EXPECT_FALSE(result.second);
    ASSERT_NE(result.first, tree.end());
    EXPECT_EQ(result.first->first, 10);
    EXPECT_EQ(result.first->second, 200);

    RBTree<int, int>::iterator find_it = tree.find(10);
    ASSERT_NE(find_it, tree.end());
    EXPECT_EQ(find_it->second, 200);
}

TEST(RBTreeGenericTest, IndexOperator_IntInt) {
    RBTree<int, int> tree;
    tree[10] = 100;
    EXPECT_EQ(tree[10], 100);

    tree[10] = 200;
    EXPECT_EQ(tree[10], 200);

    RBTree<int, int>::iterator find_it = tree.find(10);
    ASSERT_NE(find_it, tree.end());
    EXPECT_EQ(find_it->second, 200);

    tree[20];
    RBTree<int, int>::iterator find_20 = tree.find(20);
    ASSERT_NE(find_20, tree.end());
    EXPECT_EQ(find_20->first, 20);
    EXPECT_EQ(find_20->second, 0);
}

TEST(RBTreeGenericTest, IterationOrder_IntInt) {
    RBTree<int, int> tree;
    std::vector<int> keys = { 50, 25, 75, 10, 30, 60, 80, 5, 15, 28, 35, 55, 65, 78, 85 };
    for (int key : keys) {
        tree.insert(key, key * 10);
    }

    std::vector<int> iterated_keys;
    for (RBTree<int, int>::iterator it = tree.begin(); it != tree.end(); ++it) {
        iterated_keys.push_back(it->first);
    }

    std::sort(keys.begin(), keys.end());
    EXPECT_EQ(iterated_keys, keys);
}

TEST(RBTreeGenericTest, EraseLeafNode_IntInt) {
    RBTree<int, int> tree;
    tree.insert(10, 100);
    tree.insert(5, 50);

    EXPECT_TRUE(tree.erase(5));
    EXPECT_EQ(tree.find(5), tree.end());
    EXPECT_NE(tree.find(10), tree.end());
    EXPECT_FALSE(tree.empty());
}

TEST(RBTreeGenericTest, EraseNodeWithTwoChildren_IntInt) {
    RBTree<int, int> tree;
    tree.insert(50, 500);
    tree.insert(25, 250);
    tree.insert(75, 750);
    tree.insert(10, 100);
    tree.insert(30, 300);

    EXPECT_TRUE(tree.erase(25));
    EXPECT_EQ(tree.find(25), tree.end());
    EXPECT_NE(tree.find(50), tree.end());
    EXPECT_NE(tree.find(10), tree.end());
    EXPECT_NE(tree.find(30), tree.end());
    EXPECT_NE(tree.find(75), tree.end());

    std::vector<int> expected_keys = { 10, 30, 50, 75 };
    std::vector<int> iterated_keys;
    for (auto it = tree.begin(); it != tree.end(); ++it) iterated_keys.push_back(it->first);
    EXPECT_EQ(iterated_keys, expected_keys);
}

TEST(RBTreeGenericTest, EraseRoot_IntInt) {
    RBTree<int, int> tree;
    tree.insert(10, 100);
    EXPECT_TRUE(tree.erase(10));
    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.find(10), tree.end());

    tree.insert(50, 500);
    tree.insert(25, 250);
    tree.insert(75, 750);
    EXPECT_TRUE(tree.erase(50));
    EXPECT_EQ(tree.find(50), tree.end());
    EXPECT_NE(tree.find(25), tree.end());
    EXPECT_NE(tree.find(75), tree.end());
    std::vector<int> expected_keys = { 25, 75 };
    std::vector<int> iterated_keys;
    for (auto it = tree.begin(); it != tree.end(); ++it) iterated_keys.push_back(it->first);
    EXPECT_EQ(iterated_keys, expected_keys);
}

TEST(RBTreeGenericTest, EraseNonExistingKey_IntInt) {
    RBTree<int, int> tree;
    tree.insert(10, 100);
    EXPECT_FALSE(tree.erase(20));
    EXPECT_NE(tree.find(10), tree.end());
}

TEST(RBTreeGenericTest, ComplexSequenceOfInsertAndErase_IntInt) {
    RBTree<int, int> tree;
    std::map<int, int> reference_map;

    for (int i = 0; i < 20; ++i) {
        int key = (i * 7) % 23;
        int value = i * 100;
        tree.insert(key, value);
        reference_map[key] = value;
    }

    std::vector<int> tree_keys1, map_keys1;
    for (auto it = tree.begin(); it != tree.end(); ++it) tree_keys1.push_back(it->first);
    for (const auto& pair : reference_map) map_keys1.push_back(pair.first);
    EXPECT_EQ(tree_keys1, map_keys1);

    for (int i = 0; i < 10; ++i) {
        int key_to_erase = (i * 5) % 23;
        if (reference_map.count(key_to_erase)) {
            EXPECT_TRUE(tree.erase(key_to_erase));
            reference_map.erase(key_to_erase);
        }
        else {
            EXPECT_FALSE(tree.erase(key_to_erase));
        }
    }

    std::vector<int> tree_keys2, map_keys2;
    for (auto it = tree.begin(); it != tree.end(); ++it) tree_keys2.push_back(it->first);
    for (const auto& pair : reference_map) map_keys2.push_back(pair.first);
    EXPECT_EQ(tree_keys2, map_keys2);

    for (int i = 20; i < 30; ++i) {
        int key = (i * 3) % 29;
        int value = i * 1000;
        tree.insert(key, value);
        reference_map[key] = value;
    }

    std::vector<int> tree_keys3, map_keys3;
    for (auto it = tree.begin(); it != tree.end(); ++it) tree_keys3.push_back(it->first);
    for (const auto& pair : reference_map) map_keys3.push_back(pair.first);
    EXPECT_EQ(tree_keys3, map_keys3);
}

TEST(RBTreeGenericTest, CopyConstructorAndAssignment_IntInt) {
    RBTree<int, int> original_tree;
    original_tree.insert(10, 100);
    original_tree.insert(20, 200);

    RBTree<int, int> copied_tree = original_tree;
    ASSERT_NE(copied_tree.find(10), copied_tree.end());
    EXPECT_EQ((*copied_tree.find(10)).second, 100);
    ASSERT_NE(copied_tree.find(20), copied_tree.end());
    EXPECT_EQ((*copied_tree.find(20)).second, 200);
    EXPECT_FALSE(copied_tree.empty());

    copied_tree.insert(30, 300);
    EXPECT_EQ(original_tree.find(30), original_tree.end());
    EXPECT_NE(copied_tree.find(30), copied_tree.end());

    original_tree.erase(10);
    EXPECT_NE(copied_tree.find(10), copied_tree.end());
    EXPECT_EQ(original_tree.find(10), original_tree.end());

    RBTree<int, int> assigned_tree;
    assigned_tree.insert(5, 50);
    assigned_tree = original_tree;

    EXPECT_EQ(assigned_tree.find(5), assigned_tree.end());
    ASSERT_NE(assigned_tree.find(20), assigned_tree.end());
    EXPECT_EQ((*assigned_tree.find(20)).second, 200);

    assigned_tree.insert(40, 400);
    EXPECT_EQ(original_tree.find(40), original_tree.end());
    EXPECT_NE(assigned_tree.find(40), assigned_tree.end());
}

// --- Дополнительные тесты с другими типами, если нужно ---
TEST(RBTreeGenericTest, StringKeyTest) {
    RBTree<std::string, int> tree;
    tree.insert("apple", 1);
    tree.insert("banana", 2);
    tree.insert("cherry", 3);

    EXPECT_EQ((*tree.find("banana")).second, 2);
    tree["apple"] = 10;
    EXPECT_EQ((*tree.find("apple")).second, 10);

    std::vector<std::string> keys;
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        keys.push_back(it->first);
    }
    // Ожидаемый порядок для строк: "apple", "banana", "cherry"
    std::vector<std::string> expected_keys = { "apple", "banana", "cherry" };
    EXPECT_EQ(keys, expected_keys);

    EXPECT_TRUE(tree.erase("banana"));
    EXPECT_EQ(tree.find("banana"), tree.end());
}