#include "gtest.h"
#include <string>
#include <vector>
#include <set> // Для проверки уникальности ключей и сравнения содержимого
#include "../include/HashTableOpenAddressing.h"
#include <map>

// --- Тесты для HashTableOpenAddressing ---

// Тест на конструктор и начальное состояние
TEST(HashTableOpenAddressingTest, ConstructionAndEmptyState) {
    HashTableOpenAddressing<int, std::string> ht;
    EXPECT_TRUE(ht.empty());
    EXPECT_EQ(ht.size(), 0);

    HashTableOpenAddressing<int, std::string> ht_with_capacity(32);
    EXPECT_TRUE(ht_with_capacity.empty());
    EXPECT_EQ(ht_with_capacity.size(), 0);
}

// Тест на вставку одного элемента и его поиск
TEST(HashTableOpenAddressingTest, SingleInsertAndFind) {
    HashTableOpenAddressing<int, std::string> ht;
    EXPECT_TRUE(ht.insert(10, "apple"));
    EXPECT_EQ(ht.size(), 1);
    EXPECT_FALSE(ht.empty());

    std::string* value = ht.find(10);
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(*value, "apple");

    // Поиск несуществующего
    EXPECT_EQ(ht.find(20), nullptr);
}

// Тест на обновление значения при вставке существующего ключа
TEST(HashTableOpenAddressingTest, InsertExistingKeyUpdatesValue) {
    HashTableOpenAddressing<int, std::string> ht;
    ht.insert(10, "apple");
    EXPECT_FALSE(ht.insert(10, "apricot")); // Должно вернуть false (обновление)
    EXPECT_EQ(ht.size(), 1); // Размер не должен измениться

    std::string* value = ht.find(10);
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(*value, "apricot");
}

// Тест на оператор []
TEST(HashTableOpenAddressingTest, IndexOperator) {
    HashTableOpenAddressing<int, std::string> ht;

    // Вставка через []
    ht[5] = "banana";
    EXPECT_EQ(ht.size(), 1);
    std::string* value1 = ht.find(5);
    ASSERT_NE(value1, nullptr);
    EXPECT_EQ(*value1, "banana");
    EXPECT_EQ(ht[5], "banana"); // Получение значения

    // Обновление через []
    ht[5] = "blueberry";
    EXPECT_EQ(ht.size(), 1);
    std::string* value2 = ht.find(5);
    ASSERT_NE(value2, nullptr);
    EXPECT_EQ(*value2, "blueberry");
    EXPECT_EQ(ht[5], "blueberry");

    // Вставка нового элемента со значением по умолчанию (пустая строка для std::string)
    std::string default_val = ht[15];
    EXPECT_EQ(ht.size(), 2);
    EXPECT_EQ(default_val, ""); // std::string()
    std::string* value3 = ht.find(15);
    ASSERT_NE(value3, nullptr);
    EXPECT_EQ(*value3, "");
}

// Тест на удаление элемента
TEST(HashTableOpenAddressingTest, EraseElement) {
    HashTableOpenAddressing<int, std::string> ht;
    ht.insert(10, "apple");
    ht.insert(20, "banana");
    EXPECT_EQ(ht.size(), 2);

    EXPECT_TRUE(ht.erase(10)); // Удаляем существующий
    EXPECT_EQ(ht.size(), 1);
    EXPECT_EQ(ht.find(10), nullptr); // Больше не должен находиться
    ASSERT_NE(ht.find(20), nullptr); // Другой элемент должен остаться
    EXPECT_EQ(*ht.find(20), "banana");

    EXPECT_FALSE(ht.erase(10)); // Попытка удалить снова
    EXPECT_FALSE(ht.erase(30)); // Попытка удалить несуществующий
    EXPECT_EQ(ht.size(), 1);
}

// Тест на обработку коллизий (с линейным пробированием)
// Для этого теста нужно знать размер бакета и хеш-функцию.
// Мы можем спровоцировать коллизию, если хеши одинаковы.
struct BadHasher {
    size_t operator()(int key) const {
        return key % 5; // Все ключи, кратные 5, будут иметь один хеш, +1, +2 и т.д.
    }
};

