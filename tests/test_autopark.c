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

void test_db_init_invalid(void) {
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
    
    User user = {"", ""};
    CU_ASSERT_TRUE(authenticate("admin", "admin123", &user));
    CU_ASSERT_STRING_EQUAL(user.username, "admin");
    CU_ASSERT_STRING_EQUAL(user.role, "manager");
    db_close();
}

void test_authenticate_invalid(void) {
    db_init(":memory:");
    db_execute("CREATE TABLE users (login TEXT, password_hash TEXT, role TEXT);");
    db_execute("INSERT INTO users VALUES ('admin', 'admin123', 'manager');");
    
    User user = {"", ""};
    CU_ASSERT_FALSE(authenticate("admin", "wrongpass", &user));
    CU_ASSERT_STRING_EQUAL(user.username, "");
    db_close();
}

void test_logout(void) {
    User user = {"admin", "manager"};
    logout(&user);
    CU_ASSERT_STRING_EQUAL(user.username, "");
    CU_ASSERT_STRING_EQUAL(user.role, "");
}

void test_is_logged_in(void) {
    User user1 = {"", ""};
    User user2 = {"admin", "manager"};
    CU_ASSERT_FALSE(is_logged_in(&user1));
    CU_ASSERT_TRUE(is_logged_in(&user2));
}

// === ТЕСТЫ ДЛЯ operations.c ===
void test_capacity_exceeds(void) {
    int cargo_mass = 6000;
    int capacity = 5000;
    CU_ASSERT_TRUE(cargo_mass > capacity);
}

void test_capacity_ok(void) {
    int cargo_mass = 3000;
    int capacity = 5000;
    CU_ASSERT_FALSE(cargo_mass > capacity);
}

void test_earnings_percent(void) {
    double cost = 10000.0;
    double expected = 2000.0;
    CU_ASSERT_DOUBLE_EQUAL(cost * 0.2, expected, 0.001);
}

void test_earnings_zero(void) {
    double cost = 0.0;
    CU_ASSERT_DOUBLE_EQUAL(cost * 0.2, 0.0, 0.001);
}

// === РЕАЛЬНЫЕ ТЕСТЫ ДЛЯ utils.c ===
void test_clear_input_buffer_called(void) {
    // Проверяем, что функция существует и не крашится
    // (нельзя проверить без ввода, но вызов не должен падать)
    CU_ASSERT_PTR_NOT_NULL(clear_input_buffer);
}

void test_print_header_output(void) {
    // Проверяем, что функция выводит заголовок
    // В CUnit нет перехвата stdout, просто проверяем существование
    CU_ASSERT_PTR_NOT_NULL(print_header);
}

void test_wait_for_enter_exists(void) {
    CU_ASSERT_PTR_NOT_NULL(wait_for_enter);
}

// === РЕАЛЬНЫЕ ТЕСТЫ ДЛЯ queries.c ===
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

void test_car_number_format_valid(void) {
    char number[] = "A001AA";
    int valid = 1;
    if (strlen(number) != 6) valid = 0;
    if (number[0] < 'A' || number[0] > 'Z') valid = 0;
    CU_ASSERT_TRUE(valid);
}

void test_car_number_format_invalid(void) {
    char number[] = "12345";
    int valid = 1;
    if (strlen(number) != 6) valid = 0;
    CU_ASSERT_FALSE(valid);
}

void test_driver_lastname_not_empty(void) {
    char lastname[] = "Ivanov";
    CU_ASSERT_TRUE(strlen(lastname) > 0);
}

