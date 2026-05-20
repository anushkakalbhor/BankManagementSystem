/*
 * bank.c — Bank Management System

 * All API responses are JSON for the web frontend.
 */

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

/* ==================== ORIGINAL TERMINAL FUNCTIONS ==================== */

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

/* ==================== API FUNCTIONS (for web frontend) ==================== */

void api_login(int acc_no, char *password); 
void api_create(char *name, int acc_no, char *password, float initial_deposit);
void api_deposit(int acc_no, char *password, float amount);
void api_withdraw(int acc_no, char *password, float amount);
void api_transfer(int acc_no, char *password, int to_acc, float amount);
void api_balance(int acc_no, char *password);
void api_history(int acc_no, char *password);
void api_update(int acc_no, char *password, char *new_name, char *new_password);
void api_delete(int acc_no, char *password);
void api_admin(char *admin_password);

/* ==================== MAIN ==================== */

int main(int argc, char *argv[]) {

    /* ---- API MODE: called by web server with arguments ---- */
    if (argc >= 2) {
        char *cmd = argv[1];

        if (strcmp(cmd, "login") == 0 && argc == 4) {
            api_login(atoi(argv[2]), argv[3]);
        }
        else if (strcmp(cmd, "create") == 0 && argc == 6) {
            api_create(argv[2], atoi(argv[3]), argv[4], atof(argv[5]));
        }
        else if (strcmp(cmd, "deposit") == 0 && argc == 5) {
            api_deposit(atoi(argv[2]), argv[3], atof(argv[4]));
        }
        else if (strcmp(cmd, "withdraw") == 0 && argc == 5) {
            api_withdraw(atoi(argv[2]), argv[3], atof(argv[4]));
        }
        else if (strcmp(cmd, "transfer") == 0 && argc == 6) {
            api_transfer(atoi(argv[2]), argv[3], atoi(argv[4]), atof(argv[5]));
        }
        else if (strcmp(cmd, "balance") == 0 && argc == 4) {
            api_balance(atoi(argv[2]), argv[3]);
        }
        else if (strcmp(cmd, "history") == 0 && argc == 4) {
            api_history(atoi(argv[2]), argv[3]);
        }
        else if (strcmp(cmd, "update") == 0 && argc == 6) {
            api_update(atoi(argv[2]), argv[3], argv[4], argv[5]);
        }
        else if (strcmp(cmd, "delete") == 0 && argc == 4) {
            api_delete(atoi(argv[2]), argv[3]);
        }
        else if (strcmp(cmd, "admin") == 0 && argc == 3) {
            api_admin(argv[2]);
        }
        else {
            printf("{\"ok\":false,\"msg\":\"Unknown command or wrong arguments\"}\n");
        }
        return 0;
    }

    /* ---- INTERACTIVE TERMINAL MODE (original program) ---- */
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
            case 1: create_account(); break;
            case 2: deposit_money(); break;
            case 3: withdraw_money(); break;
            case 4: check_balance(); break;
            case 5: transfer_money(); break;
            case 6: delete_account(); break;
            case 7: update_account(); break;
            case 8: {
                int acc_no;
                if (login(&acc_no)) transaction_history(acc_no);
                break;
            }
            case 9: view_all_accounts(); break;
            case 10:
                printf("\nThank you for visiting the bank!\n");
                exit(0);
            default:
                printf("\nInvalid choice!\n");
        }
    }
}

/* ==================== HELPER: Find account, return 1=found ==================== */

int account_exists(int acc_no) {
    FILE *file = fopen(ACCOUNT_FILE, "rb");
    Account acc;
    if (!file) return 0;
    while (fread(&acc, sizeof(acc), 1, file))
        if (acc.acc_no == acc_no) { fclose(file); return 1; }
    fclose(file);
    return 0;
}