TEST(HashTableOpenAddressingTest, CollisionHandlingLinearProbing) {
    HashTableOpenAddressing<int, std::string, BadHasher> ht(7); // Маленькая таблица для легкой коллизии

    // Ключи 0, 5, 10 будут иметь хеш 0 (при capacity=7, (0+i)%7, (5+i)%7, (10=3+i)%7 )
    // hash(0)=0, hash(5)=5, hash(10)=3. Нам нужны ключи с одинаковым начальным хешем.
    // BadHasher(0) % 7 = 0
    // BadHasher(7) % 7 = 0
    // BadHasher(14) % 7 = 0
    EXPECT_TRUE(ht.insert(0, "zero"));   // Пойдет в (0%7 + 0)%7 = 0
    EXPECT_TRUE(ht.insert(7, "seven"));  // (7%7 + 0)%7 = 0 (коллизия), пойдет в (0+1)%7 = 1
    EXPECT_TRUE(ht.insert(14, "fourteen"));// (14%7 +0)%7 = 0 (коллизия), (0+1)%7=1 (коллизия), пойдет в (0+2)%7 = 2

    EXPECT_EQ(ht.size(), 3);

    ASSERT_NE(ht.find(0), nullptr); EXPECT_EQ(*ht.find(0), "zero");
    ASSERT_NE(ht.find(7), nullptr); EXPECT_EQ(*ht.find(7), "seven");
    ASSERT_NE(ht.find(14), nullptr); EXPECT_EQ(*ht.find(14), "fourteen");

    // Удалим средний элемент (7), чтобы создать DELETED слот
    EXPECT_TRUE(ht.erase(7));
    EXPECT_EQ(ht.size(), 2);
    EXPECT_EQ(ht.find(7), nullptr);

    // 14 все еще должен находиться, пройдя через DELETED слот
    ASSERT_NE(ht.find(14), nullptr); EXPECT_EQ(*ht.find(14), "fourteen");

    // Вставим новый элемент, который должен занять DELETED слот
    EXPECT_TRUE(ht.insert(21, "twentyone")); // (21%7+0)%7 = 0 (колл.), (0+1)%7 = 1 (бывший DELETED)
    EXPECT_EQ(ht.size(), 3);
    ASSERT_NE(ht.find(21), nullptr); EXPECT_EQ(*ht.find(21), "twentyone");
}

// Тест на рехеширование
TEST(HashTableOpenAddressingTest, Rehashing) {
    HashTableOpenAddressing<int, std::string> ht(4); // Очень маленькая начальная емкость
    // MAX_LOAD_FACTOR = 0.7. Рехеш будет при 4 * 0.7 = 2.8, т.е. при вставке 3-го элемента.

    ht.insert(1, "A");
    ht.insert(2, "B");
    EXPECT_EQ(ht.size(), 2);
    // ht.printDebug(); // Ожидаемая capacity = 4

    ht.insert(3, "C"); // Здесь должен произойти рехеш
    EXPECT_EQ(ht.size(), 3);
    // ht.printDebug(); // Ожидаемая capacity = 8 (или около того, если другая логика)

    // Проверяем, что все элементы доступны после рехеша
    ASSERT_NE(ht.find(1), nullptr); EXPECT_EQ(*ht.find(1), "A");
    ASSERT_NE(ht.find(2), nullptr); EXPECT_EQ(*ht.find(2), "B");
    ASSERT_NE(ht.find(3), nullptr); EXPECT_EQ(*ht.find(3), "C");

    ht.insert(4, "D");
    ht.insert(5, "E");
    ht.insert(6, "F"); // Еще один рехеш при 8 * 0.7 = 5.6, т.е. при вставке 6-го элемента
    EXPECT_EQ(ht.size(), 6);
    // ht.printDebug(); // Ожидаемая capacity = 16

    ASSERT_NE(ht.find(1), nullptr); EXPECT_EQ(*ht.find(1), "A");
    ASSERT_NE(ht.find(2), nullptr); EXPECT_EQ(*ht.find(2), "B");
    ASSERT_NE(ht.find(3), nullptr); EXPECT_EQ(*ht.find(3), "C");
    ASSERT_NE(ht.find(4), nullptr); EXPECT_EQ(*ht.find(4), "D");
    ASSERT_NE(ht.find(5), nullptr); EXPECT_EQ(*ht.find(5), "E");
    ASSERT_NE(ht.find(6), nullptr); EXPECT_EQ(*ht.find(6), "F");
}

