#include "unity.h"
#include "calc.c"
#include <limits.h>

void setUp(void) {}
void tearDown(void) {}

/* ---------- integer tests ---------- */

void test_add(void) {
    TEST_ASSERT_EQUAL(4, add(2, 2));
    TEST_ASSERT_EQUAL(0, add(-2, 2));
    TEST_ASSERT_EQUAL(-5, add(-2, -3));
    TEST_ASSERT_EQUAL(INT_MAX, add(INT_MAX, 1));
    TEST_ASSERT_EQUAL(INT_MIN, add(INT_MIN, -1));
    TEST_ASSERT_EQUAL(-1, add(INT_MAX, INT_MIN));
}

void test_sub(void) {
    TEST_ASSERT_EQUAL(2, sub(4, 2));
    TEST_ASSERT_EQUAL(-2, sub(2, 4));
    TEST_ASSERT_EQUAL(INT_MIN, sub(INT_MIN, 1));
    TEST_ASSERT_EQUAL(INT_MAX, sub(INT_MAX, -1));
    TEST_ASSERT_EQUAL(INT_MAX, sub(0, INT_MIN));
}

void test_mul(void) {
    TEST_ASSERT_EQUAL(12, mul(3, 4));
    TEST_ASSERT_EQUAL(-12, mul(-3, 4));
    TEST_ASSERT_EQUAL(12, mul(-3, -4));
    TEST_ASSERT_EQUAL(0, mul(0, INT_MAX));
    TEST_ASSERT_EQUAL(2147395600, mul(46340, 46340));
    TEST_ASSERT_EQUAL(INT_MAX, mul(46341, 46341));
    TEST_ASSERT_EQUAL(INT_MAX, mul(INT_MAX, 2));
    TEST_ASSERT_EQUAL(INT_MIN, mul(INT_MIN, 2));
    TEST_ASSERT_EQUAL(INT_MIN, mul(INT_MAX, -2));
    TEST_ASSERT_EQUAL(INT_MAX, mul(INT_MIN, -1));
}

void test_divide(void) {
    TEST_ASSERT_EQUAL(5, divide(10, 2));
    TEST_ASSERT_EQUAL(3, divide(7, 2));      /* truncates */
    TEST_ASSERT_EQUAL(-3, divide(-7, 2));    /* toward zero */
    TEST_ASSERT_EQUAL(0, divide(0, 5));
    TEST_ASSERT_EQUAL(0, divide(5, 0));      /* design decision */
    TEST_ASSERT_EQUAL(INT_MAX, divide(INT_MIN, -1));
}

void test_power(void) {
    TEST_ASSERT_EQUAL(1024, power(2, 10));
    TEST_ASSERT_EQUAL(1, power(5, 0));
    TEST_ASSERT_EQUAL(1, power(0, 0));
    TEST_ASSERT_EQUAL(0, power(0, 5));
    TEST_ASSERT_EQUAL(-8, power(-2, 3));
    TEST_ASSERT_EQUAL(16, power(-2, 4));
    TEST_ASSERT_EQUAL(1000000000, power(10, 9));
    TEST_ASSERT_EQUAL(INT_MAX, power(10, 10));
    TEST_ASSERT_EQUAL(INT_MAX, power(2, 31));
    TEST_ASSERT_EQUAL(INT_MIN, power(-2, 31));
    TEST_ASSERT_EQUAL(INT_MAX, power(2, 100));
    TEST_ASSERT_EQUAL(INT_MIN, power(-2, 101));
    TEST_ASSERT_EQUAL(0, power(2, -1));
    TEST_ASSERT_EQUAL(1, power(1, -5));
    TEST_ASSERT_EQUAL(-1, power(-1, -3));
    TEST_ASSERT_EQUAL(1, power(1, INT_MAX));
}

/* ---------- float tests ---------- */

void test_float_arithmetic(void) {
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.3f, addf(0.1, 0.2));
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 3.25f, subf(5.5, 2.25));
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 6.0f, mulf(1.5, 4.0));
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, -6.0f, mulf(-1.5, 4.0));
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.25f, dividef(1.0, 4.0));
}

