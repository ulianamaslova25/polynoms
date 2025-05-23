#include "gtest.h" 
#include "TOrderedTable.h" 
#include <string>      
#include <vector>      
#include <stdexcept>


// ���� ���� ��� ���, ������� ��� ��� ������ ������
TEST(OrderedTableTest, EmptyTable) {
    TOrderedTable<int, std::string> table;
    EXPECT_EQ(table.size(), 0);
    EXPECT_TRUE(table.empty());
    EXPECT_EQ(table.find(10), table.end());
    EXPECT_EQ(table.erase(10), table.end());
    // ������������� ��������� �������� [] �� ������ ������� (������ ������� ����������)
    EXPECT_THROW(table[10], std::out_of_range);
}

TEST(OrderedTableTest, InsertElementsMaintainOrder) {
    TOrderedTable<int, std::string> table;

    // ��������� � ������ �������, ���������, ��� ����������� ���������� �� �����
    table.insert(2, "two");
    table.insert(1, "one");
    table.insert(3, "three");
    table.insert(0, "zero");
    table.insert(5, "five");

    EXPECT_EQ(table.size(), 5);
    EXPECT_FALSE(table.empty());

    // ��������� ������� � ��������, ��������� ���������
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
    table.insert(1, "odin"); // ��������� �������� ����� 1

    EXPECT_EQ(table.size(), 2); // ������ �� ������ ����������

    // ���������, ��� �������� ����������
    auto it = table.find(1);
    ASSERT_NE(it, table.end()); // ���� ������ ���� ������
    EXPECT_EQ(it.value(), "odin"); // �������� ������ ���� �����������
    EXPECT_EQ(table[1], "odin");   // �������� ����� �������� []

    // ���������, ��� ������ �������� �� ���������
    auto it2 = table.find(2);
    ASSERT_NE(it2, table.end());
    EXPECT_EQ(it2.value(), "two");
    EXPECT_EQ(table[2], "two");

    // ��������� ������� ����� ����������
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
    table.insert(2, "two"); // ��������� �� �� �������

    // ���� ������������ �����
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

    // ���� �������������� �����
    EXPECT_EQ(table.find(0), table.end()); // ������ ����
    EXPECT_EQ(table.find(4), table.end()); // ������ ����
    EXPECT_EQ(table.find(10), table.end()); // ����� ����������, ������� ���
}

TEST(OrderedTableTest, IndexingOperator) {
    TOrderedTable<int, std::string> table;
    table.insert(1, "one");
    table.insert(2, "two");
    table.insert(3, "three");

    // ������ � ������������ ���������
    EXPECT_EQ(table[1], "one");
    EXPECT_EQ(table[2], "two");
    EXPECT_EQ(table[3], "three");

    // ��������� �������� ����� �������� [] (������ ��� ������������� �������)
    table[1] = "odin";
    EXPECT_EQ(table[1], "odin");

    // ������� ������� � ��������������� �������� (������ ������� ����������)
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

    // ������� ������� � �������� (2)
    auto it_after_2 = table.erase(2);
    EXPECT_EQ(table.size(), 3);
    EXPECT_EQ(table.find(2), table.end()); // ���������, ��� ������
    ASSERT_NE(it_after_2, table.end()); // ���������, ��� �������� �� end()
    EXPECT_EQ(it_after_2.key(), 3); // ���������, ��� �������� �������� �� ��������� ������� (3)

    // ������� ������ ���������� ������� (1)
    auto it_after_1 = table.erase(1);
    EXPECT_EQ(table.size(), 2);
    EXPECT_EQ(table.find(1), table.end());
    ASSERT_NE(it_after_1, table.end());
    EXPECT_EQ(it_after_1.key(), 3); // ���������, ��� �������� �������� �� ��������� ������� (3)

    // ������� ��� ���� ������� (3)
    auto it_after_3 = table.erase(3);
    EXPECT_EQ(table.size(), 1);
    EXPECT_EQ(table.find(3), table.end());
    ASSERT_NE(it_after_3, table.end());
    EXPECT_EQ(it_after_3.key(), 4); // ���������, ��� �������� �������� �� ��������� ������� (4)

    // ������� ��������� ���������� ������� (4)
    auto it_after_4 = table.erase(4);
    EXPECT_EQ(table.size(), 0);
    EXPECT_TRUE(table.empty());
    EXPECT_EQ(table.find(4), table.end());
    EXPECT_EQ(it_after_4, table.end()); // ���������, ��� �������� end()

    // ������� �� ������ �������
    EXPECT_EQ(table.erase(100), table.end()); // ������ ������� end()
    EXPECT_EQ(table.size(), 0);
}

TEST(OrderedTableTest, EraseNonExistingElements) {
    TOrderedTable<int, std::string> table;
    table.insert(1, "one");
    table.insert(3, "three");

    EXPECT_EQ(table.size(), 2);

    // ������� ������� ���� ������ �����������
    auto it_erase_0 = table.erase(0);
    EXPECT_EQ(it_erase_0, table.end()); // ������ ������� end()
    EXPECT_EQ(table.size(), 2); // ������ �� ������ ����������
    EXPECT_NE(table.find(1), table.end()); // �������� �� ������ ���� �������

    // ������� ������� ���� ����� �������������
    auto it_erase_2 = table.erase(2);
    EXPECT_EQ(it_erase_2, table.end()); // ������ ������� end()
    EXPECT_EQ(table.size(), 2); // ������ �� ������ ����������
    EXPECT_NE(table.find(1), table.end());
    EXPECT_NE(table.find(3), table.end());

    // ������� ������� ���� ������ �����������
    auto it_erase_4 = table.erase(4);
    EXPECT_EQ(it_erase_4, table.end()); // ������ ������� end()
    EXPECT_EQ(table.size(), 2); // ������ �� ������ ����������
    EXPECT_NE(table.find(1), table.end());
    EXPECT_NE(table.find(3), table.end());
}


TEST(OrderedTableTest, IteratorDereferenceAndAccess) {
    TOrderedTable<int, std::string> table;
    table.insert(1, "a");
    table.insert(2, "b");

    auto it = table.begin();
    ASSERT_NE(it, table.end());

    // �������� ��������� *
    std::pair<int, std::string>& pair_ref = *it;
    EXPECT_EQ(pair_ref.first, 1);
    EXPECT_EQ(pair_ref.second, "a");

    // �������� ��������� ->
    EXPECT_EQ(it->first, 1);
    EXPECT_EQ(it->second, "a");

    // �������� ��������� ������� key() � value()
    EXPECT_EQ(it.key(), 1);
    EXPECT_EQ(it.value(), "a");

    // ��������� �� ������� ��������
    ++it;
    ASSERT_NE(it, table.end());

    // �������� ��������� * �� ������ ��������
    std::pair<int, std::string>& pair_ref2 = *it;
    EXPECT_EQ(pair_ref2.first, 2);
    EXPECT_EQ(pair_ref2.second, "b");

    // �������� ��������� -> �� ������ ��������
    EXPECT_EQ(it->first, 2);
    EXPECT_EQ(it->second, "b");

    // �������� ��������� ������� key() � value() �� ������ ��������
    EXPECT_EQ(it.key(), 2);
    EXPECT_EQ(it.value(), "b");

    // ��������� � end()
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












