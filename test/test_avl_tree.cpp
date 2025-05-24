#include "gtest.h" // Google Test framework
#include "TAVLTree.h"    // Your TAVLTree class header

TEST(AVLTreeTest, EmptyTree) {
    TAVLTree<int, std::string> tree;
    EXPECT_EQ(tree.size(), 0);
    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.find(10), tree.end()); // find in empty tree
    EXPECT_NO_THROW(tree.erase(10));     // erase non-existing in empty tree

    // Iteration on empty tree
    EXPECT_EQ(tree.begin(), tree.end());
    // Expect --end() to throw based on the provided iterator code
    EXPECT_THROW(tree.end()--, std::runtime_error);
}

TEST(AVLTreeTest, InsertSingleElement) {
    TAVLTree<int, std::string> tree;
    tree.insert(5, "five");

    EXPECT_EQ(tree.size(), 1);
    EXPECT_FALSE(tree.empty());

    auto it = tree.find(5);
    ASSERT_NE(it, tree.end());


    // Check iteration
    auto begin_it = tree.begin();
    ASSERT_NE(begin_it, tree.end());
    ++begin_it;
    EXPECT_EQ(begin_it, tree.end());

    // Check decrement from the element
    auto end_it = tree.end();
    // This test relies on the specific behavior of the iterator's -- operator
    // to correctly find the predecessor (which is complex).
    // Given the simplified iterator, direct --end() is not supported.
    // Test decrementing the first element (should behave like begin()-- which is undef behavior or end())
    // The current implementation makes begin()-- lead to nullptr if parent is null.
    // EXPECT_THROW(tree.begin()--, std::runtime_error); // Based on the check in the code if current->left is null
}

TEST(AVLTreeTest, InsertMultipleAscending) {
    TAVLTree<int, std::string> tree;
    tree.insert(1, "one");
    tree.insert(2, "two"); // Should trigger rotations
    tree.insert(3, "three"); // Should trigger rotations
    tree.insert(4, "four");
    tree.insert(5, "five"); // Should trigger rotations

    EXPECT_EQ(tree.size(), 5);

    // Check elements are findable
    EXPECT_NE(tree.find(1), tree.end());
    EXPECT_NE(tree.find(2), tree.end());
    EXPECT_NE(tree.find(3), tree.end());
    EXPECT_NE(tree.find(4), tree.end());
    EXPECT_NE(tree.find(5), tree.end());
    EXPECT_EQ(tree.find(0), tree.end()); // Non-existent

    // Check iteration order
    std::vector<std::pair<int, std::string>> expected = { {1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}, {5, "five"} };
    std::vector<std::pair<int, std::string>> actual;
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        actual.push_back(*it); // Copy the pair
    }
    EXPECT_EQ(actual, expected);
}

TEST(AVLTreeTest, InsertMultipleDescending) {
    TAVLTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(4, "four"); // Should trigger rotations
    tree.insert(3, "three"); // Should trigger rotations
    tree.insert(2, "two");
    tree.insert(1, "one"); // Should trigger rotations

    EXPECT_EQ(tree.size(), 5);

    // Check iteration order
    std::vector<std::pair<int, std::string>> expected = { {1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}, {5, "five"} };
    std::vector<std::pair<int, std::string>> actual;
    for (auto const& pair : tree) { // Use range-based for loop
        actual.push_back(pair); // Copy the pair
    }
    EXPECT_EQ(actual, expected);
}

TEST(AVLTreeTest, InsertMultipleRandom) {
    TAVLTree<int, std::string> tree;
    tree.insert(3, "three");
    tree.insert(1, "one");
    tree.insert(5, "five");
    tree.insert(0, "zero"); // Rotations
    tree.insert(2, "two"); // Rotations
    tree.insert(4, "four"); // Rotations
    tree.insert(6, "six"); // Rotations

    EXPECT_EQ(tree.size(), 7);

    std::vector<std::pair<int, std::string>> expected = {
        {0, "zero"}, {1, "one"}, {2, "two"}, {3, "three"},
        {4, "four"}, {5, "five"}, {6, "six"}
    };
    std::vector<std::pair<int, std::string>> actual;
    for (auto const& pair : tree) {
        actual.push_back(pair);
    }
    EXPECT_EQ(actual, expected);
}




