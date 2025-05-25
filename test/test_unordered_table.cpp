#include "gtest.h"
#include <string>
#include "../include/TUnorderedTable.h"
#include <vector>


// --- Тесты для TUnorderedTable ---

// Тест на пустую таблицу
TEST(UnorderedTableTest, EmptyTable) {
    TUnorderedTable<int, std::string> table;
    EXPECT_EQ(table.size(), 0);
    EXPECT_TRUE(table.empty());
    EXPECT_EQ(table.find(10), table.end());
    EXPECT_EQ(table.erase(10), table.end());
    EXPECT_THROW(table[10], std::out_of_range);
}

// Тест на вставку элементов и их наличие (порядок не гарантируется)
TEST(UnorderedTableTest, InsertElementsAndCheckPresence) {
    TUnorderedTable<int, std::string> table;

    table.insert(2, "two");
    table.insert(1, "one");
    table.insert(3, "three");
    table.insert(0, "zero");
    table.insert(5, "five");

    EXPECT_EQ(table.size(), 5);
    EXPECT_FALSE(table.empty());

    auto it0 = table.find(0);
    ASSERT_NE(it0, table.end()); EXPECT_EQ(it0.value(), "zero");

    auto it1 = table.find(1);
    ASSERT_NE(it1, table.end()); EXPECT_EQ(it1.value(), "one");

    auto it2 = table.find(2);
    ASSERT_NE(it2, table.end()); EXPECT_EQ(it2.value(), "two");

    auto it3 = table.find(3);
    ASSERT_NE(it3, table.end()); EXPECT_EQ(it3.value(), "three");

    auto it5 = table.find(5);
    ASSERT_NE(it5, table.end()); EXPECT_EQ(it5.value(), "five");

    std::vector<std::pair<int, std::string>> collected_elements;
    for (auto it = table.begin(); it != table.end(); ++it) {
        collected_elements.push_back({ it.key(), it.value() });
    }
    EXPECT_EQ(collected_elements.size(), 5);
    std::sort(collected_elements.begin(), collected_elements.end());
    std::vector<std::pair<int, std::string>> expected_sorted = {
        {0, "zero"}, {1, "one"}, {2, "two"}, {3, "three"}, {5, "five"}
    };
    EXPECT_EQ(collected_elements, expected_sorted);
}

// Тест на обновление значения при вставке дублирующегося ключа
TEST(UnorderedTableTest, InsertDuplicateKeyUpdatesValue) {
    TUnorderedTable<int, std::string> table;

    table.insert(1, "one");
    table.insert(2, "two");
    table.insert(1, "odin"); 

    EXPECT_EQ(table.size(), 2); 

    auto it = table.find(1);
    ASSERT_NE(it, table.end());
    EXPECT_EQ(it.value(), "odin");
    EXPECT_EQ(table[1], "odin");

    auto it2 = table.find(2);
    ASSERT_NE(it2, table.end());
    EXPECT_EQ(it2.value(), "two");
    EXPECT_EQ(table[2], "two");
}

// Тест на поиск элементов
TEST(UnorderedTableTest, FindElements) {
    TUnorderedTable<int, std::string> table;
    table.insert(1, "one");
    table.insert(3, "three");
    table.insert(2, "two");

    auto it1 = table.find(1);
    ASSERT_NE(it1, table.end());
    EXPECT_EQ(it1.key(), 1);
    EXPECT_EQ(it1.value(), "one");

    auto it2 = table.find(2);
    ASSERT_NE(it2, table.end());
    EXPECT_EQ(it2.key(), 2);
    EXPECT_EQ(it2.value(), "two");

    auto it3 = table.find(3);
    ASSERT_NE(it3, table.end());
    EXPECT_EQ(it3.key(), 3);
    EXPECT_EQ(it3.value(), "three");

    EXPECT_EQ(table.find(0), table.end());
    EXPECT_EQ(table.find(4), table.end());
}

// Тест на оператор индексирования
TEST(UnorderedTableTest, IndexingOperator) {
    TUnorderedTable<int, std::string> table;
    table.insert(1, "one");
    table.insert(2, "two");

    EXPECT_EQ(table[1], "one");
    EXPECT_EQ(table[2], "two");

    table[1] = "odin";
    EXPECT_EQ(table[1], "odin");

    EXPECT_THROW(table[4], std::out_of_range);
}

