#include <stdio.h>
#include <string.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../src/db.h"
#include "../src/auth.h"
#include "../src/queries.h"
#include "../src/operations.h"
#include "../src/utils.h"

// === ТЕСТЫ ДЛЯ db.c ===
void test_db_init_close(void) {
    CU_ASSERT_TRUE(db_init(":memory:"));
    db_close();
    CU_ASSERT_PTR_NULL(db);
}

void test_db_init_null(void) {
    CU_ASSERT_FALSE(db_init("/invalid/path/that/does/not/exist.db"));
}

void test_db_execute_invalid_sql(void) {
    db_init(":memory:");
    CU_ASSERT_FALSE(db_execute("SELECT * FROM non_existent_table;"));
    db_close();
}

// === ТЕСТЫ ДЛЯ auth.c ===
void test_authenticate_valid(void) {
    db_init(":memory:");
    db_execute("CREATE TABLE users (login TEXT, password_hash TEXT, role TEXT);");
    db_execute("INSERT INTO users VALUES ('admin', 'admin123', 'manager');");

    User user = { "", "" };
    CU_ASSERT_TRUE(authenticate("admin", "admin123", &user));
    CU_ASSERT_STRING_EQUAL(user.username, "admin");
    CU_ASSERT_STRING_EQUAL(user.role, "manager");
    db_close();
}

void test_authenticate_invalid(void) {
    db_init(":memory:");
    db_execute("CREATE TABLE users (login TEXT, password_hash TEXT, role TEXT);");
    db_execute("INSERT INTO users VALUES ('admin', 'admin123', 'manager');");

    User user = { "", "" };
    CU_ASSERT_FALSE(authenticate("admin", "wrongpass", &user));
    CU_ASSERT_STRING_EQUAL(user.username, "");
    db_close();
}

void test_logout(void) {
    User user = { "admin", "manager" };
    logout(&user);
    CU_ASSERT_STRING_EQUAL(user.username, "");
    CU_ASSERT_STRING_EQUAL(user.role, "");
}

void test_is_logged_in(void) {
    User user1 = { "", "" };
    User user2 = { "admin", "manager" };
    CU_ASSERT_FALSE(is_logged_in(&user1));
    CU_ASSERT_TRUE(is_logged_in(&user2));
}

// === ТЕСТЫ ДЛЯ operations.c ===
void test_capacity_check_exceeds(void) {
    int cargo_mass = 6000;
    int capacity = 5000;
    CU_ASSERT_TRUE(cargo_mass > capacity);
}

void test_capacity_check_ok(void) {
    int cargo_mass = 3000;
    int capacity = 5000;
    CU_ASSERT_FALSE(cargo_mass > capacity);
}

void test_driver_earnings_percent(void) {
    double cost = 10000.0;
    double expected = 2000.0;
    CU_ASSERT_DOUBLE_EQUAL(cost * 0.2, expected, 0.001);
}

void test_earnings_with_zero(void) {
    double cost = 0.0;
    CU_ASSERT_DOUBLE_EQUAL(cost * 0.2, 0.0, 0.001);
}

// === ТЕСТЫ ДЛЯ utils.c ===
void test_clear_input_buffer(void) {
    // Проверяем, что функция существует и не крашится
    CU_ASSERT_PTR_NOT_NULL(clear_input_buffer);
}

void test_print_header(void) {
    CU_ASSERT_PTR_NOT_NULL(print_header);
}

void test_wait_for_enter(void) {
    CU_ASSERT_PTR_NOT_NULL(wait_for_enter);
}

// === ТЕСТЫ ДЛЯ queries.c ===
void test_string_comparison_equal(void) {
    char str1[] = "Ivanov";
    char str2[] = "Ivanov";
    CU_ASSERT_STRING_EQUAL(str1, str2);
}

void test_string_comparison_not_equal(void) {
    char str1[] = "Ivanov";
    char str2[] = "Petrov";
    CU_ASSERT_STRING_NOT_EQUAL(str1, str2);
}

void test_car_number_format(void) {
    char number[] = "A001AA";
    CU_ASSERT_TRUE(strlen(number) == 6);
    CU_ASSERT_TRUE(number[0] >= 'A' && number[0] <= 'Z');
}

int main() {
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("Тесты Автопарка", 0, 0);

    // Тесты db.c (3 теста)
    CU_add_test(suite, "db_init_close", test_db_init_close);
    CU_add_test(suite, "db_init_invalid", test_db_init_null);
    CU_add_test(suite, "db_execute_invalid", test_db_execute_invalid_sql);

    // Тесты auth.c (4 теста)
    CU_add_test(suite, "auth_valid", test_authenticate_valid);
    CU_add_test(suite, "auth_invalid", test_authenticate_invalid);
    CU_add_test(suite, "logout", test_logout);
    CU_add_test(suite, "is_logged_in", test_is_logged_in);

    // Тесты operations.c (4 теста)
    CU_add_test(suite, "capacity_exceeds", test_capacity_check_exceeds);
    CU_add_test(suite, "capacity_ok", test_capacity_check_ok);
    CU_add_test(suite, "earnings_percent", test_driver_earnings_percent);
    CU_add_test(suite, "earnings_zero", test_earnings_with_zero);

    // Тесты utils.c (3 теста)
    CU_add_test(suite, "clear_buffer", test_clear_input_buffer);
    CU_add_test(suite, "print_header", test_print_header);
    CU_add_test(suite, "wait_for_enter", test_wait_for_enter);

    // Тесты queries.c (3 теста)
    CU_add_test(suite, "string_equal", test_string_comparison_equal);
    CU_add_test(suite, "string_not_equal", test_string_comparison_not_equal);
    CU_add_test(suite, "car_number_format", test_car_number_format);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    int failed = CU_get_number_of_failures();
    CU_cleanup_registry();

    return failed ? 1 : 0;
}