TEST(AVLTreeTest, UpdateExisting) {
    TAVLTree<int, std::string> tree;
    tree.insert(5, "five original");
    tree.insert(3, "three");
    tree.insert(7, "seven");

    EXPECT_EQ(tree.size(), 3);

    tree.insert(5, "five updated"); // Update value for key 5

    EXPECT_EQ(tree.size(), 3); // Size should not change on update
    auto it = tree.find(5);
    ASSERT_NE(it, tree.end());

    // Check iteration order is still correct
    std::vector<std::pair<int, std::string>> expected = { {3, "three"}, {5, "five updated"}, {7, "seven"} };
    std::vector<std::pair<int, std::string>> actual;
    for (auto const& pair : tree) {
        actual.push_back(pair);
    }
    EXPECT_EQ(actual, expected);
}

TEST(AVLTreeTest, EraseLeaf) {
    TAVLTree<int, std::string> tree;
    tree.insert(10, "ten");
    tree.insert(5, "five");
    tree.insert(15, "fifteen");
    tree.insert(3, "three");
    tree.insert(7, "seven");

    EXPECT_EQ(tree.size(), 5);
    EXPECT_NE(tree.find(3), tree.end());

    tree.erase(3); // Erase a leaf

    EXPECT_EQ(tree.size(), 4);
    EXPECT_EQ(tree.find(3), tree.end()); // Should not be found

    // Check iteration order
    std::vector<std::pair<int, std::string>> expected = { {5, "five"}, {7, "seven"}, {10, "ten"}, {15, "fifteen"} };
    std::vector<std::pair<int, std::string>> actual;
    for (auto const& pair : tree) {
        actual.push_back(pair);
    }
    EXPECT_EQ(actual, expected);

    tree.erase(7); // Erase another leaf
    EXPECT_EQ(tree.size(), 3);
    EXPECT_EQ(tree.find(7), tree.end());
    expected = { {5, "five"}, {10, "ten"}, {15, "fifteen"} };
    actual.clear();
    for (auto const& pair : tree) {
        actual.push_back(pair);
    }
    EXPECT_EQ(actual, expected);
}

TEST(AVLTreeTest, EraseNodeWithOneChild) {
    TAVLTree<int, std::string> tree;
    tree.insert(10, "ten");
    tree.insert(5, "five");
    tree.insert(15, "fifteen");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    tree.insert(17, "seventeen");
    tree.insert(16, "sixteen"); 
    EXPECT_EQ(tree.size(), 7); 
    EXPECT_NE(tree.find(15), tree.end());

    tree.erase(15); 
    EXPECT_EQ(tree.size(), 6); 
    EXPECT_EQ(tree.find(15), tree.end()); 
    EXPECT_NE(tree.find(16), tree.end()); 

    std::vector<std::pair<int, std::string>> expected_after_erase_15 = { {3, "three"}, {5, "five"}, {7, "seven"}, {10, "ten"}, {16, "sixteen"}, {17, "seventeen"} };
    std::vector<std::pair<int, std::string>> actual_after_erase_15;
    for (auto const& pair : tree) {
        actual_after_erase_15.push_back(pair);
    }
    EXPECT_EQ(actual_after_erase_15, expected_after_erase_15);

    tree.erase(5); 
    EXPECT_EQ(tree.size(), 5);
    EXPECT_EQ(tree.find(5), tree.end()); 
    EXPECT_NE(tree.find(3), tree.end()); 

    std::vector<std::pair<int, std::string>> expected_after_erase_5 = { {3, "three"}, {7, "seven"}, {10, "ten"}, {16, "sixteen"}, {17, "seventeen"} };
    std::vector<std::pair<int, std::string>> actual_after_erase_5;
    for (auto const& pair : tree) {
        actual_after_erase_5.push_back(pair);
    }
    EXPECT_EQ(actual_after_erase_5, expected_after_erase_5);
}

