#include "polynomial.h"
#include <cmath> 
#include <sstream> 
#include <limits> 
#include <vector> 

#include <gtest.h>

TEST(MonomialTests, LessThanOperator) {
    Monomial m1(1.0, 2, 3, 4); // x^2y^3z^4
    Monomial m2(1.0, 3, 1, 1); // x^3y^1z^1
    Monomial m3(1.0, 2, 4, 0); // x^2y^4
    Monomial m4(1.0, 2, 3, 5); // x^2y^3z^5
    Monomial m5(1.0, 2, 3, 4); // x^2y^3z^4 (такой же как m1)
    Monomial m6(2.0, 2, 3, 4); // 2x^2y^3z^4 (другой коэф, те же степени)

    // Порядок сортировки по убыванию степеней: (px, py, pz)
    // (3,1,1) > (2,4,0) > (2,3,5) > (2,3,4)
    EXPECT_TRUE(m2 < m3); // 3 > 2 (px)
    EXPECT_TRUE(m3 < m4); // px равны (2), py 4 > 3
    EXPECT_TRUE(m4 < m1); // px равны (2), py равны (3), pz 5 > 4

    // Мономы с одинаковыми степенями не должны считаться "меньшими"
    EXPECT_FALSE(m1 < m5);
    EXPECT_FALSE(m5 < m1);
    EXPECT_FALSE(m1 < m6); // Коэффициент не влияет на оператор <
    EXPECT_FALSE(m6 < m1);
}

TEST(MonomialTests, EqualityOperator) {
    Monomial m1(1.2, 2, 3, 4);
    Monomial m2(1.2, 2, 3, 4);     // Точно такой же
    Monomial m3(1.2 + 1e-10, 2, 3, 4); // Коэффициент очень близок
    Monomial m4(1.3, 2, 3, 4);     // Другой коэффициент
    Monomial m5(1.2, 3, 3, 4);     // Другая степень px
    Monomial m6(1.2, 2, 4, 4);     // Другая степень py
    Monomial m7(1.2, 2, 3, 5);     // Другая степень pz
    Monomial m8(0.0, 0, 0, 0);      // Нулевой моном
    Monomial m9(1e-10, 0, 0, 0);    // Моном с коэффициентом близким к нулю

    EXPECT_TRUE(m1 == m2);
    EXPECT_TRUE(m1 == m3); // Проверка EPSILON
    EXPECT_FALSE(m1 == m4); // Коэффициент отличается
    EXPECT_FALSE(m1 == m5); // Степени отличаются
    EXPECT_FALSE(m1 == m6);
    EXPECT_FALSE(m1 == m7);
    EXPECT_TRUE(m8 == m9); // Оба близки к нулю
    EXPECT_FALSE(m1 == m8); // Совершенно разные моном
}

TEST(MonomialTests, InequalityOperator) {
    Monomial m1(1.2, 2, 3, 4);
    Monomial m2(1.2, 2, 3, 4);     // Точно такой же
    Monomial m3(1.3, 2, 3, 4);     // Другой коэффициент
    Monomial m4(1.2, 3, 3, 4);     // Другая степень px

    EXPECT_FALSE(m1 != m2);
    EXPECT_TRUE(m1 != m3);
    EXPECT_TRUE(m1 != m4);
}


TEST(MonomialTests, EqXYZ) {
    Monomial m1(1.2, 2, 3, 4);
    Monomial m2(5.6, 2, 3, 4); // Тот же степени, другой коэффициент
    Monomial m3(1.2, 3, 3, 4); // Другая степень px
    Monomial m4(1.2, 2, 4, 4); // Другая степень py
    Monomial m5(1.2, 2, 3, 5); // Другая степень pz
    Monomial m6(0.0, 0, 0, 0); // Постоянный член
    Monomial m7(7.0, 0, 0, 0); // Другой постоянный член

    EXPECT_TRUE(m1.EqXYZ(m2)); // Степени одинаковые
    EXPECT_FALSE(m1.EqXYZ(m3)); // px отличается
    EXPECT_FALSE(m1.EqXYZ(m4)); // py отличается
    EXPECT_FALSE(m1.EqXYZ(m5)); // pz отличается
    EXPECT_TRUE(m6.EqXYZ(m7)); // Степени (0,0,0) одинаковые
    EXPECT_FALSE(m1.EqXYZ(m6)); // Разные степени
}