/* Returns 1 if found and password matches, fills acc struct */
int find_and_auth(int acc_no, char *password, Account *out) {
    FILE *file = fopen(ACCOUNT_FILE, "rb");
    Account acc;
    if (!file) return 0;
    while (fread(&acc, sizeof(acc), 1, file)) {
        if (acc.acc_no == acc_no && strcmp(acc.password, password) == 0) {
            *out = acc;
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

void log_transaction(int acc_no, char action[], float amount) {
    FILE *file = fopen(TRANSACTION_FILE, "a");
    time_t t; time(&t);
    fprintf(file, "Account: %d | %s | Amount: %.2f | %s",
            acc_no, action, amount, ctime(&t));
    fclose(file);
}

/* ==================== API IMPLEMENTATIONS ==================== */

void api_login(int acc_no, char *password) {
    Account acc;
    if (find_and_auth(acc_no, password, &acc)) {
        printf("{\"ok\":true,\"name\":\"%s\",\"acc_no\":%d,\"balance\":%.2f}\n",
               acc.name, acc.acc_no, acc.balance);
    } else {
        printf("{\"ok\":false,\"msg\":\"Invalid account number or password\"}\n");
    }
}

void api_create(char *name, int acc_no, char *password, float initial_deposit) {
    if (account_exists(acc_no)) {
        printf("{\"ok\":false,\"msg\":\"Account number already exists\"}\n");
        return;
    }
    if (initial_deposit < 500) {
        printf("{\"ok\":false,\"msg\":\"Minimum initial deposit of Rs.500 is required to open an account\"}\n");
        return;
    }
    FILE *file = fopen(ACCOUNT_FILE, "ab+");
    if (!file) { printf("{\"ok\":false,\"msg\":\"File error\"}\n"); return; }
    Account acc;
    strncpy(acc.name, name, 49); acc.name[49] = '\0';
    acc.acc_no = acc_no;
    strncpy(acc.password, password, 19); acc.password[19] = '\0';
    acc.balance = initial_deposit;
    fwrite(&acc, sizeof(acc), 1, file);
    fclose(file);
    printf("{\"ok\":true,\"msg\":\"Account created successfully\",\"balance\":%.2f}\n", initial_deposit);
}

void api_deposit(int acc_no, char *password, float amount) {
    Account acc;
    if (!find_and_auth(acc_no, password, &acc)) {
        printf("{\"ok\":false,\"msg\":\"Invalid account number or password\"}\n"); return;
    }
    if (amount <= 0) {
        printf("{\"ok\":false,\"msg\":\"Invalid amount\"}\n"); return;
    }
    FILE *file = fopen(ACCOUNT_FILE, "rb+");
    Account tmp;
    while (fread(&tmp, sizeof(tmp), 1, file)) {
        if (tmp.acc_no == acc_no) {
            tmp.balance += amount;
            fseek(file, -(long)sizeof(tmp), SEEK_CUR);
            fwrite(&tmp, sizeof(tmp), 1, file);
            log_transaction(acc_no, "Deposited", amount);
            printf("{\"ok\":true,\"balance\":%.2f,\"msg\":\"Deposit successful\"}\n", tmp.balance);
            fclose(file); return;
        }
    }
    fclose(file);
    printf("{\"ok\":false,\"msg\":\"Account not found\"}\n");
}

void api_withdraw(int acc_no, char *password, float amount) {
    Account acc;
    if (!find_and_auth(acc_no, password, &acc)) {
        printf("{\"ok\":false,\"msg\":\"Invalid account number or password\"}\n"); return;
    }
    if (amount <= 0) {
        printf("{\"ok\":false,\"msg\":\"Invalid amount\"}\n"); return;
    }
    if (acc.balance - amount < 500) {
        printf("{\"ok\":false,\"msg\":\"Minimum balance of Rs.500 required\"}\n"); return;
    }
    FILE *file = fopen(ACCOUNT_FILE, "rb+");
    Account tmp;
    while (fread(&tmp, sizeof(tmp), 1, file)) {
        if (tmp.acc_no == acc_no) {
            tmp.balance -= amount;
            fseek(file, -(long)sizeof(tmp), SEEK_CUR);
            fwrite(&tmp, sizeof(tmp), 1, file);
            log_transaction(acc_no, "Withdrawn", amount);
            printf("{\"ok\":true,\"balance\":%.2f,\"msg\":\"Withdrawal successful\"}\n", tmp.balance);
            fclose(file); return;
        }
    }
    fclose(file);
    printf("{\"ok\":false,\"msg\":\"Account not found\"}\n");
}

void api_transfer(int acc_no, char *password, int to_acc, float amount) {
    Account sender;
    if (!find_and_auth(acc_no, password, &sender)) {
        printf("{\"ok\":false,\"msg\":\"Invalid account number or password\"}\n"); return;
    }
    if (!account_exists(to_acc)) {
        printf("{\"ok\":false,\"msg\":\"Receiver account not found\"}\n"); return;
    }
    if (amount <= 0) {
        printf("{\"ok\":false,\"msg\":\"Invalid amount\"}\n"); return;
    }
    if (sender.balance < amount) {
        printf("{\"ok\":false,\"msg\":\"Insufficient balance\"}\n"); return;
    }

    FILE *file = fopen(ACCOUNT_FILE, "rb+");
    Account tmp;

    /* Deduct from sender */
    rewind(file);
    while (fread(&tmp, sizeof(tmp), 1, file)) {
        if (tmp.acc_no == acc_no) {
            tmp.balance -= amount;
            fseek(file, -(long)sizeof(tmp), SEEK_CUR);
            fwrite(&tmp, sizeof(tmp), 1, file);
            break;
        }
    }

    /* Add to receiver */
    rewind(file);
    while (fread(&tmp, sizeof(tmp), 1, file)) {
        if (tmp.acc_no == to_acc) {
            tmp.balance += amount;
            fseek(file, -(long)sizeof(tmp), SEEK_CUR);
            fwrite(&tmp, sizeof(tmp), 1, file);
            break;
        }
    }

    fclose(file);
    log_transaction(acc_no, "Transferred", amount);

    /* Get updated sender balance */
    Account updated;
    find_and_auth(acc_no, password, &updated);
    printf("{\"ok\":true,\"balance\":%.2f,\"msg\":\"Transfer successful\"}\n", updated.balance);
}

void api_balance(int acc_no, char *password) {
    Account acc;
    if (!find_and_auth(acc_no, password, &acc)) {
        printf("{\"ok\":false,\"msg\":\"Invalid account number or password\"}\n"); return;
    }
    printf("{\"ok\":true,\"name\":\"%s\",\"balance\":%.2f}\n", acc.name, acc.balance);
}

void api_history(int acc_no, char *password) {
    Account acc;
    if (!find_and_auth(acc_no, password, &acc)) {
        printf("{\"ok\":false,\"msg\":\"Invalid account number or password\"}\n"); return;
    }

    FILE *file = fopen(TRANSACTION_FILE, "r");
    char line[200];
    char acc_string[30];
    sprintf(acc_string, "Account: %d", acc_no);

    printf("{\"ok\":true,\"transactions\":[");

    int first = 1;
    if (file) {
        while (fgets(line, sizeof(line), file)) {
            if (strstr(line, acc_string)) {
                /* Remove trailing newline for JSON */
                line[strcspn(line, "\n")] = '\0';
                /* Escape quotes in line */
                if (!first) printf(",");
                printf("\"%s\"", line);
                first = 0;
            }
        }
        fclose(file);
    }

    printf("]}\n");
}

void api_update(int acc_no, char *password, char *new_name, char *new_password) {
    Account acc;
    if (!find_and_auth(acc_no, password, &acc)) {
        printf("{\"ok\":false,\"msg\":\"Invalid account number or password\"}\n"); return;
    }
    FILE *file = fopen(ACCOUNT_FILE, "rb+");
    Account tmp;
    while (fread(&tmp, sizeof(tmp), 1, file)) {
        if (tmp.acc_no == acc_no) {
            strncpy(tmp.name, new_name, 49); tmp.name[49] = '\0';
            strncpy(tmp.password, new_password, 19); tmp.password[19] = '\0';
            fseek(file, -(long)sizeof(tmp), SEEK_CUR);
            fwrite(&tmp, sizeof(tmp), 1, file);
            printf("{\"ok\":true,\"msg\":\"Account updated successfully\"}\n");
            fclose(file); return;
        }
    }
    fclose(file);
    printf("{\"ok\":false,\"msg\":\"Account not found\"}\n");
}

void api_delete(int acc_no, char *password) {
    Account acc;
    if (!find_and_auth(acc_no, password, &acc)) {
        printf("{\"ok\":false,\"msg\":\"Invalid account number or password\"}\n"); return;
    }
    FILE *file = fopen(ACCOUNT_FILE, "rb");
    FILE *temp = fopen(TEMP_FILE, "wb");
    Account tmp;
    int found = 0;
    while (fread(&tmp, sizeof(tmp), 1, file)) {
        if (tmp.acc_no == acc_no) { found = 1; continue; }
        fwrite(&tmp, sizeof(tmp), 1, temp);
    }
    fclose(file); fclose(temp);
    remove(ACCOUNT_FILE);
    rename(TEMP_FILE, ACCOUNT_FILE);
    if (found)
        printf("{\"ok\":true,\"msg\":\"Account deleted successfully\"}\n");
    else
        printf("{\"ok\":false,\"msg\":\"Account not found\"}\n");
}

void api_admin(char *admin_password) {
    if (strcmp(admin_password, "admin123") != 0) {
        printf("{\"ok\":false,\"msg\":\"Wrong admin password\"}\n"); return;
    }
    FILE *file = fopen(ACCOUNT_FILE, "rb");
    Account acc;
    printf("{\"ok\":true,\"accounts\":[");
    int first = 1;
    if (file) {
        while (fread(&acc, sizeof(acc), 1, file)) {
            if (!first) printf(",");
            printf("{\"name\":\"%s\",\"acc_no\":%d,\"balance\":%.2f}",
                   acc.name, acc.acc_no, acc.balance);
            first = 0;
        }
        fclose(file);
    }
    printf("]}\n");
}

/* ==================== ORIGINAL INTERACTIVE FUNCTIONS ==================== */

void create_account() {
    FILE *file = fopen(ACCOUNT_FILE, "ab+");
    Account acc;
    if (!file) { printf("\nUnable to open file!\n"); return; }
    getchar();
    printf("\nEnter your name: ");
    fgets(acc.name, sizeof(acc.name), stdin);
    acc.name[strcspn(acc.name, "\n")] = '\0';
    printf("Enter account number: ");
    scanf("%d", &acc.acc_no);
    if (account_exists(acc.acc_no)) {
        printf("\nAccount number already exists!\n"); fclose(file); return;
    }
    printf("Enter password: ");
    scanf("%s", acc.password);

    printf("Enter initial deposit amount (minimum Rs.500): ");
    scanf("%f", &acc.balance);
    if (acc.balance < 500) {
        printf("\nMinimum initial deposit of Rs.500 is required!\n");
        fclose(file);
        return;
    }

    fwrite(&acc, sizeof(acc), 1, file);
    fclose(file);
    printf("\nAccount created successfully! Opening balance: Rs.%.2f\n", acc.balance);
}

int login(int *acc_no) {
    FILE *file = fopen(ACCOUNT_FILE, "rb");
    Account acc;
    char password[20];
    if (!file) { printf("\nUnable to open file!\n"); return 0; }
    printf("\nEnter account number: ");
    scanf("%d", acc_no);
    printf("Enter password: ");
    scanf("%s", password);
    while (fread(&acc, sizeof(acc), 1, file)) {
        if (acc.acc_no == *acc_no && strcmp(acc.password, password) == 0) {
            fclose(file); return 1;
        }
    }
    fclose(file);
    printf("\nInvalid account number or password!\n");
    return 0;
}

void deposit_money() {
    FILE *file = fopen(ACCOUNT_FILE, "rb+");
    Account acc;
    int acc_no;
    float amount;
    if (!login(&acc_no)) return;
    printf("Enter amount to deposit: ");
    scanf("%f", &amount);
    if (amount <= 0) { printf("\nInvalid amount!\n"); fclose(file); return; }
    while (fread(&acc, sizeof(acc), 1, file)) {
        if (acc.acc_no == acc_no) {
            acc.balance += amount;
            fseek(file, -sizeof(acc), SEEK_CUR);
            fwrite(&acc, sizeof(acc), 1, file);
            log_transaction(acc_no, "Deposited", amount);
            printf("\nDeposit successful!\nNew Balance: %.2f\n", acc.balance);
            fclose(file); return;
        }
    }
    fclose(file);
}

void withdraw_money() {
    FILE *file = fopen(ACCOUNT_FILE, "rb+");
    Account acc;
    int acc_no;
    float amount;
    if (!login(&acc_no)) return;
    printf("Enter amount to withdraw: ");
    scanf("%f", &amount);
    if (amount <= 0) { printf("\nInvalid amount!\n"); fclose(file); return; }
    while (fread(&acc, sizeof(acc), 1, file)) {
        if (acc.acc_no == acc_no) {
            if (acc.balance - amount < 500) {
                printf("\nMinimum balance of Rs.500 required!\n"); fclose(file); return;
            }
            acc.balance -= amount;
            fseek(file, -sizeof(acc), SEEK_CUR);
            fwrite(&acc, sizeof(acc), 1, file);
            log_transaction(acc_no, "Withdrawn", amount);
            printf("\nWithdrawal successful!\nRemaining Balance: %.2f\n", acc.balance);
            fclose(file); return;
        }
    }
    fclose(file);
}

void check_balance() {
    FILE *file = fopen(ACCOUNT_FILE, "rb");
    Account acc;
    int acc_no;
    if (!login(&acc_no)) return;
    while (fread(&acc, sizeof(acc), 1, file)) {
        if (acc.acc_no == acc_no) {
            printf("\nName: %s\nCurrent Balance: %.2f\n", acc.name, acc.balance);
            fclose(file); return;
        }
    }
    fclose(file);
}

void transfer_money() {
    FILE *file = fopen(ACCOUNT_FILE, "rb+");
    Account sender, receiver;
    int sender_acc, receiver_acc;
    float amount;
    if (!login(&sender_acc)) return;
    printf("Enter receiver account number: ");
    scanf("%d", &receiver_acc);
    if (!account_exists(receiver_acc)) {
        printf("\nReceiver account not found!\n"); fclose(file); return;
    }
    printf("Enter amount to transfer: ");
    scanf("%f", &amount);
    if (amount <= 0) { printf("\nInvalid amount!\n"); fclose(file); return; }
    rewind(file);
    while (fread(&sender, sizeof(sender), 1, file)) {
        if (sender.acc_no == sender_acc) {
            if (sender.balance < amount) {
                printf("\nInsufficient balance!\n"); fclose(file); return;
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
    int acc_no, found = 0;
    if (!login(&acc_no)) return;
    while (fread(&acc, sizeof(acc), 1, file)) {
        if (acc.acc_no == acc_no) { found = 1; continue; }
        fwrite(&acc, sizeof(acc), 1, temp);
    }
    fclose(file); fclose(temp);
    remove(ACCOUNT_FILE);
    rename(TEMP_FILE, ACCOUNT_FILE);
    if (found) printf("\nAccount deleted successfully!\n");
    else printf("\nAccount not found!\n");
}

void update_account() {
    FILE *file = fopen(ACCOUNT_FILE, "rb+");
    Account acc;
    int acc_no;
    if (!login(&acc_no)) return;
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
            fclose(file); return;
        }
    }
    fclose(file);
}

void transaction_history(int acc_no) {
    FILE *file = fopen(TRANSACTION_FILE, "r");
    char line[200];
    char acc_string[20];
    sprintf(acc_string, "Account: %d", acc_no);
    if (!file) { printf("\nNo transaction history found!\n"); return; }
    printf("\n===== TRANSACTION HISTORY =====\n");
    while (fgets(line, sizeof(line), file))
        if (strstr(line, acc_string)) printf("%s", line);
    fclose(file);
}

void view_all_accounts() {
    FILE *file = fopen(ACCOUNT_FILE, "rb");
    Account acc;
    char admin_pass[20];
    printf("\nEnter admin password: ");
    scanf("%s", admin_pass);
    if (strcmp(admin_pass, "admin123") != 0) {
        printf("\nWrong admin password!\n"); return;
    }
    printf("\n=========== ALL ACCOUNTS ===========\n");
    while (fread(&acc, sizeof(acc), 1, file)) {
        printf("\nName: %s\nAccount No: %d\nBalance: %.2f\n----------------------------------\n",
               acc.name, acc.acc_no, acc.balance);
    }
    fclose(file);
}