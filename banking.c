#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

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

void flush_input()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void ensure_account_capacity()
{
    if (account_count >= accounts_capacity)
    {
        int new_capacity = (accounts_capacity == 0) ? 10 : accounts_capacity * 2;
        Account *new_accounts = realloc(accounts, new_capacity * sizeof(Account));
        if (!new_accounts)
        {
            printf("Memory allocation failed. Cannot create more accounts.\n");
            return;
        }
        accounts = new_accounts;
        accounts_capacity = new_capacity;
    }
}

void create_account()
{
    ensure_account_capacity();
    if (account_count >= accounts_capacity)
    {
        printf("Cannot create more accounts due to memory constraints.\n");
        return;
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
}

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

void load_accounts_from_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("No existing account data found. Starting fresh.\n");
        return;
    }

    Account temp;
    while (fscanf(file, "%d|%49[^|]|%49[^|]|%199[^|]|%11[^|]|%lf|%lf\n",
                  &temp.account_number, temp.name, temp.surname, temp.address,
                  temp.pesel, &temp.balance, &temp.loan) == 7)
    {
        ensure_account_capacity();
        if (account_count >= accounts_capacity)
        {
            printf("Warning: Not all accounts loaded due to memory constraints.\n");
            break;
        }

        accounts[account_count++] = temp;
        if (temp.account_number >= next_account_number)
        {
            next_account_number = temp.account_number + 1;
        }
    }

    fclose(file);
}

void list_accounts()
{
    printf("\n--- List of Accounts ---\n");
    if (account_count == 0)
    {
        printf("No accounts to display.\n");
        return;
    }

    for (int i = 0; i < account_count; ++i)
    {
        Account a = accounts[i];
        printf("Account #%d: %s %s, PESEL: %s, Balance: %.2f, Loan: %.2f\n",
               a.account_number, a.name, a.surname, a.pesel, a.balance, a.loan);
    }
}

Account *find_account_by_number(int number)
{
    for (int i = 0; i < account_count; ++i)
        if (accounts[i].account_number == number)
            return &accounts[i];
    return NULL;
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

void deposit()
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
    printf("Enter amount to deposit: ");
    scanf("%lf", &amt);
    if (!is_valid_amount(amt))
    {
        printf("Invalid amount.\n");
        return;
    }

    if (!confirm_operation("make a deposit"))
    {
        printf("Deposit cancelled.\n");
        return;
    }

    acc->balance += amt;
    printf("Deposit successful. New balance: %.2f\n", acc->balance);
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
    printf("Withdrawal successful. New balance: %.2f\n", acc->balance);
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
    printf("Loan granted. Total debt: %.2f. New balance: %.2f\n", acc->loan, acc->balance);
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

void free_accounts()
{
    free(accounts);
    accounts = NULL;
    account_count = 0;
    accounts_capacity = 0;
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
            create_account();
            break;
        case 2:
            list_accounts();
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
    load_accounts_from_file("accounts.txt");

    menu();

    save_accounts_to_file("accounts.txt");
    free_accounts();
    return 0;
}