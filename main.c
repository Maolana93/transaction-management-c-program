#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "account.h"
// #include "transaction.h"

void print_menu() {
    printf("\n--- Transaction Management System ---\n");
    printf("1. Create New Account\n");
    printf("2. Deposit Cash\n");
    printf("3. Withdraw Cash\n");
    printf("4. Check Balance\n");
    printf("5. Close Account\n");
    printf("6. View All Accounts Report\n");
    printf("7. Exit\n");
    printf("Enter your choice: ");
}

int main() {
    // Initialize database
    if (initialize_database() != 0) {
        fprintf(stderr, "Failed to initialize database\n");
        return 1;
    }

    int choice;
    char account_number[50];
    double amount;

    while (1) {
        print_menu();
        scanf("%d", &choice);
        getchar(); // Consume newline

        switch(choice) {
            case 1: {
                char name[100], email[100];
                printf("Enter Full Name: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0; // Remove newline

                printf("Enter Email: ");
                fgets(email, sizeof(email), stdin);
                email[strcspn(email, "\n")] = 0; // Remove newline

                char *new_account = create_account(name, email);
                if (new_account) {
                    printf("Account Created Successfully. Account Number: %s\n", new_account);
                    free(new_account);
                }
                break;
            }
            case 2: {
                printf("Enter Account Number: ");
                fgets(account_number, sizeof(account_number), stdin);
                account_number[strcspn(account_number, "\n")] = 0;

                printf("Enter Deposit Amount: ");
                scanf("%lf", &amount);

                if (deposit_cash(account_number, amount) == 0) {
                    printf("Deposit Successful!\n");
                } else {
                    printf("Deposit Failed.\n");
                }
                break;
            }
            case 3: {
                printf("Enter Account Number: ");
                fgets(account_number, sizeof(account_number), stdin);
                account_number[strcspn(account_number, "\n")] = 0;

                printf("Enter Withdrawal Amount: ");
                scanf("%lf", &amount);

                if (withdraw_cash(account_number, amount) == 0) {
                    printf("Withdrawal Successful!\n");
                } else {
                    printf("Withdrawal Failed.\n");
                }
                break;
            }
            case 4: {
                printf("Enter Account Number: ");
                fgets(account_number, sizeof(account_number), stdin);
                account_number[strcspn(account_number, "\n")] = 0;

                double balance = check_balance(account_number);
                if (balance >= 0) {
                    printf("Current Balance: $%.2f\n", balance);
                } else {
                    printf("Balance Check Failed.\n");
                }
                break;
            }
            case 5: {
                printf("Enter Account Number: ");
                fgets(account_number, sizeof(account_number), stdin);
                account_number[strcspn(account_number, "\n")] = 0;

                if (close_account(account_number) == 0) {
                    printf("Account Closed Successfully.\n");
                } else {
                    printf("Account Closure Failed.\n");
                }
                break;
            }
            // case 6: {
            //     generate_accounts_report();
            //     break;
            // }
            case 6:
                close_database();
                printf("Exiting Transaction Management System.\n");
                return 0;
            default:
                printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
 }