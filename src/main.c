#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "db.h"
#include "auth.h"
#include "queries.h"
#include "operations.h"
#include "utils.h"

#ifdef _WIN32
#include <windows.h>
#endif

static void print_main_menu(void) {
    printf("\n========================================\n");
    printf("     АВТОПАРК - СИСТЕМА УПРАВЛЕНИЯ\n");
    printf("========================================\n");
    printf("1. Просмотр заказов водителя за период\n");
    printf("2. Статистика по автомобилю\n");
    printf("3. Статистика по всем водителям\n");
    printf("4. Водитель с наименьшим числом поездок\n");
    printf("5. Автомобиль с наибольшим пробегом\n");
    printf("6. Добавить автомобиль (INSERT)\n");
    printf("7. Обновить данные автомобиля (UPDATE)\n");
    printf("8. Удалить автомобиль (DELETE)\n");
    printf("9. Добавить водителя (INSERT)\n");
    printf("10. Добавить заказ (INSERT с проверкой грузоподъёмности)\n");
    printf("11. Расчитать зарплату ВСЕМ водителям за период\n");
    printf("12. Расчитать зарплату КОНКРЕТНОМУ водителю за период\n");
    printf("0. Выйти\n");
    printf("Выберите действие: ");
}

static void calculate_earnings_all_period(void) {
    char start_date[20], end_date[20];
    printf("Введите начальную дату (ГГГГ-ММ-ДД): ");
    scanf("%s", start_date);
    printf("Введите конечную дату (ГГГГ-ММ-ДД): ");
    scanf("%s", end_date);

    char sql[512];
    snprintf(sql, sizeof(sql),
        "INSERT INTO driver_salary (driver_id, start_date, end_date, total_amount) "
        "SELECT d.id, '%s', '%s', SUM(o.cost * 0.2) "
        "FROM drivers d "
        "LEFT JOIN orders o ON d.lastname = o.driver_lastname "
        "WHERE o.date >= '%s' AND o.date <= '%s' "
        "GROUP BY d.id;",
        start_date, end_date, start_date, end_date);

    if (db_execute(sql)) {
        printf("Зарплата расчитана и сохранена в таблицу driver_salary.\n");
        get_all_drivers_stats();
    }
    else {
        printf("Не удалось расчитать зарплату.\n");
    }
}

static void calculate_earnings_driver_period(void) {
    char lastname[100], start_date[20], end_date[20];
    printf("Введите фамилию водителя: ");
    scanf("%s", lastname);
    printf("Введите начальную дату (ГГГГ-ММ-ДД): ");
    scanf("%s", start_date);
    printf("Введите конечную дату (ГГГГ-ММ-ДД): ");
    scanf("%s", end_date);

    char sql[512];
    snprintf(sql, sizeof(sql),
        "SELECT SUM(cost * 0.2) as total_earned FROM orders "
        "WHERE driver_lastname = '%s' AND date >= '%s' AND date <= '%s';",
        lastname, start_date, end_date);

    printf("\n=== Заработок водителя %s за период %s - %s ===\n", lastname, start_date, end_date);
    sqlite3_exec(db, sql, db_callback, NULL, NULL);
}

static int check_if_manager(User* user) {
    if (strcmp(user->role, "manager") != 0) {
        printf("Доступ запрещён. Требуются права менеджера.\n");
        return 0;
    }
    return 1;
}

int main(void) {
    // Установка русской локали
    setlocale(LC_ALL, "Russian");

    // Для Windows
#ifdef _WIN32
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
#endif

    if (!db_init("autopark.db")) {
        printf("Не удалось подключиться к базе данных.\n");
        return 1;
    }

    User current_user = { "", "" };
    char login[50], password[50];
    int choice;

    printf("=== СИСТЕМА АВТОПАРК ===\n");

    while (!is_logged_in(&current_user)) {
        printf("Логин: ");
        scanf("%s", login);
        printf("Пароль: ");
        scanf("%s", password);

        if (!authenticate(login, password, &current_user)) {
            printf("Ошибка. Попробуйте снова.\n");
        }
    }

    do {
        print_main_menu();
        scanf("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1: {
            char lastname[100], start[20], end[20];
            printf("Введите фамилию водителя: ");
            scanf("%s", lastname);
            printf("Введите начальную дату (ГГГГ-ММ-ДД): ");
            scanf("%s", start);
            printf("Введите конечную дату (ГГГГ-ММ-ДД): ");
            scanf("%s", end);
            get_orders_by_driver_period(lastname, start, end, current_user.username, current_user.role);
            break;
        }
        case 2: {
            char car_number[100];
            printf("Введите номер автомобиля: ");
            scanf("%s", car_number);
            get_car_stats(car_number);
            break;
        }
        case 3:
            get_all_drivers_stats();
            break;
        case 4:
            get_driver_with_fewest_trips();
            break;
        case 5:
            get_car_with_max_mileage();
            break;
        case 6:
            if (check_if_manager(&current_user)) insert_car();
            break;
        case 7:
            if (check_if_manager(&current_user)) update_car();
            break;
        case 8:
            if (check_if_manager(&current_user)) delete_car();
            break;
        case 9:
            if (check_if_manager(&current_user)) insert_driver();
            break;
        case 10:
            if (check_if_manager(&current_user)) insert_order();
            break;
        case 11:
            if (check_if_manager(&current_user)) calculate_earnings_all_period();
            break;
        case 12:
            if (check_if_manager(&current_user)) calculate_earnings_driver_period();
            break;
        case 0:
            logout(&current_user);
            break;
        default:
            printf("Неверный выбор.\n");
        }

        if (choice != 0 && is_logged_in(&current_user)) {
            wait_for_enter();
        }
    } while (is_logged_in(&current_user));

    db_close();
    printf("До свидания!\n");
    return 0;
}