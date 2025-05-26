#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define FILENAME "accounts.txt"

void flush_input()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

typedef struct
{
    int account_number;
    char name[50];
    char surname[50];
    char address[200];
    char pesel[12];
    double balance;
    double loan;
} Account;

Account *accounts = NULL;
int account_count = 0;
int next_account_number = 1001;
int accounts_capacity = 0;

int is_valid_pesel(const char *pesel)
{
    if (strlen(pesel) != 11)
        return 0;
    for (int i = 0; i < 11; ++i)
        if (!isdigit(pesel[i]))
            return 0;
    return 1;
}

int is_valid_name(const char *name)
{
    size_t len = strlen(name);
    if (len == 0 || len > 50)
        return 0;
    for (size_t i = 0; i < len; ++i)
        if (!isalpha(name[i]) && name[i] != '-' && name[i] != ' ')
            return 0;
    return 1;
}

int is_valid_address(const char *address)
{
    size_t len = strlen(address);
    if (len == 0 || len > 100)
        return 0;
    for (size_t i = 0; i < len; ++i)
        if (!isprint(address[i]))
            return 0;
    return 1;
}

int is_valid_amount(double amount)
{
    return amount >= 0;
}

int is_valid_interest(double interest)
{
    return interest >= 0 && interest <= 100;
}

int read_int()
{
    char buffer[100];
    int value;
    while (1)
    {
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        int valid = 1;
        for (size_t i = 0; i < strlen(buffer); ++i)
        {
            if (!isdigit(buffer[i]))
            {
                valid = 0;
                break;
            }
        }

        if (valid && strlen(buffer) > 0)
        {
            value = atoi(buffer);
            return value;
        }
        if (!valid)
        {
            printf("Invalid input. Please enter a valid integer: ");
        }
    }
}

// NOTE: will be obsolete
void ensure_account_capacity()
{
    if (account_count >= accounts_capacity)
    {
        int new_capacity = (accounts_capacity == 0) ? 10 : accounts_capacity * 2;
        Account *new_accounts = realloc(accounts, new_capacity * sizeof(Account));
        if (!new_accounts)
        {
            printf("[E] Memory allocation failed. Cannot create more accounts.\n");
            return;
        }
        accounts = new_accounts;
        accounts_capacity = new_capacity;
    }
}

// TODO: make it just append line with new Account to .txt
int create_account()
{
    ensure_account_capacity();
    if (account_count >= accounts_capacity)
    {
        printf("Cannot create more accounts due to memory constraints.\n");
        return -1;
    }

    Account new_acc;
    new_acc.account_number = next_account_number++;
    printf("Creating new account (Account number: %d)\n", new_acc.account_number);

    printf("Enter name: ");
    scanf(" %49[^\n]", new_acc.name);
    while (!is_valid_name(new_acc.name))
    {
        printf("Invalid name. Try again: ");
        flush_input();
        scanf(" %49[^\n]", new_acc.name);
    }

    printf("Enter surname: ");
    flush_input();
    scanf(" %49[^\n]", new_acc.surname);
    while (!is_valid_name(new_acc.surname))
    {
        printf("Invalid surname. Try again: ");
        flush_input();
        scanf(" %49[^\n]", new_acc.surname);
    }

    printf("Enter address: ");
    flush_input();
    scanf(" %99[^\n]", new_acc.address);
    while (!is_valid_address(new_acc.address))
    {
        printf("Invalid address. Try again: ");
        flush_input();
        scanf(" %99[^\n]", new_acc.address);
    }

    printf("Enter PESEL: ");
    flush_input();
    scanf(" %11s", new_acc.pesel);
    while (!is_valid_pesel(new_acc.pesel))
    {
        printf("Invalid PESEL. Try again: ");
        flush_input();
        scanf(" %11s", new_acc.pesel);
    }

    new_acc.balance = 0.0;
    new_acc.loan = 0.0;
    accounts[account_count++] = new_acc;
    printf("Account created successfully.\n");

    return 0;
}

