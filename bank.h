#ifndef BANK_H
#define BANK_H

#include "bank_customer.h"
#include "transaction.h"
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>

using namespace std;

class Bank{
private:
    string name;
    vector<BankCustomer> Accounts{};
    int customerCount;
    vector<Transaction> bankTransactions;

public:
    Bank(const string& name){
        this->name = name;
        this->customerCount = 0;
    }

    void addBankTransaction(const Transaction& trans) {
        bankTransactions.push_back(trans);
    }

    // List all transactions within a week starting from now backwards
    void listTransactionsLastWeek() const {
        cout << "\n=== Bank Transactions (Last 7 Days) ===" << endl;
        cout << setw(8) << "ID" << setw(20) << "Date" << setw(20) << "Customer" 
             << setw(15) << "Type" << setw(15) << "Amount" << endl;
        cout << string(78, '-') << endl;

        bool found = false;
        for (const auto& trans : bankTransactions) {
            if (trans.isWithinDays(7)) {
                cout << setw(8) << trans.getId()
                     << setw(20) << trans.getDateString()
                     << setw(20) << trans.getBuyerName()
                     << setw(15) << trans.getTypeString()
                     << "  $" << setw(12) << fixed << setprecision(2) << trans.getAmount() << endl;
                found = true;
            }
        }
        if (!found) {
            cout << "No transactions in the last 7 days." << endl;
        }
        cout << endl;
    }

    // List all bank customers
    void listAllCustomers(const vector<BankCustomer>& customers) const {
        cout << "\n=== All Bank Customers ===" << endl;
        cout << setw(8) << "ID" << setw(20) << "Name" << setw(15) << "Balance" << endl;
        cout << string(43, '-') << endl;

        for (const auto& customer : customers) {
            cout << setw(8) << customer.getId()
                 << setw(20) << customer.getName()
                 << "  $" << setw(12) << fixed << setprecision(2) << customer.getBalance() << endl;
        }
        if (customers.empty()) {
            cout << "No customers found." << endl;
        }
        cout << endl;
    }

    // List all dormant accounts (no transaction within a month)
    void listDormantAccounts(const vector<BankCustomer>& customers) const {
        cout << "\n=== Dormant Accounts (No transaction in 30 days) ===" << endl;
        cout << setw(8) << "ID" << setw(20) << "Name" << setw(15) << "Balance" << endl;
        cout << string(43, '-') << endl;

        map<int, time_t> lastTransactionTime;
        for (const auto& trans : bankTransactions) {
            int customerId = trans.getBuyerId();
            if (lastTransactionTime.find(customerId) == lastTransactionTime.end() ||
                trans.getTimestamp() > lastTransactionTime[customerId]) {
                lastTransactionTime[customerId] = trans.getTimestamp();
            }
        }

        time_t now = time(nullptr);
        bool found = false;

        for (const auto& customer : customers) {
            int customerId = customer.getId();
            if (lastTransactionTime.find(customerId) == lastTransactionTime.end()) {
                // No transactions at all
                cout << setw(8) << customer.getId()
                     << setw(20) << customer.getName()
                     << "  $" << setw(12) << fixed << setprecision(2) << customer.getBalance() << endl;
                found = true;
            } else {
                time_t lastTime = lastTransactionTime[customerId];
                double daysSince = difftime(now, lastTime) / (60 * 60 * 24);
                if (daysSince > 30) {
                    cout << setw(8) << customer.getId()
                         << setw(20) << customer.getName()
                         << "  $" << setw(12) << fixed << setprecision(2) << customer.getBalance() << endl;
                    found = true;
                }
            }
        }

        if (!found) {
            cout << "No dormant accounts found." << endl;
        }
        cout << endl;
    }

    // List n top users that conduct most transactions for today
    void listTopUsersToday(int n) const {
        cout << "\n=== Top " << n << " Users by Transactions Today ===" << endl;
        
        map<int, pair<string, int>> userCount;
        for (const auto& trans : bankTransactions) {
            if (trans.isToday()) {
                userCount[trans.getBuyerId()].first = trans.getBuyerName();
                userCount[trans.getBuyerId()].second++;
            }
        }

        vector<pair<string, int>> users;
        for (const auto& pair : userCount) {
            users.push_back({pair.second.first, pair.second.second});
        }

        sort(users.begin(), users.end(),
             [](const pair<string, int>& a, const pair<string, int>& b) {
                 return a.second > b.second;
             });

        cout << setw(20) << "Customer Name" << setw(20) << "Transactions Today" << endl;
        cout << string(40, '-') << endl;

        int count = 0;
        for (const auto& user : users) {
            if (count >= n) break;
            cout << setw(20) << user.first << setw(20) << user.second << endl;
            count++;
        }
        if (count == 0) {
            cout << "No transactions today." << endl;
        }
        cout << endl;
    }
};

#endif // BANK_H