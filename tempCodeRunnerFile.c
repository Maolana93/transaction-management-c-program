
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