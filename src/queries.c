#include "queries.h"
#include "db.h"
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

static int print_row_callback(void* NotUsed, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++) {
        printf("%-15s ", argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

void get_orders_by_driver_period(const char* lastname, const char* start_date, const char* end_date, const char* current_username, const char* current_role) {
    if (strcmp(current_role, "driver") == 0 && strcmp(current_username, lastname) != 0) {
        printf("Доступ запрещён. Вы можете просматривать только свои заказы.\n");
        return;
    }

    char sql[512];
    snprintf(sql, sizeof(sql),
        "SELECT date, car_number, mileage, cargo_mass, cost FROM orders "
        "WHERE driver_lastname = '%s' AND date >= '%s' AND date <= '%s';",
        lastname, start_date, end_date);

    printf("\n=== Заказы водителя %s за период %s - %s ===\n", lastname, start_date, end_date);
    printf("%-12s %-12s %-10s %-10s %-10s\n", "Дата", "Машина", "Км", "Кг", "Цена");
    sqlite3_exec(db, sql, print_row_callback, NULL, NULL);
}

void get_car_stats(const char* car_number) {
    char sql[256];
    snprintf(sql, sizeof(sql),
        "SELECT SUM(mileage) as total_mileage, SUM(cargo_mass) as total_cargo FROM orders WHERE car_number = '%s';",
        car_number);

    printf("\n=== Статистика по автомобилю %s ===\n", car_number);
    sqlite3_exec(db, sql, print_row_callback, NULL, NULL);
}

void get_all_drivers_stats(void) {
    const char* sql =
        "SELECT d.lastname, "
        "COUNT(o.id) as trips, "
        "SUM(o.cargo_mass) as total_mass, "
        "SUM(o.cost * 0.2) as total_earned "
        "FROM drivers d "
        "LEFT JOIN orders o ON d.lastname = o.driver_lastname "
        "GROUP BY d.lastname;";

    printf("\n=== Статистика по всем водителям ===\n");
    printf("%-20s %-10s %-15s %-12s\n", "Фамилия", "Поездок", "Груза (кг)", "Заработано");
    sqlite3_exec(db, sql, print_row_callback, NULL, NULL);
}

void get_driver_with_fewest_trips(void) {
    const char* sql =
        "SELECT d.lastname, d.tab_number, d.category, d.experience, d.address, d.birth_year, "
        "COUNT(o.id) as trips, SUM(o.cost * 0.2) as total_earned "
        "FROM drivers d "
        "LEFT JOIN orders o ON d.lastname = o.driver_lastname "
        "GROUP BY d.lastname "
        "ORDER BY trips ASC LIMIT 1;";

    printf("\n=== Водитель с наименьшим количеством поездок ===\n");
    sqlite3_exec(db, sql, print_row_callback, NULL, NULL);
}

void get_car_with_max_mileage(void) {
    const char* sql =
        "SELECT c.number, c.brand, c.mileage_at_purchase, c.capacity, "
        "SUM(o.mileage) as total_mileage "
        "FROM cars c "
        "LEFT JOIN orders o ON c.number = o.car_number "
        "GROUP BY c.number "
        "ORDER BY total_mileage DESC LIMIT 1;";

    printf("\n=== Автомобиль с наибольшим пробегом ===\n");
    sqlite3_exec(db, sql, print_row_callback, NULL, NULL);
}