TEST(MonomialTests, MonomialMultiplicationEdgeCases) {
    Monomial m_x(1.0, 1, 0, 0);     // x
    Monomial m_y(1.0, 0, 1, 0);     // y
    Monomial m_z(1.0, 0, 0, 1);     // z
    Monomial m_const(5.0, 0, 0, 0); // 5
    Monomial m_zero(0.0, 0, 0, 0);  // 0
    Monomial m_zero_deg(10.0, 0, 0, 0); // 10 (с px=py=pz=0)

    // x * y = xy
    Monomial res1 = m_x * m_y;
    EXPECT_TRUE((Monomial(1.0, 1, 1, 0) == res1));

    // 5 * x = 5x
    Monomial res2 = m_const * m_x;
    EXPECT_TRUE((Monomial(5.0, 1, 0, 0) == res2));

    // x^2 * x^3 = x^5
    Monomial m_x2(1.0, 2, 0, 0);
    Monomial m_x3(1.0, 3, 0, 0);
    Monomial res3 = m_x2 * m_x3;
    EXPECT_TRUE((Monomial(1.0, 5, 0, 0) == res3));

    // (2x^2y) * (-3z^4) = -6x^2yz^4
    Monomial m_2x2y(2.0, 2, 1, 0);
    Monomial m_neg3z4(-3.0, 0, 0, 4);
    Monomial res4 = m_2x2y * m_neg3z4;
    EXPECT_TRUE((Monomial(-6.0, 2, 1, 4) == res4));

    // Умножение на моном с нулевым коэффициентом (результат - моном с нулевым коэффициентом и суммой степеней)
    Monomial res5 = m_x * Monomial(0.0, 2, 2, 2);
    EXPECT_TRUE((Monomial(0.0, 1 + 2, 0 + 2, 0 + 2) == res5)); // {0.0, 3, 2, 2}

    // Умножение на нулевой моном (специальный случай нулевого коэффициента)
    Monomial res6 = m_x * Monomial(0.0, 0, 0, 0);
    EXPECT_TRUE((Monomial(0.0, 1, 0, 0) == res6)); // {0.0, 1, 0, 0}

    // 0 * x = 0
    Monomial res7 = m_zero * m_x;
    EXPECT_TRUE((Monomial(0.0, 0 + 1, 0 + 0, 0 + 0) == res7)); // {0.0, 1, 0, 0}
}


// --- Дополнительные Тесты для класса Polynomial ---

TEST(PolynomialTests, CanonicalizeCombineLikeTerms) {
    // x + 2x + 3x = 6x
    std::vector<Monomial> input1 = { {1.0, 1, 0, 0}, {2.0, 1, 0, 0}, {3.0, 1, 0, 0} };
    Polynomial p1(input1);
    std::vector<Monomial> expected1 = { {6.0, 1, 0, 0} };
    ASSERT_EQ(Polynomial(expected1), p1);

    // x^2y + 5xy^2 - 3x^2y = -2x^2y + 5xy^2
    std::vector<Monomial> input2 = { {1.0, 2, 1, 0}, {5.0, 1, 2, 0}, {-3.0, 2, 1, 0} };
    Polynomial p2(input2);
    // Ожидаемый порядок: x^2y (-2.0), xy^2 (5.0) -> {-2.0, 2,1,0}, {5.0, 1,2,0} - НЕТ! Сортировка px desc, py desc, pz desc
    // (2,1,0) идет перед (1,2,0)
    std::vector<Monomial> expected2 = { {-2.0, 2, 1, 0}, {5.0, 1, 2, 0} };
    ASSERT_EQ(Polynomial(expected2), p2);

    // 5 + 2x - 3 + x = 3x + 2
    std::vector<Monomial> input3 = { {5.0, 0, 0, 0}, {2.0, 1, 0, 0}, {-3.0, 0, 0, 0}, {1.0, 1, 0, 0} };
    Polynomial p3(input3);
    // Ожидаемый порядок: x (3.0), const (2.0) -> {3.0, 1,0,0}, {2.0, 0,0,0}
    std::vector<Monomial> expected3 = { {3.0, 1, 0, 0}, {2.0, 0, 0, 0} };
    ASSERT_EQ(Polynomial(expected3), p3);
}

