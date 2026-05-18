#include "operations.h"
#include "db.h"
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

int insert_car(void) {
    char number[100], brand[100];
    int mileage, capacity;

    printf("Введите номер автомобиля: ");
    scanf("%s", number);
    printf("Введите марку: ");
    scanf("%s", brand);
    printf("Введите пробег при покупке (км): ");
    scanf("%d", &mileage);
    printf("Введите грузоподъёмность (кг): ");
    scanf("%d", &capacity);

    char sql[512];
    snprintf(sql, sizeof(sql),
        "INSERT INTO cars (number, brand, mileage_at_purchase, capacity) "
        "VALUES ('%s', '%s', %d, %d);",
        number, brand, mileage, capacity);

    if (db_execute(sql)) {
        printf("Автомобиль добавлен.\n");
        return 1;
    }
    printf("Ошибка при добавлении автомобиля.\n");
    return 0;
}

int update_car(void) {
    char number[100], brand[100];
    int mileage, capacity;

    printf("Введите номер автомобиля для обновления: ");
    scanf("%s", number);
    printf("Введите новую марку: ");
    scanf("%s", brand);
    printf("Введите новый пробег: ");
    scanf("%d", &mileage);
    printf("Введите новую грузоподъёмность: ");
    scanf("%d", &capacity);

    char sql[512];
    snprintf(sql, sizeof(sql),
        "UPDATE cars SET brand = '%s', mileage_at_purchase = %d, capacity = %d "
        "WHERE number = '%s';",
        brand, mileage, capacity, number);

    if (db_execute(sql)) {
        printf("Автомобиль обновлён.\n");
        return 1;
    }
    printf("Ошибка при обновлении автомобиля.\n");
    return 0;
}

int delete_car(void) {
    char number[100];
    printf("Введите номер автомобиля для удаления: ");
    scanf("%s", number);

    char sql[256];
    snprintf(sql, sizeof(sql), "DELETE FROM cars WHERE number = '%s';", number);

    if (db_execute(sql)) {
        printf("Автомобиль удалён.\n");
        return 1;
    }
    printf("Ошибка при удалении автомобиля.\n");
    return 0;
}

int insert_driver(void) {
    char tab_number[100], lastname[100], category[100], address[100];
    int experience, birth_year;

    printf("Введите табельный номер: ");
    scanf("%s", tab_number);
    printf("Введите фамилию водителя: ");
    scanf("%s", lastname);
    printf("Введите категорию (A/B/C/D): ");
    scanf("%s", category);
    printf("Введите стаж (лет): ");
    scanf("%d", &experience);
    printf("Введите адрес: ");
    scanf("%s", address);
    printf("Введите год рождения: ");
    scanf("%d", &birth_year);

    char sql[768];
    snprintf(sql, sizeof(sql),
        "INSERT INTO drivers (tab_number, lastname, category, experience, address, birth_year) "
        "VALUES ('%s', '%s', '%s', %d, '%s', %d);",
        tab_number, lastname, category, experience, address, birth_year);

    if (db_execute(sql)) {
        printf("Водитель добавлен.\n");
        return 1;
    }
    printf("Ошибка при добавлении водителя.\n");
    return 0;
}

int insert_order(void) {
    char date[20], driver_lastname[100], car_number[100];
    int mileage, cargo_mass;
    double cost;

    printf("Введите дату (ГГГГ-ММ-ДД): ");
    scanf("%s", date);
    printf("Введите фамилию водителя: ");
    scanf("%s", driver_lastname);
    printf("Введите номер автомобиля: ");
    scanf("%s", car_number);
    printf("Введите километраж: ");
    scanf("%d", &mileage);
    printf("Введите массу груза (кг): ");
    scanf("%d", &cargo_mass);
    printf("Введите стоимость перевозки: ");
    scanf("%lf", &cost);

    // Проверка грузоподъёмности (пункт 4)
    char check_sql[256];
    snprintf(check_sql, sizeof(check_sql),
        "SELECT capacity FROM cars WHERE number = '%s';", car_number);

    int capacity = 0;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, check_sql, -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            capacity = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }

    if (cargo_mass > capacity && capacity > 0) {
        printf("ОШИБКА: Масса груза (%d) превышает грузоподъёмность (%d)!\n", cargo_mass, capacity);
        printf("Заказ НЕ добавлен.\n");
        return 0;
    }

    char sql[768];
    snprintf(sql, sizeof(sql),
        "INSERT INTO orders (date, driver_lastname, car_number, mileage, cargo_mass, cost) "
        "VALUES ('%s', '%s', '%s', %d, %d, %.2f);",
        date, driver_lastname, car_number, mileage, cargo_mass, cost);

    if (db_execute(sql)) {
        printf("Заказ добавлен.\n");
        return 1;
    }
    printf("Ошибка при добавлении заказа.\n");
    return 0;
}