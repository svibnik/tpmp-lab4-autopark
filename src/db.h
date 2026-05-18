#ifndef DB_H
#define DB_H

#include <sqlite3.h>

extern sqlite3* db;

int db_init(const char* db_path);
void db_close(void);
int db_execute(const char* sql);
int db_callback(void* NotUsed, int argc, char** argv, char** azColName);

#endif