void test_float_divide_by_zero(void) {
    TEST_ASSERT_FLOAT_IS_NAN(dividef(1.0, 0.0));
    TEST_ASSERT_FLOAT_IS_NAN(dividef(0.0, 0.0));
}

void test_float_power(void) {
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 8.0f, powerf(2.0, 3.0));
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 3.0f, powerf(9.0, 0.5));
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.5f, powerf(2.0, -1.0));
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 1.0f, powerf(7.5, 0.0));
}

/* ---------- calculate() tests ---------- */

void test_calculate_integers(void) {
    TEST_ASSERT_EQUAL_STRING("4", calculate("2 * 2"));
    TEST_ASSERT_EQUAL_STRING("5", calculate("2 + 3"));
    TEST_ASSERT_EQUAL_STRING("-1", calculate("2 - 3"));
    TEST_ASSERT_EQUAL_STRING("3", calculate("7 / 2"));
    TEST_ASSERT_EQUAL_STRING("1024", calculate("2 ^ 10"));
}

void test_calculate_floats(void) {
    TEST_ASSERT_EQUAL_STRING("13.3", calculate("4.2 + 9.1"));
    TEST_ASSERT_EQUAL_STRING("4.0", calculate("2.0 * 2"));
    TEST_ASSERT_EQUAL_STRING("3.5", calculate("7.0 / 2"));
    TEST_ASSERT_EQUAL_STRING("0.3", calculate("0.1 + 0.2"));
    TEST_ASSERT_EQUAL_STRING("0.5", calculate("2 ^ -1"));
}

void test_calculate_whitespace_and_signs(void) {
    TEST_ASSERT_EQUAL_STRING("6", calculate("2*3"));
    TEST_ASSERT_EQUAL_STRING("6", calculate("   2   *   3   "));
    TEST_ASSERT_EQUAL_STRING("-2", calculate("-5 + 3"));
    TEST_ASSERT_EQUAL_STRING("8", calculate("5 - -3"));
    TEST_ASSERT_EQUAL_STRING("2", calculate("5-3"));
}

void test_calculate_overflow(void) {
    TEST_ASSERT_EQUAL_STRING("2147483647", calculate("2147483647 + 1"));
    TEST_ASSERT_EQUAL_STRING("error", calculate("99999999999 + 1"));
}

void test_calculate_errors(void) {
    TEST_ASSERT_EQUAL_STRING("error", calculate(NULL));
    TEST_ASSERT_EQUAL_STRING("error", calculate(""));
    TEST_ASSERT_EQUAL_STRING("error", calculate("   "));
    TEST_ASSERT_EQUAL_STRING("error", calculate("abc"));
    TEST_ASSERT_EQUAL_STRING("error", calculate("2 +"));
    TEST_ASSERT_EQUAL_STRING("error", calculate("+ 2"));
    TEST_ASSERT_EQUAL_STRING("error", calculate("2 2"));
    TEST_ASSERT_EQUAL_STRING("error", calculate("2 & 3"));
    TEST_ASSERT_EQUAL_STRING("error", calculate("2 + 3 abc"));
    TEST_ASSERT_EQUAL_STRING("error", calculate("1 + 2 + 3"));
    TEST_ASSERT_EQUAL_STRING("error", calculate("1.5.2 + 1"));
    TEST_ASSERT_EQUAL_STRING("error", calculate("5 / 0"));
    TEST_ASSERT_EQUAL_STRING("error", calculate("5.0 / 0"));
    TEST_ASSERT_EQUAL_STRING("error", calculate("0 ^ -1"));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_add);
    RUN_TEST(test_sub);
    RUN_TEST(test_mul);
    RUN_TEST(test_divide);
    RUN_TEST(test_power);
    RUN_TEST(test_float_arithmetic);
    RUN_TEST(test_float_divide_by_zero);
    RUN_TEST(test_float_power);
    RUN_TEST(test_calculate_integers);
    RUN_TEST(test_calculate_floats);
    RUN_TEST(test_calculate_whitespace_and_signs);
    RUN_TEST(test_calculate_overflow);
    RUN_TEST(test_calculate_errors);
    return UNITY_END();
}
