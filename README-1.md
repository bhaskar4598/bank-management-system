# Bank Management Application (C++)

A console-based banking system in C++ that simulates core banking operations using object-oriented programming and file handling.

## Features
- Create Savings / Current accounts (auto-generated account number)
- PIN-protected login (PIN is stored as a salted hash, never in plain text)
- Deposit, withdraw and balance check
- Savings account enforces a minimum balance of Rs. 500
- Persistent storage in `accounts.txt`

## OOP concepts used
- **Encapsulation** – account data is `protected`, accessed through methods
- **Abstraction** – `Account` is an abstract base class
- **Inheritance** – `SavingsAccount` and `CurrentAccount` derive from `Account`
- **Polymorphism** – virtual `type()` and `minBalance()` overridden per account type

## How to run
```bash
g++ -std=c++14 main.cpp -o bank_app
./bank_app        # Windows: bank_app.exe
```

## Note
The PIN hashing is a simple educational implementation. A real banking system would use a proper algorithm such as bcrypt or Argon2.
