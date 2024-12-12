#include <stdio.h>
#include <sqlite3.h>
#include "database.h"

sqlite3 *db;

int initialize_database() {
    int rc = sqlite3_open("transaction_system.db", &db);
    if (rc) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    // Create accounts table
    const char *create_table_sql = 
        "CREATE TABLE IF NOT EXISTS accounts ("
        "account_number TEXT PRIMARY KEY, "
        "name TEXT, "
        "email TEXT, "
        "balance REAL, "
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP);"
        
        "CREATE TABLE IF NOT EXISTS transactions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "account_number TEXT, "
        "type TEXT, "
        "amount REAL, "
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP)";

    char *err_msg = 0;
    rc = sqlite3_exec(db, create_table_sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }

    return SQLITE_OK;
}

void close_database() {
    sqlite3_close(db);
}