// NOTE: may become obsolete once all moved to .txt operations, no caching
void save_accounts_to_file(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("Failed to save accounts.\n");
        return;
    }

    for (int i = 0; i < account_count; ++i)
    {
        Account a = accounts[i];
        fprintf(file, "%d|%s|%s|%s|%s|%.2f|%.2f\n",
                a.account_number, a.name, a.surname, a.address, a.pesel, a.balance, a.loan);
    }

    fclose(file);
}

Account *load_account_from_file(int account_number) {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        printf("[E] No existing account data found. Starting fresh.\n");
        return NULL;
    }

    Account *account = malloc(sizeof(Account));
    if (!account) {
        printf("[E] Memory allocation failed.\n");
        fclose(file);
        return NULL;
    }

    while (fscanf(file, "%d|%49[^|]|%49[^|]|%199[^|]|%11[^|]|%lf|%lf\n",
                  &account->account_number, account->name, account->surname,
                  account->address, account->pesel, &account->balance, &account->loan) == 7) {
        if (account->account_number == account_number) {
            printf("[D] Found!\tAccount #%d: %s %s, PESEL: %s, Balance: %.2f, Loan: %.2f\n",
                   account->account_number, account->name, account->surname,
                   account->pesel, account->balance, account->loan);
            fclose(file);
            return account;
        }
    }

    free(account);
    fclose(file);
    return NULL;
}

void list_accounts()
{
    printf("\n--- List of Accounts ---\n");

    FILE *file = fopen(FILENAME, "r");
    if (!file)
    {
        printf("[E] No existing account data found. Starting fresh.\n");
        return;
    }

    Account a;
    while (fscanf(file, "%d|%49[^|]|%49[^|]|%199[^|]|%11[^|]|%lf|%lf\n",
                  &a.account_number, a.name, a.surname, a.address,
                  a.pesel, &a.balance, &a.loan) == 7)
    {
        printf("Account #%d: %s %s, PESEL: %s, Balance: %.2f, Loan: %.2f\n",
            a.account_number, a.name, a.surname, a.pesel, a.balance, a.loan);
    }

    fclose(file);

    printf("\n--- End ---\n");
}

// TODO: refactor to use read from file
Account *find_account_by_number(int number) {
    return load_account_from_file(number);
}

int update_account_in_file(const char *filename, const Account *updated_acc) {
    FILE *input = fopen(filename, "r");
    FILE *temp = fopen("temp_accounts.txt", "w");
    if (!input || !temp) {
        printf("[E] Failed to open files.\n");
        if (input) fclose(input);
        if (temp) fclose(temp);
        return 0;
    }

    Account a;
    int updated = 0;
    while (fscanf(input, "%d|%49[^|]|%49[^|]|%199[^|]|%11[^|]|%lf|%lf\n",
                  &a.account_number, a.name, a.surname, a.address,
                  a.pesel, &a.balance, &a.loan) == 7)
    {
        if (a.account_number == updated_acc->account_number) {
            fprintf(temp, "%d|%s|%s|%s|%s|%.2lf|%.2lf\n",
                    updated_acc->account_number, updated_acc->name,
                    updated_acc->surname, updated_acc->address,
                    updated_acc->pesel, updated_acc->balance,
                    updated_acc->loan);
            updated = 1;
        } else {
            fprintf(temp, "%d|%s|%s|%s|%s|%.2lf|%.2lf\n",
                    a.account_number, a.name, a.surname, a.address,
                    a.pesel, a.balance, a.loan);
        }
    }

    fclose(input);
    fclose(temp);

    if (updated) {
        remove(filename);
        rename("temp_accounts.txt", filename);
    } else {
        remove("temp_accounts.txt");
    }

    return updated;
}

