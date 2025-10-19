#ifndef BUYER_H
#define BUYER_H

#include <cstddef>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include "bank_customer.h"

using namespace std;

// Forward declaration
class Transaction;

class Buyer {
private:
    int id;
    string name;
    BankCustomer &account; // Association with BankCustomer

public:
    Buyer(int id, const string& name, BankCustomer &account0)
        : id(id), name(name), account(account0) {}

    int getId() const { return id; }
    string getName() const { return name; }
    BankCustomer& getAccount() { return account; }

    void setId(int newId) { id = newId; }
    void setName(const std::string& newName) { name = newName; }

    // List orders with filters
    void listOrders(const vector<Transaction>& allTransactions, int filterStatus = -1) const;

    // Check spending for last k days
    void checkSpendingLastKDays(const vector<Transaction>& allTransactions, int k) const;
};

#endif // BUYER_H