TEST(PolynomialTests, CanonicalizeRemoveZeroTerms) {
    // x - x = 0
    std::vector<Monomial> input1 = { {1.0, 1, 0, 0}, {-1.0, 1, 0, 0} };
    Polynomial p1(input1);
    ASSERT_EQ(0, p1.getTerms().size()); // Должен быть нулевой полином

    // 2x + y - 2x = y
    std::vector<Monomial> input2 = { {2.0, 1, 0, 0}, {1.0, 0, 1, 0}, {-2.0, 1, 0, 0} };
    Polynomial p2(input2);
    std::vector<Monomial> expected2 = { {1.0, 0, 1, 0} };
    ASSERT_EQ(Polynomial(expected2), p2);

    // 0*x^2 + 5y = 5y (член с нулевым коэффициентом должен удалиться при создании)
    std::vector<Monomial> input3 = { {0.0, 2, 0, 0}, {5.0, 0, 1, 0} };
    Polynomial p3(input3);
    std::vector<Monomial> expected3 = { {5.0, 0, 1, 0} };
    ASSERT_EQ(Polynomial(expected3), p3);

    // Все члены нулевые -> 0
    std::vector<Monomial> input4 = { {0.0, 1,1,1}, {0.0, 0,0,0}, {0.0, 5,0,0} };
    Polynomial p4(input4);
    ASSERT_EQ(0, p4.getTerms().size()); // Должен быть нулевой полином

    // Сумма коэффициентов близка к нулю -> 0
    std::vector<Monomial> input5 = { {1.0, 1, 0, 0}, {-1.0 + 1e-12, 1, 0, 0} };
    Polynomial p5(input5);
    ASSERT_EQ(0, p5.getTerms().size()); // Должен быть нулевой полином из-за EPSILON
}

TEST(PolynomialTests, CanonicalizeSorting) {
    // y + x -> x + y
    std::vector<Monomial> input1 = { {1.0, 0, 1, 0}, {1.0, 1, 0, 0} };
    Polynomial p1(input1);
    // Ожидаемый порядок: px desc (1), px desc (0) -> {1.0, 1,0,0}, {1.0, 0,1,0}
    std::vector<Monomial> expected1 = { {1.0, 1, 0, 0}, {1.0, 0, 1, 0} };
    ASSERT_EQ(Polynomial(expected1), p1);

    // z + y + x -> x + y + z
    std::vector<Monomial> input2 = { {1.0, 0, 0, 1}, {1.0, 0, 1, 0}, {1.0, 1, 0, 0} };
    Polynomial p2(input2);
    // Ожидаемый порядок: {1.0, 1,0,0}, {1.0, 0,1,0}, {1.0, 0,0,1}
    std::vector<Monomial> expected2 = { {1.0, 1, 0, 0}, {1.0, 0, 1, 0}, {1.0, 0, 0, 1} };
    ASSERT_EQ(Polynomial(expected2), p2);

    // x^2y + xy^2 -> x^2y + xy^2 (правильный порядок уже)
    std::vector<Monomial> input3 = { {1.0, 2, 1, 0}, {1.0, 1, 2, 0} };
    Polynomial p3(input3);
    std::vector<Monomial> expected3 = { {1.0, 2, 1, 0}, {1.0, 1, 2, 0} };
    ASSERT_EQ(Polynomial(expected3), p3);

    // xy^2 + x^2y -> x^2y + xy^2 (должно отсортироваться)
    std::vector<Monomial> input4 = { {1.0, 1, 2, 0}, {1.0, 2, 1, 0} };
    Polynomial p4(input4);
    std::vector<Monomial> expected4 = { {1.0, 2, 1, 0}, {1.0, 1, 2, 0} };
    ASSERT_EQ(Polynomial(expected4), p4);
}