void search_accounts()
{
    int option;
    char input[200];
    printf("\nSearch by:\n");
    printf("1. Account Number\n");
    printf("2. Name\n");
    printf("3. Surname\n");
    printf("4. Address\n");
    printf("5. PESEL\n");
    printf("Choice: ");
    scanf("%d", &option);
    flush_input();

    int found = 0;

    switch (option)
    {
    case 1:
    {
        int acc_num = -1;
        while (acc_num == -1)
        {
            printf("Enter account number: ");
            acc_num = read_int();
        }

        for (int i = 0; i < account_count; ++i)
        {
            if (accounts[i].account_number == acc_num)
            {
                Account a = accounts[i];
                printf("Found: #%d: %s %s, PESEL: %s, Balance: %.2f, Loan: %.2f\n",
                       a.account_number, a.name, a.surname, a.pesel, a.balance, a.loan);
                found = 1;
            }
        }
        break;
    }
    case 2:
        printf("Enter name: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        for (int i = 0; i < account_count; ++i)
        {
            if (strcasecmp(accounts[i].name, input) == 0)
            {
                Account a = accounts[i];
                printf("Found: #%d: %s %s, PESEL: %s, Balance: %.2f, Loan: %.2f\n",
                       a.account_number, a.name, a.surname, a.pesel, a.balance, a.loan);
                found = 1;
            }
        }
        break;
    case 3:
        printf("Enter surname: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        for (int i = 0; i < account_count; ++i)
        {
            if (strcasecmp(accounts[i].surname, input) == 0)
            {
                Account a = accounts[i];
                printf("Found: #%d: %s %s, PESEL: %s, Balance: %.2f, Loan: %.2f\n",
                       a.account_number, a.name, a.surname, a.pesel, a.balance, a.loan);
                found = 1;
            }
        }
        break;
    case 4:
        printf("Enter address: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        for (int i = 0; i < account_count; ++i)
        {
            if (strcasecmp(accounts[i].address, input) == 0)
            {
                Account a = accounts[i];
                printf("Found: #%d: %s %s, PESEL: %s, Balance: %.2f, Loan: %.2f\n",
                       a.account_number, a.name, a.surname, a.pesel, a.balance, a.loan);
                found = 1;
            }
        }
        break;
    case 5:
        printf("Enter PESEL: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        for (int i = 0; i < account_count; ++i)
        {
            if (strcmp(accounts[i].pesel, input) == 0)
            {
                Account a = accounts[i];
                printf("Found: #%d: %s %s, PESEL: %s, Balance: %.2f, Loan: %.2f\n",
                       a.account_number, a.name, a.surname, a.pesel, a.balance, a.loan);
                found = 1;
            }
        }
        break;
    default:
        printf("Invalid option.\n");
        return;
    }

    if (!found)
    {
        printf("No matching accounts found.\n");
    }
}

bool confirm_operation(const char *operation)
{
    printf("You are about to %s. Are you sure? (y/n): ", operation);
    char response;
    scanf(" %c", &response);
    flush_input();
    return (response == 'y' || response == 'Y');
}

void deposit() {
    int num = -1;
    while (num == -1) {
        printf("Enter account number: ");
        num = read_int();
    }

    Account *acc = find_account_by_number(num);
    if (!acc) {
        printf("Account not found.\n");
        goto exit;
    }

    double amt;
    printf("Enter amount to deposit: ");
    scanf("%lf", &amt);
    if (amt <= 0) {
        printf("Invalid amount.\n");
        goto exit;
    }

    if (!confirm_operation("make a deposit")) {
        printf("Deposit cancelled.\n");
        goto exit;
    }

    acc->balance += amt;

    if (update_account_in_file(FILENAME, acc)) {
        printf("Deposit successful. New balance: %.2f\n", acc->balance);
    } else {
        printf("[E] Failed to deposit!\n");
    }

exit:
    free(acc);
    return;
}

void withdraw()
{
    int num = -1;
    while (num == -1)
    {
        printf("Enter account number: ");
        num = read_int();
    }

    Account *acc = find_account_by_number(num);
    if (!acc)
    {
        printf("Account not found.\n");
        return;
    }

    double amt;
    printf("Enter amount to withdraw: ");
    scanf("%lf", &amt);
    if (!is_valid_amount(amt) || amt > acc->balance)
    {
        printf("Invalid or insufficient funds.\n");
        return;
    }

    if (!confirm_operation("make a withdrawal"))
    {
        printf("Withdrawal cancelled.\n");
        return;
    }

    acc->balance -= amt;
    if (update_account_in_file(FILENAME, acc)) {
        printf("Withdrawal successful. New balance: %.2f\n", acc->balance);
    } else {
        printf("[E] Failed to withdraw!\n");
    }

    free(acc);
}

void log_transfer(int from, int to, double amount)
{
    FILE *file = fopen("transfers.txt", "a");
    if (!file)
    {
        printf("Failed to log transfer.\n");
        return;
    }
    fprintf(file, "Transfer from %d to %d: %.2f\n", from, to, amount);
    fclose(file);
}

void transfer()
{
    int from = -1;
    while (from == -1)
    {
        printf("Enter sender account number: ");
        from = read_int();
    }

    int to = -1;
    while (to == -1)
    {
        printf("Enter receiver account number: ");
        to = read_int();
    }

    Account *acc1 = find_account_by_number(from);
    Account *acc2 = find_account_by_number(to);
    if (!acc1 || !acc2)
    {
        printf("One of the accounts not found.\n");
        return;
    }

    double amt;
    printf("Enter amount to transfer: ");
    scanf("%lf", &amt);
    if (!is_valid_amount(amt) || amt > acc1->balance)
    {
        printf("Invalid or insufficient funds.\n");
        return;
    }

    if (!confirm_operation("make a transfer"))
    {
        printf("Transfer cancelled.\n");
        return;
    }

    acc1->balance -= amt;
    acc2->balance += amt;
    log_transfer(from, to, amt);

    if (!update_account_in_file(FILENAME, acc1)) {
        printf("[E] Failed to transfer acc1!\n");
        // TODO: handle free()
    }

    if (!update_account_in_file(FILENAME, acc2)) {
        printf("[E] Failed to transfer acc2!\n");
        // TODO: handle free()
    }

    free(acc1);
    free(acc2);
    printf("Transfer successful.\n");
}

void take_loan()
{
    int num = -1;
    while (num == -1)
    {
        printf("Enter account number: ");
        num = read_int();
    }

    Account *acc = find_account_by_number(num);
    if (!acc)
    {
        printf("Account not found.\n");
        return;
    }

    double amt, rate;
    printf("Enter loan amount: ");
    scanf("%lf", &amt);
    printf("Enter interest rate (0-100): ");
    scanf("%lf", &rate);

    if (!is_valid_amount(amt) || !is_valid_interest(rate))
    {
        printf("Invalid input.\n");
        return;
    }

    if (!confirm_operation("take a loan"))
    {
        printf("Loan cancelled.\n");
        return;
    }

    double total = amt + (amt * rate / 100);
    acc->loan += total;
    acc->balance += amt;

    if (update_account_in_file(FILENAME, acc)) {
        printf("Loan granted. Total debt: %.2f. New balance: %.2f\n", acc->loan, acc->balance);
    } else {
        printf("[E] Failed to take a loan!\n");
    }

    free(acc);
}

void pay_debt()
{
    int num = -1;
    while (num == -1)
    {
        printf("Enter account number: ");
        num = read_int();
    }

    Account *acc = find_account_by_number(num);
    if (!acc)
    {
        printf("Account not found.\n");
        return;
    }

    if (acc->loan == 0)
    {
        printf("No debt to pay.\n");
        return;
    }

    if (!confirm_operation("pay debt"))
    {
        printf("Debt payment cancelled.\n");
        return;
    }

    if (acc->balance >= acc->loan)
    {
        acc->balance -= acc->loan;
        acc->loan = 0;
        printf("Debt paid in full. Remaining balance: %.2f\n", acc->balance);
    }
    else
    {
        printf("Insufficient funds to pay debt. You owe: %.2f\n", acc->loan);
    }
}

void menu()
{
    int choice;
    do
    {
        printf("\n--- Bank Menu ---\n");
        printf("1. Create Account\n2. List Accounts\n3. Search Account\n4. Deposit\n");
        printf("5. Withdraw\n6. Transfer\n7. Take Loan\n8. Pay Debt\n0. Exit\n");
        printf("Choice: ");
        choice = read_int();

        switch (choice)
        {
        case 1:
            if (create_account())
                printf("[E] Failed to create account\n");
            break;
        case 2:
            list_accounts(FILENAME);
            break;
        case 3:
            search_accounts();
            break;
        case 4:
            deposit();
            break;
        case 5:
            withdraw();
            break;
        case 6:
            transfer();
            break;
        case 7:
            take_loan();
            break;
        case 8:
            pay_debt();
            break;
        case 0:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 0);
}

int main()
{
    menu();

    save_accounts_to_file(FILENAME);
    return 0;
}