// Тест с большим количеством вставок и удалений
TEST(HashTableOpenAddressingTest, StressTest) {
    HashTableOpenAddressing<int, int> ht;
    std::map<int, int> reference_map; // Эталон для сравнения

    const int num_ops = 5000;
    std::vector<int> keys_inserted; // Храним ключи, чтобы потом их удалять

    for (int i = 0; i < num_ops; ++i) {
        int key = rand() % (num_ops / 2); // Генерируем ключи в меньшем диапазоне для коллизий и обновлений
        int value = rand();

        if (rand() % 3 == 0 && !keys_inserted.empty()) { // Удаление (примерно 1/3 операций)
            int random_idx = rand() % keys_inserted.size();
            int key_to_erase = keys_inserted[random_idx];

            bool ht_erased = ht.erase(key_to_erase);
            bool map_erased = (reference_map.erase(key_to_erase) > 0);
            EXPECT_EQ(ht_erased, map_erased);

            if (ht_erased) {
                // Удаляем из нашего списка, чтобы не пытаться удалить снова (хотя erase это обработает)
                keys_inserted.erase(keys_inserted.begin() + random_idx);
            }
        }
        else { // Вставка или обновление
            ht[key] = value; // Используем operator[] для удобства
            reference_map[key] = value;
            // Добавляем в keys_inserted, только если это действительно новая вставка (для простоты можно всегда добавлять и положиться на уникальность в map)
            if (std::find(keys_inserted.begin(), keys_inserted.end(), key) == keys_inserted.end()) {
                keys_inserted.push_back(key);
            }
        }
        EXPECT_EQ(ht.size(), reference_map.size());
    }

    // Финальная проверка содержимого
    EXPECT_EQ(ht.size(), reference_map.size());
    for (const auto& pair : reference_map) {
        const int* ht_value = ht.find(pair.first);
        ASSERT_NE(ht_value, nullptr) << "Key " << pair.first << " missing in HashTable";
        EXPECT_EQ(*ht_value, pair.second) << "Value mismatch for key " << pair.first;
    }
    //for (size_t i = 0; i < ht.capacity /* это приватное поле, так что не можем */; ++i) {
        // Если бы был итератор, могли бы проверить, что все элементы из ht есть в map
    //}
}

TEST(HashTableOpenAddressingTest, ClearTable) {
    HashTableOpenAddressing<int, std::string> ht;
    ht.insert(1, "A");
    ht.insert(2, "B");
    ht.insert(3, "C");
    EXPECT_EQ(ht.size(), 3);
    EXPECT_FALSE(ht.empty());

    ht.clear();
    EXPECT_EQ(ht.size(), 0);
    EXPECT_TRUE(ht.empty());
    EXPECT_EQ(ht.find(1), nullptr);
    EXPECT_EQ(ht.find(2), nullptr);
    EXPECT_EQ(ht.find(3), nullptr);

    // Проверяем, что после clear можно снова вставлять
    ht.insert(4, "D");
    EXPECT_EQ(ht.size(), 1);
    ASSERT_NE(ht.find(4), nullptr);
    EXPECT_EQ(*ht.find(4), "D");
}

TEST(HashTableOpenAddressingTest, CopyConstructorAndAssignment) {
    HashTableOpenAddressing<int, std::string> ht1;
    ht1.insert(10, "cat");
    ht1.insert(20, "dog");
    ht1[30] = "rat"; // Используем [] для вставки

    // Копирующий конструктор
    HashTableOpenAddressing<int, std::string> ht2 = ht1;
    EXPECT_EQ(ht1.size(), ht2.size());
    ASSERT_NE(ht2.find(10), nullptr); EXPECT_EQ(*ht2.find(10), "cat");
    ASSERT_NE(ht2.find(20), nullptr); EXPECT_EQ(*ht2.find(20), "dog");
    ASSERT_NE(ht2.find(30), nullptr); EXPECT_EQ(*ht2.find(30), "rat");

    // Проверка независимости
    ht2.insert(40, "bat");
    EXPECT_EQ(ht1.find(40), nullptr); // Не должно быть в оригинале
    EXPECT_EQ(ht1.size(), 3);
    EXPECT_EQ(ht2.size(), 4);

    ht1.erase(10);
    ASSERT_NE(ht2.find(10), nullptr); // Должно остаться в копии
    EXPECT_EQ(*ht2.find(10), "cat");
    EXPECT_EQ(ht1.size(), 2);

    // Оператор присваивания
    HashTableOpenAddressing<int, std::string> ht3;
    ht3.insert(5, "owl");
    ht3 = ht1; // ht1 сейчас содержит {20:dog, 30:rat}

    EXPECT_EQ(ht3.size(), 2);
    EXPECT_EQ(ht3.find(5), nullptr); // Старого элемента быть не должно
    ASSERT_NE(ht3.find(20), nullptr); EXPECT_EQ(*ht3.find(20), "dog");
    ASSERT_NE(ht3.find(30), nullptr); EXPECT_EQ(*ht3.find(30), "rat");

    // Проверка независимости
    ht3.insert(50, "fox");
    EXPECT_EQ(ht1.find(50), nullptr);
    EXPECT_EQ(ht1.size(), 2);
    EXPECT_EQ(ht3.size(), 3);
}

