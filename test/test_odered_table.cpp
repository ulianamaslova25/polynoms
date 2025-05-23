#include "gtest.h" 
#include "TOrderedTable.h" 
#include <string>      
#include <vector>      
#include <stdexcept>


// Этот тест уже был, оставлю его как пример начала
TEST(OrderedTableTest, EmptyTable) {
    TOrderedTable<int, std::string> table;
    EXPECT_EQ(table.size(), 0);
    EXPECT_TRUE(table.empty());
    EXPECT_EQ(table.find(10), table.end());
    EXPECT_EQ(table.erase(10), table.end());
    // Дополнительно проверяем оператор [] на пустой таблице (должен бросить исключение)
    EXPECT_THROW(table[10], std::out_of_range);
}

TEST(OrderedTableTest, InsertElementsMaintainOrder) {
    TOrderedTable<int, std::string> table;

    // Вставляем в разном порядке, проверяем, что сохраняется сортировка по ключу
    table.insert(2, "two");
    table.insert(1, "one");
    table.insert(3, "three");
    table.insert(0, "zero");
    table.insert(5, "five");

    EXPECT_EQ(table.size(), 5);
    EXPECT_FALSE(table.empty());

    // Проверяем порядок и значения, используя итераторы
    auto it = table.begin();
    ASSERT_NE(it, table.end()); EXPECT_EQ(it.key(), 0); EXPECT_EQ(it.value(), "zero");
    ++it;
    ASSERT_NE(it, table.end()); EXPECT_EQ(it.key(), 1); EXPECT_EQ(it.value(), "one");
    ++it;
    ASSERT_NE(it, table.end()); EXPECT_EQ(it.key(), 2); EXPECT_EQ(it.value(), "two");
    ++it;
    ASSERT_NE(it, table.end()); EXPECT_EQ(it.key(), 3); EXPECT_EQ(it.value(), "three");
    ++it;
    ASSERT_NE(it, table.end()); EXPECT_EQ(it.key(), 5); EXPECT_EQ(it.value(), "five");
    ++it;
    EXPECT_EQ(it, table.end());
}

TEST(OrderedTableTest, InsertDuplicateKeyUpdatesValue) {
    TOrderedTable<int, std::string> table;

    table.insert(1, "one");
    table.insert(2, "two");
    table.insert(1, "odin"); // Вставляем дубликат ключа 1

    EXPECT_EQ(table.size(), 2); // Размер не должен измениться

    // Проверяем, что значение обновилось
    auto it = table.find(1);
    ASSERT_NE(it, table.end()); // Ключ должен быть найден
    EXPECT_EQ(it.value(), "odin"); // Значение должно быть обновленным
    EXPECT_EQ(table[1], "odin");   // Проверка через оператор []

    // Проверяем, что другие элементы не затронуты
    auto it2 = table.find(2);
    ASSERT_NE(it2, table.end());
    EXPECT_EQ(it2.value(), "two");
    EXPECT_EQ(table[2], "two");

    // Проверяем порядок после обновления
    auto begin_it = table.begin();
    ASSERT_NE(begin_it, table.end()); EXPECT_EQ(begin_it.key(), 1); EXPECT_EQ(begin_it.value(), "odin");
    ++begin_it;
    ASSERT_NE(begin_it, table.end()); EXPECT_EQ(begin_it.key(), 2); EXPECT_EQ(begin_it.value(), "two");
    ++begin_it;
    EXPECT_EQ(begin_it, table.end());
}


TEST(OrderedTableTest, FindElements) {
    TOrderedTable<int, std::string> table;
    table.insert(1, "one");
    table.insert(3, "three");
    table.insert(2, "two"); // Вставляем не по порядку

    // Ищем существующие ключи
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

    // Ищем несуществующие ключи
    EXPECT_EQ(table.find(0), table.end()); // Меньше всех
    EXPECT_EQ(table.find(4), table.end()); // Больше всех
    EXPECT_EQ(table.find(10), table.end()); // Между элементами, которых нет
}

TEST(OrderedTableTest, IndexingOperator) {
    TOrderedTable<int, std::string> table;
    table.insert(1, "one");
    table.insert(2, "two");
    table.insert(3, "three");

    // Доступ к существующим элементам
    EXPECT_EQ(table[1], "one");
    EXPECT_EQ(table[2], "two");
    EXPECT_EQ(table[3], "three");

    // Изменение значения через оператор [] (только для неконстантной таблицы)
    table[1] = "odin";
    EXPECT_EQ(table[1], "odin");

    // Попытка доступа к несуществующему элементу (должно бросить исключение)
    EXPECT_THROW(table[4], std::out_of_range);
    EXPECT_THROW(table[0], std::out_of_range);
}

