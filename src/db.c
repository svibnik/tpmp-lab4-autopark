#include "db.h"
#include <stdio.h>
#include <stdlib.h>

sqlite3* db = NULL;

int db_init(const char* db_path) {
    int rc = sqlite3_open(db_path, &db);
    if (rc) {
        fprintf(stderr, "Не удалось открыть базу данных: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    return 1;
}

void db_close(void) {
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}

int db_execute(const char* sql) {
    char* err_msg = NULL;
    int rc = sqlite3_exec(db, sql, db_callback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Ошибка SQL: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    return 1;
}

int db_callback(void* NotUsed, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}