#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm> // для std::sort, std::all_of
#include <random>
#include <chrono>

// --- Расширенные тесты для HashTableOpenAddressing ---

// Вспомогательная функция для сбора всех элементов таблицы в std::map
template<class K, class V>
std::map<K, V> getHashTableContentAsMap(HashTableOpenAddressing<K, V>& ht) { // Принимаем по неконстантной ссылке, т.к. begin/end не const
    std::map<K, V> content;
    for (auto it = ht.begin(); it != ht.end(); ++it) {
        content[it->first] = it->second;
    }
    return content;
}

TEST(HashTableOpenAddressingAdvTest, ConstructionAndEmptyState) {
    HashTableOpenAddressing<int, std::string> ht;
    EXPECT_TRUE(ht.empty());
    EXPECT_EQ(ht.size(), 0);
    EXPECT_EQ(ht.begin(), ht.end());
}

TEST(HashTableOpenAddressingAdvTest, IteratorBasic) {
    HashTableOpenAddressing<int, std::string> ht;
    ht.insert(10, "apple");
    ht.insert(20, "banana");
    ht.insert(5, "cherry"); // Порядок вставки не гарантирует порядок итерации

    std::map<int, std::string> expected_content;
    expected_content[10] = "apple";
    expected_content[20] = "banana";
    expected_content[5] = "cherry";

    std::map<int, std::string> actual_content = getHashTableContentAsMap(ht);
    EXPECT_EQ(actual_content.size(), 3);
    EXPECT_EQ(actual_content, expected_content); // Сравниваем содержимое map

    // Проверка изменения значения через итератор
    auto it_find = ht.begin();
    while (it_find != ht.end() && it_find->first != 10) { ++it_find; } // Найдем элемент с ключом 10
    ASSERT_NE(it_find, ht.end());
    // (*it_find).second = "apricot"; // Ошибка: operator* возвращает const TKey
    // Для изменения значения нужно получить TValue* через find или operator[]
    ht[10] = "apricot"; // Изменяем через []

    expected_content[10] = "apricot";
    actual_content = getHashTableContentAsMap(ht);
    EXPECT_EQ(actual_content, expected_content);
}

TEST(HashTableOpenAddressingAdvTest, IteratorSkipsEmptyAndDeleted_Predictable) {
    HashTableOpenAddressing<int, std::string> ht(7); // Емкость 7
    // Индексы будут N % 7
    ht.insert(1, "A");  // -> idx 1
    ht.insert(2, "B");  // -> idx 2
    ht.insert(3, "C");  // -> idx 3
    ht.insert(4, "D");  // -> idx 4
    ht.insert(5, "E");  // -> idx 5
    EXPECT_EQ(ht.size(), 5);
    // ht.printDebug(); 
    // Ожидаемо: [1:A, 2:B, 3:C, 4:D, 5:E], остальные EMPTY

    ht.erase(2); // Слот на idx 2 (B) -> DELETED
    ht.erase(4); // Слот на idx 4 (D) -> DELETED
    EXPECT_EQ(ht.size(), 3);
    // ht.printDebug();
    // Ожидаемо: [1:A, 2:DEL, 3:C, 4:DEL, 5:E]

    std::map<int, std::string> expected_content;
    expected_content[1] = "A";
    expected_content[3] = "C";
    expected_content[5] = "E";

    std::map<int, std::string> actual_content = getHashTableContentAsMap(ht);
    EXPECT_EQ(actual_content.size(), expected_content.size());
    EXPECT_EQ(actual_content, expected_content);

    // Вставим элемент, который должен занять DELETED слот (например, idx 2)
    // Если хеш(9) = 9, то 9 % 7 = 2.
    ht.insert(9, "F"); // Должен пойти в idx 2 (бывший DELETED)
    EXPECT_EQ(ht.size(), 4);
    // ht.printDebug();
    // Ожидаемо: [1:A, 2:F, 3:C, 4:DEL, 5:E]

    expected_content[9] = "F";
    actual_content = getHashTableContentAsMap(ht);
    EXPECT_EQ(actual_content.size(), expected_content.size());
    EXPECT_EQ(actual_content, expected_content);
}