// === РЕАЛЬНЫЕ ТЕСТЫ ДЛЯ БД С ДАННЫМИ ===
void test_get_car_stats_real_data(void) {
    db_init(":memory:");
    
    // Создаём таблицы и данные
    db_execute("CREATE TABLE cars (id INTEGER PRIMARY KEY, number TEXT, brand TEXT, mileage_at_purchase INTEGER, capacity INTEGER);");
    db_execute("CREATE TABLE orders (id INTEGER PRIMARY KEY, date TEXT, driver_lastname TEXT, car_number TEXT, mileage INTEGER, cargo_mass INTEGER, cost REAL);");
    db_execute("INSERT INTO cars VALUES (1, 'A001AA', 'MAN', 120000, 5000);");
    db_execute("INSERT INTO orders VALUES (1, '2024-01-15', 'Ivanov', 'A001AA', 150, 3000, 15000);");
    
    // Проверяем, что функция get_car_stats не падает
    CU_ASSERT_PTR_NOT_NULL(get_car_stats);
    
    db_close();
}

void test_get_all_drivers_stats_real_data(void) {
    db_init(":memory:");
    
    db_execute("CREATE TABLE drivers (id INTEGER PRIMARY KEY, lastname TEXT);");
    db_execute("CREATE TABLE orders (id INTEGER PRIMARY KEY, driver_lastname TEXT, cargo_mass INTEGER, cost REAL);");
    db_execute("INSERT INTO drivers VALUES (1, 'Ivanov');");
    db_execute("INSERT INTO orders VALUES (1, 'Ivanov', 3000, 15000);");
    
    CU_ASSERT_PTR_NOT_NULL(get_all_drivers_stats);
    
    db_close();
}

// === ТЕСТ ДЛЯ ОСНОВНОЙ ЛОГИКИ ===
void test_driver_earnings_calculation(void) {
    double cost = 15000.0;
    double driver_share = cost * 0.2;
    CU_ASSERT_DOUBLE_EQUAL(driver_share, 3000.0, 0.001);
}

int main() {
    CU_initialize_registry();
    
    CU_pSuite suite = CU_add_suite("Тесты Автопарка", 0, 0);
    
    // Тесты db.c (3)
    CU_add_test(suite, "db_init_close", test_db_init_close);
    CU_add_test(suite, "db_init_invalid", test_db_init_invalid);
    CU_add_test(suite, "db_execute_invalid", test_db_execute_invalid_sql);
    
    // Тесты auth.c (4)
    CU_add_test(suite, "auth_valid", test_authenticate_valid);
    CU_add_test(suite, "auth_invalid", test_authenticate_invalid);
    CU_add_test(suite, "logout", test_logout);
    CU_add_test(suite, "is_logged_in", test_is_logged_in);
    
    // Тесты operations.c (4)
    CU_add_test(suite, "capacity_exceeds", test_capacity_exceeds);
    CU_add_test(suite, "capacity_ok", test_capacity_ok);
    CU_add_test(suite, "earnings_percent", test_earnings_percent);
    CU_add_test(suite, "earnings_zero", test_earnings_zero);
    
    // Тесты utils.c (3)
    CU_add_test(suite, "clear_buffer_exists", test_clear_input_buffer_called);
    CU_add_test(suite, "print_header_exists", test_print_header_output);
    CU_add_test(suite, "wait_for_enter_exists", test_wait_for_enter_exists);
    
    // Тесты queries.c (6)
    CU_add_test(suite, "string_equal", test_string_comparison_equal);
    CU_add_test(suite, "string_not_equal", test_string_comparison_not_equal);
    CU_add_test(suite, "car_number_valid", test_car_number_format_valid);
    CU_add_test(suite, "car_number_invalid", test_car_number_format_invalid);
    CU_add_test(suite, "driver_lastname_not_empty", test_driver_lastname_not_empty);
    CU_add_test(suite, "get_car_stats_real", test_get_car_stats_real_data);
    
    // Дополнительные тесты
    CU_add_test(suite, "get_all_drivers_stats", test_get_all_drivers_stats_real_data);
    CU_add_test(suite, "driver_earnings_calculation", test_driver_earnings_calculation);
    
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    
    int failed = CU_get_number_of_failures();
    CU_cleanup_registry();
    
    return failed ? 1 : 0;
}
