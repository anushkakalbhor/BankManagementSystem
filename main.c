#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ACCOUNT_FILE "account.dat"
#define TEMP_FILE "temp.dat"
#define TRANSACTION_FILE "transactions.txt"

typedef struct {
    char name[50];
    int acc_no;
    char password[20];
    float balance;
} Account;

void create_account();
void deposit_money();
void withdraw_money();
void check_balance();
void transfer_money();
void delete_account();
void update_account();
void view_all_accounts();
void transaction_history(int acc_no);
void log_transaction(int acc_no, char action[], float amount);
int login(int *acc_no);
int account_exists(int acc_no);

int main() {
    int choice;

    while (1) {
        printf("\n\n====================================");
        printf("\n      BANK MANAGEMENT SYSTEM");
        printf("\n====================================");
        printf("\n1. Create Account");
        printf("\n2. Deposit Money");
        printf("\n3. Withdraw Money");
        printf("\n4. Check Balance");
        printf("\n5. Transfer Money");
        printf("\n6. Delete Account");
        printf("\n7. Update Account Details");
        printf("\n8. View Transaction History");
        printf("\n9. Admin - View All Accounts");
        printf("\n10. Exit");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                create_account();
                break;

            case 2:
                deposit_money();
                break;

            case 3:
                withdraw_money();
                break;

            case 4:
                check_balance();
                break;

            case 5:
                transfer_money();
                break;

            case 6:
                delete_account();
                break;

            case 7:
                update_account();
                break;

            case 8: {
                int acc_no;
                if (login(&acc_no)) {
                    transaction_history(acc_no);
                }
                break;
            }

            case 9:
                view_all_accounts();
                break;

            case 10:
                printf("\nThank you for visiting the bank!\n");
                exit(0);

            default:
                printf("\nInvalid choice!\n");
        }
    }
}