TEST(PolynomialTests, PolynomialSubtraction) {
    // P1 = x + y
    std::vector<Monomial> p1_terms = { {1.0, 1, 0, 0}, {1.0, 0, 1, 0} };
    Polynomial p1(p1_terms);

    // P2 = y + z
    std::vector<Monomial> p2_terms = { {1.0, 0, 1, 0}, {1.0, 0, 0, 1} };
    Polynomial p2(p2_terms);

    // P1 - P2 = (x + y) - (y + z) = x - z
    Polynomial p1_minus_p2 = p1 - p2;
    std::vector<Monomial> expected_diff_terms = { {1.0, 1, 0, 0}, {-1.0, 0, 0, 1} }; // Ожидаемый порядок: x, z
    Polynomial expected_diff(expected_diff_terms);
    ASSERT_EQ(expected_diff, p1_minus_p2);

    // P3 = x^2 + 5
    std::vector<Monomial> p3_terms = { {1.0, 2, 0, 0}, {5.0, 0, 0, 0} };
    Polynomial p3(p3_terms);

    // P4 = x^2 - 10
    std::vector<Monomial> p4_terms = { {1.0, 2, 0, 0}, {-10.0, 0, 0, 0} };
    Polynomial p4(p4_terms);

    // P3 - P4 = (x^2 + 5) - (x^2 - 10) = 5 + 10 = 15
    Polynomial p3_minus_p4 = p3 - p4;
    std::vector<Monomial> expected_diff2_terms = { {15.0, 0, 0, 0} };
    Polynomial expected_diff2(expected_diff2_terms);
    ASSERT_EQ(expected_diff2, p3_minus_p4);

}

TEST(PolynomialTests, PolynomialMultiplicationBasic) {
    // P1 = x + y
    std::vector<Monomial> p1_terms = { {1.0, 1, 0, 0}, {1.0, 0, 1, 0} };
    Polynomial p1(p1_terms);

    // P2 = x - y
    std::vector<Monomial> p2_terms = { {1.0, 1, 0, 0}, {-1.0, 0, 1, 0} };
    Polynomial p2(p2_terms);

    // P1 * P2 = (x + y)(x - y) = x^2 - y^2
    Polynomial p1_times_p2 = p1 * p2;
    // Ожидаемые члены: x^2, -y^2 -> {1.0, 2,0,0}, {-1.0, 0,2,0}
    std::vector<Monomial> expected_prod1_terms = { {1.0, 2, 0, 0}, {-1.0, 0, 2, 0} };
    Polynomial expected_prod1(expected_prod1_terms);
    ASSERT_EQ(expected_prod1, p1_times_p2);

    // P3 = x^2 + 3x + 2
    std::vector<Monomial> p3_terms = { {1.0, 2, 0, 0}, {3.0, 1, 0, 0}, {2.0, 0, 0, 0} };
    Polynomial p3(p3_terms);

    // P4 = x - 1
    std::vector<Monomial> p4_terms = { {1.0, 1, 0, 0}, {-1.0, 0, 0, 0} };
    Polynomial p4(p4_terms);

    // P3 * P4 = (x^2 + 3x + 2)(x - 1) = x^3 - x^2 + 3x^2 - 3x + 2x - 2 = x^3 + 2x^2 - x - 2
    Polynomial p3_times_p4 = p3 * p4;
    // Ожидаемые члены: x^3, 2x^2, -x, -2 -> {1.0, 3,0,0}, {2.0, 2,0,0}, {-1.0, 1,0,0}, {-2.0, 0,0,0}
    std::vector<Monomial> expected_prod2_terms = { {1.0, 3, 0, 0}, {2.0, 2, 0, 0}, {-1.0, 1, 0, 0}, {-2.0, 0, 0, 0} };
    Polynomial expected_prod2(expected_prod2_terms);
    ASSERT_EQ(expected_prod2, p3_times_p4);
}