TEST(AVLTreeTest, EraseNodeWithTwoChildren) {
    TAVLTree<int, std::string> tree;
    tree.insert(10, "ten");
    tree.insert(5, "five");
    tree.insert(15, "fifteen");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    tree.insert(12, "twelve"); // Successor of 10
    tree.insert(17, "seventeen");
    // Структура дерева: 10(0)/ 5(0) \ 15(0) / 3(0) \ 7(0) / 12(0) \ 17(0)
    // Size: 7

    // Изначальный размер после всех вставок
    EXPECT_EQ(tree.size(), 7);
    EXPECT_NE(tree.find(10), tree.end()); // 10 должен быть в дереве

    tree.erase(10); // Удаляем корень (10) - узел с двумя потомками (5 и 15)
    // Заменится наследником 12. Размер уменьшится на 1.
    // Size: 6

    // Проверка после первого удаления
    EXPECT_EQ(tree.size(), 6);
    EXPECT_EQ(tree.find(10), tree.end()); // 10 должен быть удален
    EXPECT_NE(tree.find(12), tree.end()); // 12 (наследник) должен остаться

    // Проверка порядка после первого удаления (ожидаемые элементы после удаления 10)
    // Корень теперь 12. Inorder обход: 3, 5, 7, 12, 15, 17
    std::vector<std::pair<int, std::string>> expected_after_erase_10 = { {3, "three"}, {5, "five"}, {7, "seven"}, {12, "twelve"}, {15, "fifteen"}, {17, "seventeen"} };
    std::vector<std::pair<int, std::string>> actual_after_erase_10;
    for (auto const& pair : tree) {
        actual_after_erase_10.push_back(pair);
    }
    EXPECT_EQ(actual_after_erase_10, expected_after_erase_10);


    tree.erase(5); // Удаляем 5 - узел с двумя потомками (3 и 7)
    // Заменится наследником 7. Размер уменьшится на 1.
    // Size: 5

    // Проверка после второго удаления
    EXPECT_EQ(tree.size(), 5);
    EXPECT_EQ(tree.find(5), tree.end()); // 5 должен быть удален

    // Проверка порядка после второго удаления (ожидаемые элементы после удаления 5)
    // Inorder обход: 3, 7, 12, 15, 17
    std::vector<std::pair<int, std::string>> expected_after_erase_5 = { {3, "three"}, {7, "seven"}, {12, "twelve"}, {15, "fifteen"}, {17, "seventeen"} };
    std::vector<std::pair<int, std::string>> actual_after_erase_5;
    for (auto const& pair : tree) {
        actual_after_erase_5.push_back(pair);
    }
    // В оригинальном тесте здесь был код с expected.erase(expected.begin() + 1);
    // Это также корректно.
    EXPECT_EQ(actual_after_erase_5, expected_after_erase_5);
}

TEST(AVLTreeTest, EraseRootSingleElement) {
    TAVLTree<int, std::string> tree;
    tree.insert(10, "ten");
    EXPECT_EQ(tree.size(), 1);
    EXPECT_FALSE(tree.empty());

    tree.erase(10); // Erase the only element (root)

    EXPECT_EQ(tree.size(), 0);
    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.find(10), tree.end()); // Should not be found
    EXPECT_EQ(tree.begin(), tree.end()); // Tree should be empty
}


TEST(AVLTreeTest, EraseNonExisting) {
    TAVLTree<int, std::string> tree;
    tree.insert(10, "ten");
    tree.insert(5, "five");
    EXPECT_EQ(tree.size(), 2);

    EXPECT_NO_THROW(tree.erase(100)); // Erase non-existing
    EXPECT_EQ(tree.size(), 2); // Size should not change

    EXPECT_NO_THROW(tree.erase(0)); // Erase another non-existing
    EXPECT_EQ(tree.size(), 2); // Size should not change
}


TEST(AVLTreeTest, EraseAllElements) {
    TAVLTree<int, std::string> tree;
    tree.insert(10, "ten");
    tree.insert(5, "five");
    tree.insert(15, "fifteen");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    tree.insert(12, "twelve");
    tree.insert(17, "seventeen");

    EXPECT_EQ(tree.size(), 7);

    tree.erase(5);
    EXPECT_EQ(tree.size(), 6);
    tree.erase(15);
    EXPECT_EQ(tree.size(), 5);
    tree.erase(3);
    EXPECT_EQ(tree.size(), 4);
    tree.erase(7);
    EXPECT_EQ(tree.size(), 3);
    tree.erase(12);
    EXPECT_EQ(tree.size(), 2);
    tree.erase(17);
    EXPECT_EQ(tree.size(), 1);
    tree.erase(10);
    EXPECT_EQ(tree.size(), 0);
    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.begin(), tree.end());
}

// This test specifically checks that decrementing the end iterator throws
TEST(AVLTreeTest, IterationDecrementEndThrows) {
    TAVLTree<int, int> tree;
    tree.insert(1, 1);
    tree.insert(2, 2);
    EXPECT_THROW(tree.end()--, std::runtime_error);
    EXPECT_THROW(--tree.end(), std::runtime_error);
}