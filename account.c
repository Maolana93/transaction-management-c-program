#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <cjson/cJSON.h>
#include "account.h"
#include "database.h"


extern sqlite3 *db;

char* generate_account_number() {
    char *account_number = malloc(10);
    snprintf(account_number, 10, "AC%06d", rand() % 1000000);
    return account_number;
}

char* create_account(const char *name, const char *email) {
    char *account_number = generate_account_number();
    char sql[500];
    char *err_msg = 0;

    snprintf(sql, sizeof(sql), 
        "INSERT INTO accounts (account_number, name, email, balance) "
        "VALUES ('%s', '%s', '%s', 0.0)", 
        account_number, name, email);

    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        free(account_number);
        return NULL;
    }

    return account_number;
}

int deposit_cash(const char *account_number, double amount) {
    if (amount <= 0) return -1;

    char sql[500];
    snprintf(sql, sizeof(sql), 
        "UPDATE accounts SET balance = balance + %f WHERE account_number = '%s'", 
        amount, account_number);

    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }

    // Log transaction
    snprintf(sql, sizeof(sql), 
        "INSERT INTO transactions (account_number, type, amount) "
        "VALUES ('%s', 'DEPOSIT', %f)", 
        account_number, amount);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    return rc == SQLITE_OK ? 0 : -1;
}

int withdraw_cash(const char *account_number, double amount) {
    if (amount <= 0) return -1;

    char sql[500];
    sqlite3_stmt *stmt;

    // First, check balance
    snprintf(sql, sizeof(sql), 
        "SELECT balance FROM accounts WHERE account_number = '%s'", 
        account_number);

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return -1;

    rc = sqlite3_step(stmt);
    double current_balance = sqlite3_column_double(stmt, 0);
    sqlite3_finalize(stmt);

    if (current_balance < amount) return -1;

    snprintf(sql, sizeof(sql), 
        "UPDATE accounts SET balance = balance - %f WHERE account_number = '%s'", 
        amount, account_number);

    char *err_msg = 0;
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }

    // Log transaction
    snprintf(sql, sizeof(sql), 
        "INSERT INTO transactions (account_number, type, amount) "
        "VALUES ('%s', 'WITHDRAWAL', %f)", 
        account_number, amount);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    return rc == SQLITE_OK ? 0 : -1;
}

double check_balance(const char *account_number) {
    char sql[500];
    sqlite3_stmt *stmt;

    snprintf(sql, sizeof(sql), 
        "SELECT balance FROM accounts WHERE account_number = '%s'", 
        account_number);

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return -1;

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return -1;
    }

    double balance = sqlite3_column_double(stmt, 0);
    sqlite3_finalize(stmt);

    return balance;
}

int close_account(const char *account_number) {
    char sql[500];
    char *err_msg = 0;

    // Ensure balance is zero
    double balance = check_balance(account_number);
    if (balance > 0) return -1;

    snprintf(sql, sizeof(sql), 
        "DELETE FROM accounts WHERE account_number = '%s'", 
        account_number);

    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }

    return 0;
}

void generate_accounts_report() {
    // Create a JSON array to hold account objects
    cJSON *report = cJSON_CreateArray();

    // Assume we're fetching accounts from database
    sqlite3_stmt *stmt;
    char *sql = "SELECT account_number, name, email, balance FROM accounts";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch accounts\n");
        return;
    }

    // Iterate through accounts and create JSON objects
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        cJSON *account = cJSON_CreateObject();
        
        // Add account details to JSON object
        cJSON_AddStringToObject(account, "account_number", 
            (const char*)sqlite3_column_text(stmt, 0));
        cJSON_AddStringToObject(account, "name", 
            (const char*)sqlite3_column_text(stmt, 1));
        cJSON_AddStringToObject(account, "email", 
            (const char*)sqlite3_column_text(stmt, 2));
        cJSON_AddNumberToObject(account, "balance", 
            sqlite3_column_double(stmt, 3));
        
        // Add account object to report array
        cJSON_AddItemToArray(report, account);
    }

    sqlite3_finalize(stmt);

    // Convert JSON to string
    char *json_report = cJSON_Print(report);
    
    // Write to file (optional)
    FILE *file = fopen("accounts_report.json", "w");
    if (file) {
        fprintf(file, "%s", json_report);
        fclose(file);
    }

    // Print to console
    printf("Accounts Report:\n%s\n", json_report);

    // Clean up
    free(json_report);
    cJSON_Delete(report);
}

// Function to parse JSON input for account creation
int create_account_from_json(const char *json_string) {
    cJSON *json = cJSON_Parse(json_string);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Error parsing JSON: %s\n", error_ptr);
        }
        return -1;
    }

    // Extract account details from JSON
    cJSON *name = cJSON_GetObjectItemCaseSensitive(json, "name");
    cJSON *email = cJSON_GetObjectItemCaseSensitive(json, "email");
    
    if (!cJSON_IsString(name) || !cJSON_IsString(email)) {
        cJSON_Delete(json);
        return -1;
    }

    // Create account using extracted details
    char *account_number = create_account(name->valuestring, email->valuestring);
    
    cJSON_Delete(json);
    
    return account_number ? 0 : -1;
}
// void generate_accounts_report() {
//     char *sql = "SELECT * FROM accounts";
//     sqlite3_stmt *stmt;
//     cJSON *report = cJSON_CreateArray();

//     int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
//     if (rc != SQLITE_OK) {
//         fprintf(stderr, "Failed to retrieve accounts\n");
//         return;
//     }

//     while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
//         cJSON *account = cJSON_CreateObject();
//         cJSON_AddStringToObject(account, "account_number", (const char*)sqlite3_column_text(stmt, 0));
//         cJSON_AddStringToObject(account, "name", (const char*)sqlite3_column_text(stmt, 1));
//         cJSON_AddStringToObject(account, "email", (const char*)sqlite3_column_text(stmt, 2));
//         cJSON_AddNumberToObject(account, "balance", sqlite3_column_double(stmt, 3));
//         cJSON_AddStringToObject(account, "created_at", (const char*)sqlite3_column_text(stmt, 4));

//         cJSON_AddItemToArray(report, account);
//     }

//     sqlite3_finalize(stmt);

//     char *json_report = cJSON_Print(report);
//     printf("Accounts Report:\n%s\n", json_report);

//     free(json_report);
//     cJSON_Delete(report);
// }