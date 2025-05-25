#include "ChainHashTable.h" 

#include <gtest.h>  
#include <string>        
#include <set>          
#include <algorithm>    

TEST(ChainHashTable, canJustExist) {
	ChainHashTable<int, double> map;
}

TEST(ChainHashTable, canInsertAnElement) {
	ChainHashTable<int, double> map;
	ASSERT_NO_THROW(map.insert(1, 2.0));
}

TEST(ChainHashTable, canInsertSeveralElements) {
	ChainHashTable<int, double> map;
	map.insert(1, 2.0);
	ASSERT_NO_THROW(map.insert(2, 4.0));
}

TEST(ChainHashTable, throwsForInsertionOfTheAlreadyExistantElement) {
	ChainHashTable<int, double> map;
	size_t st_cap = map.getCapacity();
	for (int i = 0; i < 5; i++) {
		map.insert(i, i);
	}
	ASSERT_ANY_THROW(map.insert(3, 3));
}

TEST(ChainHashTable, canFindAnExistantElement) {
	ChainHashTable<int, double> map;
	for (int i = 0; i < 5; i++) {
		map.insert(i, i);
	}
	ASSERT_NO_THROW(map.find(3));
}

TEST(ChainHashTable, canCorrectlyFindAnExistantElement) {
	ChainHashTable<int, double> map;
	for (int i = 0; i < 5; i++) {
		map.insert(i, i);
	}
	EXPECT_EQ(3.0, (*map.find(3)).second);
}

TEST(ChainHashTable, findingANonExistantElementReturnsIteratorToTheEnd) {
	ChainHashTable<int, double> map;
	for (int i = 0; i < 5; i++) {
		map.insert(i, i);
	}
	EXPECT_EQ(map.end(), map.find(6));
}

TEST(ChainHashTable, canEraseAnExistantElement) {
	ChainHashTable<int, double> map;
	for (int i = 0; i < 5; i++) {
		map.insert(i, (double)i);
	}
	ASSERT_NO_THROW(map.erase(3));
}

TEST(ChainHashTable, erasureReturnsIteratorToTheNextElement) {
	ChainHashTable<int, double> map;
	for (int i = 0; i < 5; i++) {
		map.insert(i, i);
	}
	auto it = map.find(3);
	it++;
	EXPECT_EQ(it, map.erase(3));
}

TEST(ChainHashTable, throwsForErasureOfANonExistantElement) {
	ChainHashTable<int, double> map;
	for (int i = 0; i < 5; i++) {
		map.insert(i, i);
	}

	ASSERT_ANY_THROW(map.erase(6));
}

TEST(ChainHashTable, canReturnValueOfAnExistingElement) {
	ChainHashTable<int, double> a;
	size_t n = 4;
	for (int i = 0; i < n; i++) a.insert(i, i);

	ASSERT_NO_THROW(a[2]);
}

TEST(ChainHashTable, canReturnCorrectValueOfAnExistingElement) {
	ChainHashTable<int, double> a;
	size_t n = 4;
	for (int i = 0; i < n; i++) a.insert(i, i);

	double exp = 2.0;

	EXPECT_EQ(exp, a[2]);
}

TEST(ChainHashTable, canReturnSmthIfTheKeyDoesntExist) {
	ChainHashTable<int, double> a;
	size_t n = 4;
	for (int i = 0; i < n; i++) a.insert(i, i);

	ASSERT_NO_THROW(a[5]);
}

TEST(ChainHashTable, insertsTheElementWithDefaultValueIfSuchKeyDoesntExist) {
	ChainHashTable<int, double> a;
	size_t n = 4;
	for (int i = 0; i < n; i++) a.insert(i, i);

	double p = a[5];

	EXPECT_EQ(double(), p);
}

TEST(ChainHashTable, someTestsForStringKey) {
	ChainHashTable<std::string, double> map;
	std::vector<std::string> keys = { "1st", "2nd", "3hd", "4th", "5th" };

	for (int i = 0; i < 5; i++) {
		ASSERT_NO_THROW(map.insert(keys[i], i));
	}

}