int account_exists(int acc_no) {
    FILE *file = fopen(ACCOUNT_FILE, "rb");
    Account acc;

    if (file == NULL)
        return 0;

    while (fread(&acc, sizeof(acc), 1, file)) {
        if (acc.acc_no == acc_no) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

void create_account() {
    FILE *file = fopen(ACCOUNT_FILE, "ab+");
    Account acc;

    if (file == NULL) {
        printf("\nUnable to open file!\n");
        return;
    }

    getchar();

    printf("\nEnter your name: ");
    fgets(acc.name, sizeof(acc.name), stdin);
    acc.name[strcspn(acc.name, "\n")] = '\0';

    printf("Enter account number: ");
    scanf("%d", &acc.acc_no);

    if (account_exists(acc.acc_no)) {
        printf("\nAccount number already exists!\n");
        fclose(file);
        return;
    }

    printf("Enter password: ");
    scanf("%s", acc.password);

    acc.balance = 0;

    fwrite(&acc, sizeof(acc), 1, file);
    fclose(file);

    printf("\nAccount created successfully!\n");
}

int login(int *acc_no) {
    FILE *file = fopen(ACCOUNT_FILE, "rb");
    Account acc;
    char password[20];

    if (file == NULL) {
        printf("\nUnable to open file!\n");
        return 0;
    }

    printf("\nEnter account number: ");
    scanf("%d", acc_no);

    printf("Enter password: ");
    scanf("%s", password);

    while (fread(&acc, sizeof(acc), 1, file)) {
        if (acc.acc_no == *acc_no && strcmp(acc.password, password) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    printf("\nInvalid account number or password!\n");
    return 0;
}

void log_transaction(int acc_no, char action[], float amount) {
    FILE *file = fopen(TRANSACTION_FILE, "a");

    time_t t;
    time(&t);

    fprintf(file, "Account: %d | %s | Amount: %.2f | %s",
            acc_no, action, amount, ctime(&t));

    fclose(file);
}

void deposit_money() {
    FILE *file = fopen(ACCOUNT_FILE, "rb+");
    Account acc;
    int acc_no;
    float amount;

    if (!login(&acc_no))
        return;

    printf("Enter amount to deposit: ");
    scanf("%f", &amount);

    if (amount <= 0) {
        printf("\nInvalid amount!\n");
        fclose(file);
        return;
    }

    while (fread(&acc, sizeof(acc), 1, file)) {
        if (acc.acc_no == acc_no) {
            acc.balance += amount;

            fseek(file, -sizeof(acc), SEEK_CUR);
            fwrite(&acc, sizeof(acc), 1, file);

            log_transaction(acc_no, "Deposited", amount);

            printf("\nDeposit successful!\n");
            printf("New Balance: %.2f\n", acc.balance);

            fclose(file);
            return;
        }
    }

    fclose(file);
}

void withdraw_money() {
    FILE *file = fopen(ACCOUNT_FILE, "rb+");
    Account acc;
    int acc_no;
    float amount;

    if (!login(&acc_no))
        return;

    printf("Enter amount to withdraw: ");
    scanf("%f", &amount);

    if (amount <= 0) {
        printf("\nInvalid amount!\n");
        fclose(file);
        return;
    }

    while (fread(&acc, sizeof(acc), 1, file)) {
        if (acc.acc_no == acc_no) {
            if (acc.balance - amount < 500) {
                printf("\nMinimum balance of Rs.500 required!\n");
                fclose(file);
                return;
            }

            acc.balance -= amount;

            fseek(file, -sizeof(acc), SEEK_CUR);
            fwrite(&acc, sizeof(acc), 1, file);

            log_transaction(acc_no, "Withdrawn", amount);

            printf("\nWithdrawal successful!\n");
            printf("Remaining Balance: %.2f\n", acc.balance);

            fclose(file);
            return;
        }
    }

    fclose(file);
}

void check_balance() {
    FILE *file = fopen(ACCOUNT_FILE, "rb");
    Account acc;
    int acc_no;

    if (!login(&acc_no))
        return;

    while (fread(&acc, sizeof(acc), 1, file)) {
        if (acc.acc_no == acc_no) {
            printf("\nName: %s", acc.name);
            printf("\nCurrent Balance: %.2f\n", acc.balance);
            fclose(file);
            return;
        }
    }

    fclose(file);
}

void transfer_money() {
    FILE *file = fopen(ACCOUNT_FILE, "rb+");
    Account sender, receiver;

    int sender_acc, receiver_acc;
    float amount;

    if (!login(&sender_acc))
        return;

    printf("Enter receiver account number: ");
    scanf("%d", &receiver_acc);

    if (!account_exists(receiver_acc)) {
        printf("\nReceiver account not found!\n");
        fclose(file);
        return;
    }

    printf("Enter amount to transfer: ");
    scanf("%f", &amount);

    if (amount <= 0) {
        printf("\nInvalid amount!\n");
        fclose(file);
        return;
    }

    rewind(file);

    while (fread(&sender, sizeof(sender), 1, file)) {
        if (sender.acc_no == sender_acc) {
            if (sender.balance < amount) {
                printf("\nInsufficient balance!\n");
                fclose(file);
                return;
            }

            sender.balance -= amount;
            fseek(file, -sizeof(sender), SEEK_CUR);
            fwrite(&sender, sizeof(sender), 1, file);
            break;
        }
    }

    rewind(file);

    while (fread(&receiver, sizeof(receiver), 1, file)) {
        if (receiver.acc_no == receiver_acc) {
            receiver.balance += amount;
            fseek(file, -sizeof(receiver), SEEK_CUR);
            fwrite(&receiver, sizeof(receiver), 1, file);
            break;
        }
    }

    log_transaction(sender_acc, "Transferred", amount);

    printf("\nMoney transferred successfully!\n");

    fclose(file);
}

void delete_account() {
    FILE *file = fopen(ACCOUNT_FILE, "rb");
    FILE *temp = fopen(TEMP_FILE, "wb");

    Account acc;
    int acc_no;
    int found = 0;

    if (!login(&acc_no))
        return;

    while (fread(&acc, sizeof(acc), 1, file)) {
        if (acc.acc_no == acc_no) {
            found = 1;
            continue;
        }

        fwrite(&acc, sizeof(acc), 1, temp);
    }

    fclose(file);
    fclose(temp);

    remove(ACCOUNT_FILE);
    rename(TEMP_FILE, ACCOUNT_FILE);

    if (found)
        printf("\nAccount deleted successfully!\n");
    else
        printf("\nAccount not found!\n");
}

void update_account() {
    FILE *file = fopen(ACCOUNT_FILE, "rb+");
    Account acc;
    int acc_no;

    if (!login(&acc_no))
        return;

    while (fread(&acc, sizeof(acc), 1, file)) {
        if (acc.acc_no == acc_no) {
            getchar();

            printf("\nEnter new name: ");
            fgets(acc.name, sizeof(acc.name), stdin);
            acc.name[strcspn(acc.name, "\n")] = '\0';

            printf("Enter new password: ");
            scanf("%s", acc.password);

            fseek(file, -sizeof(acc), SEEK_CUR);
            fwrite(&acc, sizeof(acc), 1, file);

            printf("\nAccount updated successfully!\n");

            fclose(file);
            return;
        }
    }

    fclose(file);
}

void transaction_history(int acc_no) {
    FILE *file = fopen(TRANSACTION_FILE, "r");
    char line[200];
    char acc_string[20];

    sprintf(acc_string, "Account: %d", acc_no);

    if (file == NULL) {
        printf("\nNo transaction history found!\n");
        return;
    }

    printf("\n===== TRANSACTION HISTORY =====\n");

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, acc_string)) {
            printf("%s", line);
        }
    }

    fclose(file);
}

void view_all_accounts() {
    FILE *file = fopen(ACCOUNT_FILE, "rb");
    Account acc;

    char admin_pass[20];

    printf("\nEnter admin password: ");
    scanf("%s", admin_pass);

    if (strcmp(admin_pass, "admin123") != 0) {
        printf("\nWrong admin password!\n");
        return;
    }

    printf("\n=========== ALL ACCOUNTS ===========\n");

    while (fread(&acc, sizeof(acc), 1, file)) {
        printf("\nName: %s", acc.name);
        printf("\nAccount No: %d", acc.acc_no);
        printf("\nBalance: %.2f", acc.balance);
        printf("\n----------------------------------\n");
    }

    fclose(file);
}
