#ifndef ACCOUNT_H
#define ACCOUNT_H

char* create_account(const char *name, const char *email);
int deposit_cash(const char *account_number, double amount);
int withdraw_cash(const char *account_number, double amount);
double check_balance(const char *account_number);
int close_account(const char *account_number);
void generate_accounts_report();

#endif // ACCOUNT_H