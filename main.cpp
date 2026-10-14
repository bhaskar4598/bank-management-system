// Bank Management Application - C++ (OOP + File Handling)
// OOP concepts: Encapsulation, Inheritance, Polymorphism (abstract class, virtual functions)
// Features: Create account, Login (PIN), Deposit, Withdraw, Balance check
// Data is stored in accounts.txt (PIN is stored as a hash, not plain text)

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <string>
#include <limits>
#include <iomanip>
using namespace std;

const string FILE_NAME = "accounts.txt";

// ---------- Helper functions ----------
unsigned long long hashPin(const string &pin) {
    // djb2 hash with a salt (simple, for learning purposes)
    unsigned long long h = 5381;
    string salted = "bank_salt_" + pin;
    for (char c : salted) h = ((h << 5) + h) + (unsigned char)c;
    return h;
}

int readInt(const string &prompt) {
    int x;
    while (true) {
        cout << prompt;
        if (cin >> x) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return x;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input! Number daalo.\n";
    }
}

double readAmount(const string &prompt) {
    double x;
    while (true) {
        cout << prompt;
        if (cin >> x && x > 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return x;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid amount! Amount 0 se zyada hona chahiye.\n";
    }
}

string readLine(const string &prompt) {
    string s;
    do {
        cout << prompt;
        getline(cin, s);
    } while (s.empty());
    for (char &c : s) if (c == '|') c = '-';  // '|' is the file separator
    return s;
}

string readPin() {
    string pin;
    while (true) {
        cout << "4-digit PIN: ";
        getline(cin, pin);
        bool ok = pin.size() == 4;
        for (char c : pin) if (!isdigit((unsigned char)c)) ok = false;
        if (ok) return pin;
        cout << "PIN exactly 4 digits ka hona chahiye.\n";
    }
}

// ---------- Base class (abstract) ----------
class Account {
protected:
    int accNo;
    string name;
    unsigned long long pinHash;
    double balance;

public:
    Account(int no, const string &n, unsigned long long ph, double bal)
        : accNo(no), name(n), pinHash(ph), balance(bal) {}
    virtual ~Account() {}

    // Polymorphism: each account type defines its own rules
    virtual string type() const = 0;
    virtual double minBalance() const = 0;

    int getAccNo() const { return accNo; }
    string getName() const { return name; }
    double getBalance() const { return balance; }
    bool checkPin(const string &pin) const { return pinHash == hashPin(pin); }

    void deposit(double amt) {
        balance += amt;
        cout << "Rs. " << fixed << setprecision(2) << amt << " deposit ho gaye.\n";
    }

    bool withdraw(double amt) {
        if (balance - amt < minBalance()) {
            cout << "Insufficient balance! Minimum Rs. " << minBalance()
                 << " account mein rehna zaroori hai.\n";
            return false;
        }
        balance -= amt;
        cout << "Rs. " << fixed << setprecision(2) << amt << " withdraw ho gaye.\n";
        return true;
    }

    void showBalance() const {
        cout << "Account No: " << accNo << " | " << name << " (" << type() << ")\n"
             << "Current Balance: Rs. " << fixed << setprecision(2) << balance << '\n';
    }

    string serialize() const {
        ostringstream ss;
        ss << type() << '|' << accNo << '|' << name << '|' << pinHash << '|'
           << fixed << setprecision(2) << balance;
        return ss.str();
    }
};

// ---------- Derived classes (Inheritance) ----------
class SavingsAccount : public Account {
public:
    using Account::Account;
    string type() const override { return "Savings"; }
    double minBalance() const override { return 500.0; }
};

class CurrentAccount : public Account {
public:
    using Account::Account;
    string type() const override { return "Current"; }
    double minBalance() const override { return 0.0; }
};

// ---------- Bank class (manages all accounts + file handling) ----------
class Bank {
    vector<unique_ptr<Account>> accounts;

public:
    Bank() { load(); }

    void load() {
        accounts.clear();
        ifstream in(FILE_NAME);
        if (!in) return;
        string line;
        while (getline(in, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string type, noS, name, hashS, balS;
            getline(ss, type, '|');
            getline(ss, noS, '|');
            getline(ss, name, '|');
            getline(ss, hashS, '|');
            getline(ss, balS, '|');
            try {
                int no = stoi(noS);
                unsigned long long h = stoull(hashS);
                double bal = stod(balS);
                if (type == "Savings")
                    accounts.push_back(make_unique<SavingsAccount>(no, name, h, bal));
                else if (type == "Current")
                    accounts.push_back(make_unique<CurrentAccount>(no, name, h, bal));
            } catch (...) {
                // skip corrupted line
            }
        }
    }

    void save() const {
        ofstream out(FILE_NAME);
        for (const auto &a : accounts) out << a->serialize() << '\n';
    }

    Account *find(int accNo) {
        for (auto &a : accounts)
            if (a->getAccNo() == accNo) return a.get();
        return nullptr;
    }

    int nextAccNo() const {
        int mx = 1000;
        for (const auto &a : accounts) mx = max(mx, a->getAccNo());
        return mx + 1;
    }

    void createAccount() {
        cout << "\n--- Naya Account ---\n";
        string name = readLine("Customer name: ");
        cout << "Account type: 1. Savings (min Rs.500)  2. Current\n";
        int t;
        do { t = readInt("Choice: "); } while (t != 1 && t != 2);
        string pin = readPin();
        double initial = 0;
        while (true) {
            cout << "Initial deposit: ";
            if (cin >> initial && initial >= 0) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (t == 1 && initial < 500) {
                    cout << "Savings account ke liye minimum Rs. 500 chahiye.\n";
                    continue;
                }
                break;
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid amount!\n";
        }
        int no = nextAccNo();
        if (t == 1)
            accounts.push_back(make_unique<SavingsAccount>(no, name, hashPin(pin), initial));
        else
            accounts.push_back(make_unique<CurrentAccount>(no, name, hashPin(pin), initial));
        save();
        cout << "Account ban gaya! Aapka Account Number: " << no << " (isse yaad rakho)\n";
    }

    void login() {
        int no = readInt("\nAccount number: ");
        Account *acc = find(no);
        cout << "PIN: ";
        string pin;
        getline(cin, pin);
        if (!acc || !acc->checkPin(pin)) {
            cout << "Account number ya PIN galat hai.\n";
            return;
        }
        cout << "\nWelcome, " << acc->getName() << "!\n";
        accountMenu(*acc);
    }

    void accountMenu(Account &acc) {
        int choice;
        do {
            cout << "\n--- Account Menu ---\n"
                 << "1. Deposit\n2. Withdraw\n3. Check Balance\n4. Logout\n";
            choice = readInt("Choice (1-4): ");
            switch (choice) {
                case 1: acc.deposit(readAmount("Amount: ")); save(); break;
                case 2:
                    if (acc.withdraw(readAmount("Amount: "))) save();
                    break;
                case 3: acc.showBalance(); break;
                case 4: cout << "Logged out.\n"; break;
                default: cout << "Galat choice!\n";
            }
        } while (choice != 4);
    }
};

// ---------- Main ----------
int main() {
    Bank bank;
    int choice;
    do {
        cout << "\n===== BANK MANAGEMENT SYSTEM =====\n"
             << "1. Create Account\n2. Login\n3. Exit\n";
        choice = readInt("Choice (1-3): ");
        switch (choice) {
            case 1: bank.createAccount(); break;
            case 2: bank.login(); break;
            case 3: cout << "Thank you! Data accounts.txt mein save hai.\n"; break;
            default: cout << "Galat choice, dobara try karo.\n";
        }
    } while (choice != 3);
    return 0;
}
