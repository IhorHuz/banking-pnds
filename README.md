# 💳 Banking System in C

A simple command-line banking system written in C. This project allows users to create, manage, and interact with bank accounts via terminal input.

## 📌 Features

- Create a new bank account
- View all existing accounts
- Search for an account by:
  - Account number
  - Name
  - Surname
  - Address
  - PESEL
- Deposit and withdraw money
- Transfer funds between accounts
- Take out and repay loans
- Save and load account data from a file
- Binary search for efficient lookups

## 🛠️ Technologies

- C (ISO C99)
- Standard Library (`stdio.h`, `stdlib.h`, `string.h`)
- File I/O
- Data structures (structs, arrays)
- Git for version control

## ⚙️ Compilation

To compile the project:

```bash
gcc -Wall -Wextra -g banking.c -o banking

To run:
./banking