TEST(PolynomialTests, PolynomialMultiplicationPolyConst) {
    // P1 = x + y
    std::vector<Monomial> p1_terms = { {1.0, 1, 0, 0}, {1.0, 0, 1, 0} };
    Polynomial p1(p1_terms);

    // P1 * 5.0 = 5x + 5y
    Polynomial p1_times_const = p1 * 5.0;
    std::vector<Monomial> expected1 = { {5.0, 1, 0, 0}, {5.0, 0, 1, 0} };
    ASSERT_EQ(Polynomial(expected1), p1_times_const);

    // (x^2 + 5) * 0.0 = 0
    std::vector<Monomial> p2_terms = { {1.0, 2, 0, 0}, {5.0, 0, 0, 0} };
    Polynomial p2(p2_terms);
    Polynomial p2_times_zero = p2 * 0.0;
    ASSERT_EQ(Polynomial(), p2_times_zero); // Ожидаем нулевой полином
}


TEST(PolynomialTests, PolynomialMultiplicationZeroPoly) {
    Polynomial zero_poly; // Нулевой полином
    // P1 = x + y
    std::vector<Monomial> p1_terms = { {1.0, 1, 0, 0}, {1.0, 0, 1, 0} };
    Polynomial p1(p1_terms);

    // P1 * 0 = 0
    Polynomial p1_times_zero = p1 * zero_poly;
    ASSERT_EQ(Polynomial(), p1_times_zero);

    // 0 * P1 = 0
    Polynomial zero_times_p1 = zero_poly * p1;
    ASSERT_EQ(Polynomial(), zero_times_p1);

    // 0 * 0 = 0
    Polynomial zero_times_zero = zero_poly * zero_poly;
    ASSERT_EQ(Polynomial(), zero_times_zero);
}


TEST(PolynomialTests, PolynomialMultiplicationDegreeOverflow) {
    // P1 = x^5
    std::vector<Monomial> p1_terms = { {1.0, 5, 0, 0} };
    Polynomial p1(p1_terms);

    // P2 = x^5
    std::vector<Monomial> p2_terms = { {1.0, 5, 0, 0} };
    Polynomial p2(p2_terms);

    // P1 * P2 = x^10 -> Должно выбросить исключение
    ASSERT_THROW(p1 * p2, std::out_of_range);

    // P3 = x^9
    std::vector<Monomial> p3_terms = { {1.0, 9, 0, 0} };
    Polynomial p3(p3_terms);

    // P4 = x
    std::vector<Monomial> p4_terms = { {1.0, 1, 0, 0} };
    Polynomial p4(p4_terms);

    // P3 * P4 = x^10 -> Должно выбросить исключение
    ASSERT_THROW(p3 * p4, std::out_of_range);

    // (x^5 + y^5) * (x^5 + y^5) = x^10 + 2x^5y^5 + y^10 -> Должно выбросить исключение из-за x^10 и y^10
    std::vector<Monomial> p5_terms = { {1.0, 5, 0, 0}, {1.0, 0, 5, 0} };
    Polynomial p5(p5_terms);
    ASSERT_THROW(p5 * p5, std::out_of_range);
}