TEST(OrderedTableTest, EraseExistingElementsReturnIterator) {
    TOrderedTable<int, std::string> table;
    table.insert(1, "one");
    table.insert(2, "two");
    table.insert(3, "three");
    table.insert(4, "four");

    EXPECT_EQ(table.size(), 4);

    // Удаляем элемент в середине (2)
    auto it_after_2 = table.erase(2);
    EXPECT_EQ(table.size(), 3);
    EXPECT_EQ(table.find(2), table.end()); // Проверяем, что удален
    ASSERT_NE(it_after_2, table.end()); // Проверяем, что вернулся не end()
    EXPECT_EQ(it_after_2.key(), 3); // Проверяем, что вернулся итератор на следующий элемент (3)

    // Удаляем первый оставшийся элемент (1)
    auto it_after_1 = table.erase(1);
    EXPECT_EQ(table.size(), 2);
    EXPECT_EQ(table.find(1), table.end());
    ASSERT_NE(it_after_1, table.end());
    EXPECT_EQ(it_after_1.key(), 3); // Проверяем, что вернулся итератор на следующий элемент (3)

    // Удаляем еще один элемент (3)
    auto it_after_3 = table.erase(3);
    EXPECT_EQ(table.size(), 1);
    EXPECT_EQ(table.find(3), table.end());
    ASSERT_NE(it_after_3, table.end());
    EXPECT_EQ(it_after_3.key(), 4); // Проверяем, что вернулся итератор на следующий элемент (4)

    // Удаляем последний оставшийся элемент (4)
    auto it_after_4 = table.erase(4);
    EXPECT_EQ(table.size(), 0);
    EXPECT_TRUE(table.empty());
    EXPECT_EQ(table.find(4), table.end());
    EXPECT_EQ(it_after_4, table.end()); // Проверяем, что вернулся end()

    // Удаляем из пустой таблицы
    EXPECT_EQ(table.erase(100), table.end()); // Должен вернуть end()
    EXPECT_EQ(table.size(), 0);
}

TEST(OrderedTableTest, EraseNonExistingElements) {
    TOrderedTable<int, std::string> table;
    table.insert(1, "one");
    table.insert(3, "three");

    EXPECT_EQ(table.size(), 2);

    // Попытка удалить ключ меньше наименьшего
    auto it_erase_0 = table.erase(0);
    EXPECT_EQ(it_erase_0, table.end()); // Должен вернуть end()
    EXPECT_EQ(table.size(), 2); // Размер не должен измениться
    EXPECT_NE(table.find(1), table.end()); // Элементы не должны быть удалены

    // Попытка удалить ключ между существующими
    auto it_erase_2 = table.erase(2);
    EXPECT_EQ(it_erase_2, table.end()); // Должен вернуть end()
    EXPECT_EQ(table.size(), 2); // Размер не должен измениться
    EXPECT_NE(table.find(1), table.end());
    EXPECT_NE(table.find(3), table.end());

    // Попытка удалить ключ больше наибольшего
    auto it_erase_4 = table.erase(4);
    EXPECT_EQ(it_erase_4, table.end()); // Должен вернуть end()
    EXPECT_EQ(table.size(), 2); // Размер не должен измениться
    EXPECT_NE(table.find(1), table.end());
    EXPECT_NE(table.find(3), table.end());
}


TEST(OrderedTableTest, IteratorDereferenceAndAccess) {
    TOrderedTable<int, std::string> table;
    table.insert(1, "a");
    table.insert(2, "b");

    auto it = table.begin();
    ASSERT_NE(it, table.end());

    // Проверка оператора *
    std::pair<int, std::string>& pair_ref = *it;
    EXPECT_EQ(pair_ref.first, 1);
    EXPECT_EQ(pair_ref.second, "a");

    // Проверка оператора ->
    EXPECT_EQ(it->first, 1);
    EXPECT_EQ(it->second, "a");

    // Проверка кастомных методов key() и value()
    EXPECT_EQ(it.key(), 1);
    EXPECT_EQ(it.value(), "a");

    // Переходим ко второму элементу
    ++it;
    ASSERT_NE(it, table.end());

    // Проверка оператора * на втором элементе
    std::pair<int, std::string>& pair_ref2 = *it;
    EXPECT_EQ(pair_ref2.first, 2);
    EXPECT_EQ(pair_ref2.second, "b");

    // Проверка оператора -> на втором элементе
    EXPECT_EQ(it->first, 2);
    EXPECT_EQ(it->second, "b");

    // Проверка кастомных методов key() и value() на втором элементе
    EXPECT_EQ(it.key(), 2);
    EXPECT_EQ(it.value(), "b");

    // Переходим к end()
    ++it;
    EXPECT_EQ(it, table.end());
}


TEST(OrderedTableTest, IteratorComparison) {
    TOrderedTable<int, std::string> table;
    table.insert(1, "a");
    table.insert(2, "b");
    table.insert(3, "c");

    auto it1 = table.begin(); // Points to 1
    auto it2 = table.begin(); // Points to 1
    auto it3 = table.begin(); ++it3; // Points to 2
    auto it_end = table.end();

    EXPECT_TRUE(it1 == it2);
    EXPECT_FALSE(it1 != it2);

    EXPECT_FALSE(it1 == it3);
    EXPECT_TRUE(it1 != it3);

    EXPECT_FALSE(it1 == it_end);
    EXPECT_TRUE(it1 != it_end);

    EXPECT_TRUE(it_end == table.end());
    EXPECT_FALSE(it_end != table.end());
}