TEST(HashTableOpenAddressingAdvTest, IterateEmptyTable) {
    HashTableOpenAddressing<int, int> ht;
    int count = 0;
    for (auto it = ht.begin(); it != ht.end(); ++it) {
        count++;
    }
    EXPECT_EQ(count, 0);
    EXPECT_EQ(ht.begin(), ht.end());
}

TEST(HashTableOpenAddressingAdvTest, IterateAfterRehash) {
    HashTableOpenAddressing<int, std::string> ht(3); // Очень маленькая capacity для быстрого рехеша
    ht.insert(1, "one");
    ht.insert(2, "two"); // Загрузка 2/3 ~ 0.66, близко к MAX_LOAD_FACTOR = 0.7

    std::map<int, std::string> expected1;
    expected1[1] = "one";
    expected1[2] = "two";
    EXPECT_EQ(getHashTableContentAsMap(ht), expected1);

    ht.insert(3, "three"); // Рехеш здесь, capacity станет ~6
    ht.insert(4, "four");

    std::map<int, std::string> expected2;
    expected2[1] = "one";
    expected2[2] = "two";
    expected2[3] = "three";
    expected2[4] = "four";
    EXPECT_EQ(getHashTableContentAsMap(ht), expected2);
    EXPECT_EQ(ht.size(), 4);
}


TEST(HashTableOpenAddressingAdvTest, RandomOperationsWithIteratorChecks) {
    HashTableOpenAddressing<int, int> ht;
    std::map<int, int> reference_map;

    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> key_dist(0, 200); // Меньший диапазон для большего числа коллизий/обновлений
    std::uniform_int_distribution<int> val_dist(0, 10000);
    std::uniform_int_distribution<int> op_dist(0, 3); // 0: insert, 1: erase, 2: find, 3: iterate_and_check

    const int num_operations = 2000;
    std::vector<int> inserted_keys_for_random_erase;


    for (int i = 0; i < num_operations; ++i) {
        int op_type = op_dist(rng);
        int key = key_dist(rng);

        if (op_type == 0) { // Insert
            int value = val_dist(rng);
            ht.insert(key, value);
            reference_map[key] = value;
            if (std::find(inserted_keys_for_random_erase.begin(), inserted_keys_for_random_erase.end(), key) == inserted_keys_for_random_erase.end()) {
                inserted_keys_for_random_erase.push_back(key);
            }
        }
        else if (op_type == 1 && !inserted_keys_for_random_erase.empty()) { // Erase
            int rand_idx = rng() % inserted_keys_for_random_erase.size();
            int key_to_erase = inserted_keys_for_random_erase[rand_idx];

            bool ht_erased = ht.erase(key_to_erase);
            bool map_erased = (reference_map.erase(key_to_erase) > 0);
            EXPECT_EQ(ht_erased, map_erased);
            if (map_erased) { // Если действительно удалили из эталона
                inserted_keys_for_random_erase.erase(inserted_keys_for_random_erase.begin() + rand_idx);
            }
        }
        else if (op_type == 2) { // Find
            const int* ht_value = ht.find(key);
            auto map_it = reference_map.find(key);
            if (map_it == reference_map.end()) {
                EXPECT_EQ(ht_value, nullptr);
            }
            else {
                ASSERT_NE(ht_value, nullptr);
                EXPECT_EQ(*ht_value, map_it->second);
            }
        }
        else { // Iterate and check (op_type == 3 or if erase was skipped)
            std::map<int, int> current_ht_content = getHashTableContentAsMap(ht);
            EXPECT_EQ(current_ht_content, reference_map) << "Full content mismatch at op " << i;
        }
        EXPECT_EQ(ht.size(), reference_map.size());
    }
    // Финальная проверка
    std::map<int, int> final_ht_content = getHashTableContentAsMap(ht);
    EXPECT_EQ(final_ht_content, reference_map);
}

TEST(HashTableOpenAddressingAdvTest, DereferenceEndIteratorThrows) {
    HashTableOpenAddressing<int, int> ht;
    ht.insert(1, 1);
    auto it_end = ht.end();
    EXPECT_THROW((*it_end), std::out_of_range);
    EXPECT_THROW(it_end->first, std::out_of_range);

    HashTableOpenAddressing<int, int> empty_ht;
    auto it_empty_end = empty_ht.end();
    EXPECT_THROW((*it_empty_end), std::out_of_range);
}