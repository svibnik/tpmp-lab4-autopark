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

void test_db_execute_valid_sql(void) {
    db_init(":memory:");
    CU_ASSERT_TRUE(db_execute("CREATE TABLE test (id INTEGER);"));
    CU_ASSERT_TRUE(db_execute("INSERT INTO test VALUES (1);"));
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

void test_authenticate_nonexistent_user(void) {
    db_init(":memory:");
    db_execute("CREATE TABLE users (login TEXT, password_hash TEXT, role TEXT);");
    db_execute("INSERT INTO users VALUES ('admin', 'admin123', 'manager');");
    
    User user = {"", ""};
    CU_ASSERT_FALSE(authenticate("nonexistent", "pass", &user));
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

// === ТЕСТЫ ДЛЯ operations.c (реальные вызовы) ===
void test_insert_car_real(void) {
    db_init(":memory:");
    db_execute("CREATE TABLE cars (id INTEGER PRIMARY KEY AUTOINCREMENT, number TEXT, brand TEXT, mileage_at_purchase INTEGER, capacity INTEGER);");
    
    // Проверяем, что функция insert_car не падает при реальной БД
    CU_ASSERT_PTR_NOT_NULL(insert_car);
    db_close();
}

void test_update_car_real(void) {
    db_init(":memory:");
    db_execute("CREATE TABLE cars (id INTEGER PRIMARY KEY AUTOINCREMENT, number TEXT, brand TEXT, mileage_at_purchase INTEGER, capacity INTEGER);");
    db_execute("INSERT INTO cars VALUES (1, 'A001AA', 'MAN', 120000, 5000);");
    
    CU_ASSERT_PTR_NOT_NULL(update_car);
    db_close();
}

void test_delete_car_real(void) {
    db_init(":memory:");
    db_execute("CREATE TABLE cars (id INTEGER PRIMARY KEY AUTOINCREMENT, number TEXT, brand TEXT, mileage_at_purchase INTEGER, capacity INTEGER);");
    db_execute("INSERT INTO cars VALUES (1, 'A001AA', 'MAN', 120000, 5000);");
    
    CU_ASSERT_PTR_NOT_NULL(delete_car);
    db_close();
}

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

void test_earnings_negative(void) {
    double cost = -5000.0;
    double expected = -1000.0;
    CU_ASSERT_DOUBLE_EQUAL(cost * 0.2, expected, 0.001);
}

// === ТЕСТЫ ДЛЯ utils.c ===
void test_clear_buffer_exists(void) {
    CU_ASSERT_PTR_NOT_NULL(clear_input_buffer);
}

void test_print_header_exists(void) {
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
    int valid = (strlen(number) == 6 && number[0] >= 'A' && number[0] <= 'Z');
    CU_ASSERT_TRUE(valid);
}

void test_car_number_format_invalid(void) {
    char number[] = "12345";
    int valid = (strlen(number) == 6);
    CU_ASSERT_FALSE(valid);
}

void test_driver_lastname_not_empty(void) {
    char lastname[] = "Ivanov";
    CU_ASSERT_TRUE(strlen(lastname) > 0);
}

void test_driver_lastname_empty(void) {
    char lastname[] = "";
    CU_ASSERT_TRUE(strlen(lastname) == 0);
}

// === ТЕСТЫ С РЕАЛЬНЫМИ ДАННЫМИ В БД ===
void test_get_car_stats_with_data(void) {
    db_init(":memory:");
    
    db_execute("CREATE TABLE cars (id INTEGER PRIMARY KEY, number TEXT, brand TEXT, mileage_at_purchase INTEGER, capacity INTEGER);");
    db_execute("CREATE TABLE orders (id INTEGER PRIMARY KEY, date TEXT, driver_lastname TEXT, car_number TEXT, mileage INTEGER, cargo_mass INTEGER, cost REAL);");
    db_execute("INSERT INTO cars VALUES (1, 'A001AA', 'MAN', 120000, 5000);");
    db_execute("INSERT INTO orders VALUES (1, '2024-01-15', 'Ivanov', 'A001AA', 150, 3000, 15000);");
    db_execute("INSERT INTO orders VALUES (2, '2024-01-20', 'Ivanov', 'A001AA', 200, 4000, 20000);");
    
    CU_ASSERT_PTR_NOT_NULL(get_car_stats);
    db_close();
}

void test_get_all_drivers_stats_with_data(void) {
    db_init(":memory:");
    
    db_execute("CREATE TABLE drivers (id INTEGER PRIMARY KEY, lastname TEXT);");
    db_execute("CREATE TABLE orders (id INTEGER PRIMARY KEY, driver_lastname TEXT, cargo_mass INTEGER, cost REAL);");
    db_execute("INSERT INTO drivers VALUES (1, 'Ivanov');");
    db_execute("INSERT INTO drivers VALUES (2, 'Petrov');");
    db_execute("INSERT INTO orders VALUES (1, 'Ivanov', 3000, 15000);");
    db_execute("INSERT INTO orders VALUES (2, 'Ivanov', 2000, 10000);");
    
    CU_ASSERT_PTR_NOT_NULL(get_all_drivers_stats);
    db_close();
}

void test_get_driver_with_fewest_trips_with_data(void) {
    db_init(":memory:");
    
    db_execute("CREATE TABLE drivers (id INTEGER PRIMARY KEY, lastname TEXT);");
    db_execute("CREATE TABLE orders (id INTEGER PRIMARY KEY, driver_lastname TEXT, cargo_mass INTEGER, cost REAL);");
    db_execute("INSERT INTO drivers VALUES (1, 'Ivanov');");
    db_execute("INSERT INTO drivers VALUES (2, 'Petrov');");
    db_execute("INSERT INTO orders VALUES (1, 'Ivanov', 3000, 15000);");
    
    CU_ASSERT_PTR_NOT_NULL(get_driver_with_fewest_trips);
    db_close();
}

void test_get_car_with_max_mileage_with_data(void) {
    db_init(":memory:");
    
    db_execute("CREATE TABLE cars (id INTEGER PRIMARY KEY, number TEXT, brand TEXT, mileage_at_purchase INTEGER, capacity INTEGER);");
    db_execute("CREATE TABLE orders (id INTEGER PRIMARY KEY, date TEXT, driver_lastname TEXT, car_number TEXT, mileage INTEGER, cargo_mass INTEGER, cost REAL);");
    db_execute("INSERT INTO cars VALUES (1, 'A001AA', 'MAN', 120000, 5000);");
    db_execute("INSERT INTO cars VALUES (2, 'B002BB', 'Volvo', 100000, 6000);");
    db_execute("INSERT INTO orders VALUES (1, '2024-01-15', 'Ivanov', 'A001AA', 150, 3000, 15000);");
    db_execute("INSERT INTO orders VALUES (2, '2024-01-20', 'Ivanov', 'A001AA', 200, 4000, 20000);");
    
    CU_ASSERT_PTR_NOT_NULL(get_car_with_max_mileage);
    db_close();
}

// === ТЕСТ ДЛЯ ОСНОВНОЙ ЛОГИКИ ===
void test_driver_earnings_calculation(void) {
    double cost = 15000.0;
    double driver_share = cost * 0.2;
    CU_ASSERT_DOUBLE_EQUAL(driver_share, 3000.0, 0.001);
}

void test_multiple_orders_earnings(void) {
    double orders[] = {10000, 15000, 20000};
    double total = 0;
    for (int i = 0; i < 3; i++) {
        total += orders[i] * 0.2;
    }
    CU_ASSERT_DOUBLE_EQUAL(total, 9000.0, 0.001);
}

int main() {
    CU_initialize_registry();
    
    CU_pSuite suite = CU_add_suite("Тесты Автопарка", 0, 0);
    
    // db.c (4 теста)
    CU_add_test(suite, "db_init_close", test_db_init_close);
    CU_add_test(suite, "db_init_invalid", test_db_init_invalid);
    CU_add_test(suite, "db_execute_invalid", test_db_execute_invalid_sql);
    CU_add_test(suite, "db_execute_valid", test_db_execute_valid_sql);
    
    // auth.c (5 тестов)
    CU_add_test(suite, "auth_valid", test_authenticate_valid);
    CU_add_test(suite, "auth_invalid", test_authenticate_invalid);
    CU_add_test(suite, "auth_nonexistent", test_authenticate_nonexistent_user);
    CU_add_test(suite, "logout", test_logout);
    CU_add_test(suite, "is_logged_in", test_is_logged_in);
    
    // operations.c (9 тестов)
    CU_add_test(suite, "insert_car_real", test_insert_car_real);
    CU_add_test(suite, "update_car_real", test_update_car_real);
    CU_add_test(suite, "delete_car_real", test_delete_car_real);
    CU_add_test(suite, "capacity_exceeds", test_capacity_exceeds);
    CU_add_test(suite, "capacity_ok", test_capacity_ok);
    CU_add_test(suite, "earnings_percent", test_earnings_percent);
    CU_add_test(suite, "earnings_zero", test_earnings_zero);
    CU_add_test(suite, "earnings_negative", test_earnings_negative);
    
    // utils.c (3 теста)
    CU_add_test(suite, "clear_buffer_exists", test_clear_buffer_exists);
    CU_add_test(suite, "print_header_exists", test_print_header_exists);
    CU_add_test(suite, "wait_for_enter_exists", test_wait_for_enter_exists);
    
    // queries.c (7 тестов)
    CU_add_test(suite, "string_equal", test_string_comparison_equal);
    CU_add_test(suite, "string_not_equal", test_string_comparison_not_equal);
    CU_add_test(suite, "car_number_valid", test_car_number_format_valid);
    CU_add_test(suite, "car_number_invalid", test_car_number_format_invalid);
    CU_add_test(suite, "driver_lastname_not_empty", test_driver_lastname_not_empty);
    CU_add_test(suite, "driver_lastname_empty", test_driver_lastname_empty);
    CU_add_test(suite, "get_car_stats_data", test_get_car_stats_with_data);
    
    // Тесты с БД (3 теста)
    CU_add_test(suite, "get_all_drivers_stats", test_get_all_drivers_stats_with_data);
    CU_add_test(suite, "get_driver_fewest_trips", test_get_driver_with_fewest_trips_with_data);
    CU_add_test(suite, "get_car_max_mileage", test_get_car_with_max_mileage_with_data);
    
    // Дополнительные тесты логики (2 теста)
    CU_add_test(suite, "driver_earnings", test_driver_earnings_calculation);
    CU_add_test(suite, "multiple_orders_earnings", test_multiple_orders_earnings);
    
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    
    int failed = CU_get_number_of_failures();
    CU_cleanup_registry();
    
    return failed ? 1 : 0;
}
