#include "auth.h"
#include "db.h"
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

static int auth_callback(void* data, int argc, char** argv, char** azColName) {
    User* user = (User*)data;
    if (argc >= 2) {
        strcpy(user->username, argv[0] ? argv[0] : "");
        strcpy(user->role, argv[1] ? argv[1] : "");
    }
    return 0;
}

int authenticate(const char* login, const char* password, User* current_user) {
    char sql[256];
    snprintf(sql, sizeof(sql),
        "SELECT login, role FROM users WHERE login = '%s' AND password_hash = '%s';",
        login, password);

    current_user->username[0] = '\0';
    current_user->role[0] = '\0';

    int rc = sqlite3_exec(db, sql, auth_callback, current_user, NULL);

    if (rc == SQLITE_OK && strlen(current_user->username) > 0) {
        printf("Аутентификация успешна. Добро пожаловать, %s!\n", current_user->username);
        return 1;
    }
    else {
        printf("Ошибка аутентификации. Неверный логин или пароль.\n");
        return 0;
    }
}

void logout(User* current_user) {
    current_user->username[0] = '\0';
    current_user->role[0] = '\0';
    printf("Выход выполнен.\n");
}

int is_logged_in(User* current_user) {
    return strlen(current_user->username) > 0;
}