// Тест на удаление существующих элементов и возвращаемый итератор (swap-and-pop)
TEST(UnorderedTableTest, EraseExistingElementsUnordered) {
    TUnorderedTable<int, std::string> table;
    table.insert(0, "zero");  
    table.insert(1, "one");   
    table.insert(2, "two");   
    table.insert(3, "three");

    EXPECT_EQ(table.size(), 4);

    auto it_after_1 = table.erase(1);
    EXPECT_EQ(table.size(), 3);
    EXPECT_EQ(table.find(1), table.end()); 
    ASSERT_NE(it_after_1, table.end());
    EXPECT_EQ(it_after_1.key(), 3);    
    EXPECT_EQ(it_after_1.value(), "three"); 
    EXPECT_NE(table.find(0), table.end()); 
    EXPECT_NE(table.find(2), table.end()); 
    EXPECT_NE(table.find(3), table.end()); 


    auto it_after_2 = table.erase(2);
    EXPECT_EQ(table.size(), 2);
    EXPECT_EQ(table.find(2), table.end()); 
    EXPECT_EQ(it_after_2, table.end());
    EXPECT_NE(table.find(0), table.end());
    EXPECT_NE(table.find(3), table.end());

    auto it_after_0 = table.erase(0);
    EXPECT_EQ(table.size(), 1);
    EXPECT_EQ(table.find(0), table.end());
    ASSERT_NE(it_after_0, table.end());
    EXPECT_EQ(it_after_0.key(), 3);
    EXPECT_EQ(it_after_0.value(), "three");

    auto it_after_3 = table.erase(3);
    EXPECT_EQ(table.size(), 0);
    EXPECT_TRUE(table.empty());
    EXPECT_EQ(table.find(3), table.end());
    EXPECT_EQ(it_after_3, table.end()); 

    EXPECT_EQ(table.erase(100), table.end());
    EXPECT_EQ(table.size(), 0);
}


// Тест на удаление несуществующих элементов
TEST(UnorderedTableTest, EraseNonExistingElements) {
    TUnorderedTable<int, std::string> table;
    table.insert(1, "one");
    table.insert(3, "three");

    EXPECT_EQ(table.size(), 2);

    auto it_erase_0 = table.erase(0);
    EXPECT_EQ(it_erase_0, table.end());
    EXPECT_EQ(table.size(), 2);
    EXPECT_NE(table.find(1), table.end());

    auto it_erase_2 = table.erase(2);
    EXPECT_EQ(it_erase_2, table.end());
    EXPECT_EQ(table.size(), 2);
    EXPECT_NE(table.find(1), table.end());
    EXPECT_NE(table.find(3), table.end());

    auto it_erase_4 = table.erase(4);
    EXPECT_EQ(it_erase_4, table.end());
    EXPECT_EQ(table.size(), 2);
}

// Тест на разыменование и доступ через итератор
TEST(UnorderedTableTest, IteratorDereferenceAndAccess) {
    TUnorderedTable<int, std::string> table;
    table.insert(10, "alpha");
    table.insert(20, "beta");

    auto it = table.begin(); 
    ASSERT_NE(it, table.end());

    EXPECT_EQ((*it).first, 10);
    EXPECT_EQ((*it).second, "alpha");
    EXPECT_EQ(it->first, 10);
    EXPECT_EQ(it->second, "alpha");
    EXPECT_EQ(it.key(), 10);
    EXPECT_EQ(it.value(), "alpha");

    it.value() = "gamma";
    EXPECT_EQ(it.value(), "gamma");
    EXPECT_EQ(table[10], "gamma");

    ++it;
    ASSERT_NE(it, table.end());

    EXPECT_EQ(it.key(), 20);
    EXPECT_EQ(it.value(), "beta");

    ++it;
    EXPECT_EQ(it, table.end());
}

// Тест на сравнение итераторов
TEST(UnorderedTableTest, IteratorComparison) {
    TUnorderedTable<int, std::string> table;
    table.insert(1, "a");
    table.insert(2, "b");

    auto it1 = table.begin();
    auto it2 = table.begin();
    auto it3 = table.begin();
    ++it3;
    auto it_end = table.end();

    EXPECT_TRUE(it1 == it2);
    EXPECT_FALSE(it1 != it2);

    EXPECT_FALSE(it1 == it3);
    EXPECT_TRUE(it1 != it3);

    EXPECT_FALSE(it1 == it_end);
    EXPECT_TRUE(it1 != it_end);

    EXPECT_TRUE(it_end == table.end());
}

// Дополнительный тест на поведение итератора после erase (swap-and-pop)
TEST(UnorderedTableTest, IteratorBehaviorAfterSwapAndPopErase) {
    TUnorderedTable<int, std::string> table;
    table.insert(1, "A"); 
    table.insert(2, "B"); 
    table.insert(3, "C"); 
    table.insert(4, "D"); 

    auto it_b = table.find(2); 
    auto it_c = table.find(3); 
    auto it_d = table.find(4); 

    ASSERT_NE(it_b, table.end());
    ASSERT_NE(it_c, table.end());
    ASSERT_NE(it_d, table.end());

    auto returned_it = table.erase(2);

    ASSERT_NE(returned_it, table.end());
    EXPECT_EQ(returned_it.key(), 4); 
    EXPECT_EQ(returned_it.value(), "D");

    auto found_c_after_erase = table.find(3);
    ASSERT_NE(found_c_after_erase, table.end());
    EXPECT_EQ(found_c_after_erase.key(), 3); 
    EXPECT_EQ(found_c_after_erase.value(), "C");

    EXPECT_EQ(table.find(2), table.end());
    EXPECT_EQ(table.size(), 3);
}