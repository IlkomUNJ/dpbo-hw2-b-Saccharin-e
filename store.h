#ifndef STORE_H
#define STORE_H

#include "transaction.h"
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <iomanip>

using namespace std;

class Store {
private:
    vector<Transaction> transactions;
    int nextTransactionId;

public:
    Store() : nextTransactionId(1) {}

    void addTransaction(const Transaction& trans) {
        transactions.push_back(trans);
    }

    int getNextTransactionId() {
        return nextTransactionId++;
    }

    int getTransactionCount() const {
        return transactions.size();
    }

    // List all transactions of the latest k days
    void listTransactionsLastKDays(int k) const {
        cout << "\n=== Transactions in Last " << k << " Days ===" << endl;
        cout << setw(8) << "ID" << setw(15) << "Date" << setw(15) << "Buyer" 
             << setw(15) << "Seller" << setw(20) << "Item" 
             << setw(12) << "Amount" << setw(12) << "Status" << endl;
        cout << string(97, '-') << endl;

        bool found = false;
        for (const auto& trans : transactions) {
            if (trans.isWithinDays(k) && trans.getType() == PURCHASE) {
                cout << setw(8) << trans.getId()
                     << setw(15) << trans.getDateOnlyString()
                     << setw(15) << trans.getBuyerName()
                     << setw(15) << trans.getSellerName()
                     << setw(20) << trans.getItemName()
                     << "  $" << setw(9) << fixed << setprecision(2) << trans.getAmount()
                     << setw(12) << trans.getStatusString() << endl;
                found = true;
            }
        }
        if (!found) {
            cout << "No transactions found." << endl;
        }
        cout << endl;
    }

    // List all paid transactions but yet to be completed
    void listPaidNotCompleted() const {
        cout << "\n=== Paid But Not Completed Transactions ===" << endl;
        cout << setw(8) << "ID" << setw(15) << "Date" << setw(15) << "Buyer" 
             << setw(15) << "Seller" << setw(20) << "Item" 
             << setw(12) << "Amount" << endl;
        cout << string(85, '-') << endl;

        bool found = false;
        for (const auto& trans : transactions) {
            if (trans.getStatus() == PAID && trans.getType() == PURCHASE) {
                cout << setw(8) << trans.getId()
                     << setw(15) << trans.getDateOnlyString()
                     << setw(15) << trans.getBuyerName()
                     << setw(15) << trans.getSellerName()
                     << setw(20) << trans.getItemName()
                     << "  $" << setw(9) << fixed << setprecision(2) << trans.getAmount() << endl;
                found = true;
            }
        }
        if (!found) {
            cout << "No pending transactions found." << endl;
        }
        cout << endl;
    }

    // List all most m frequent item transactions
    void listMostFrequentItems(int m) const {
        cout << "\n=== Top " << m << " Most Frequent Items ===" << endl;
        
        map<int, pair<string, int>> itemCount;
        for (const auto& trans : transactions) {
            if (trans.getType() == PURCHASE && trans.getStatus() != CANCELLED) {
                itemCount[trans.getItemId()].first = trans.getItemName();
                itemCount[trans.getItemId()].second += trans.getQuantity();
            }
        }

        vector<pair<string, int>> items;
        for (const auto& pair : itemCount) {
            items.push_back({pair.second.first, pair.second.second});
        }

        sort(items.begin(), items.end(), 
             [](const pair<string, int>& a, const pair<string, int>& b) {
                 return a.second > b.second;
             });

        cout << setw(30) << "Item Name" << setw(15) << "Total Sold" << endl;
        cout << string(45, '-') << endl;

        int count = 0;
        for (const auto& item : items) {
            if (count >= m) break;
            cout << setw(30) << item.first << setw(15) << item.second << endl;
            count++;
        }
        if (count == 0) {
            cout << "No items found." << endl;
        }
        cout << endl;
    }

    // List all most active buyers counted by number of transactions per day
    void listMostActiveBuyers(int n) const {
        cout << "\n=== Top " << n << " Most Active Buyers (Today) ===" << endl;
        
        map<int, pair<string, int>> buyerCount;
        for (const auto& trans : transactions) {
            if (trans.isToday() && trans.getType() == PURCHASE) {
                buyerCount[trans.getBuyerId()].first = trans.getBuyerName();
                buyerCount[trans.getBuyerId()].second++;
            }
        }

        vector<pair<string, int>> buyers;
        for (const auto& pair : buyerCount) {
            buyers.push_back({pair.second.first, pair.second.second});
        }

        sort(buyers.begin(), buyers.end(),
             [](const pair<string, int>& a, const pair<string, int>& b) {
                 return a.second > b.second;
             });

        cout << setw(20) << "Buyer Name" << setw(20) << "Transactions Today" << endl;
        cout << string(40, '-') << endl;

        int count = 0;
        for (const auto& buyer : buyers) {
            if (count >= n) break;
            cout << setw(20) << buyer.first << setw(20) << buyer.second << endl;
            count++;
        }
        if (count == 0) {
            cout << "No active buyers today." << endl;
        }
        cout << endl;
    }

    // List all most active sellers counted by number of transactions per day
    void listMostActiveSellers(int n) const {
        cout << "\n=== Top " << n << " Most Active Sellers (Today) ===" << endl;
        
        map<int, pair<string, int>> sellerCount;
        for (const auto& trans : transactions) {
            if (trans.isToday() && trans.getType() == PURCHASE) {
                sellerCount[trans.getSellerId()].first = trans.getSellerName();
                sellerCount[trans.getSellerId()].second++;
            }
        }

        vector<pair<string, int>> sellers;
        for (const auto& pair : sellerCount) {
            sellers.push_back({pair.second.first, pair.second.second});
        }

        sort(sellers.begin(), sellers.end(),
             [](const pair<string, int>& a, const pair<string, int>& b) {
                 return a.second > b.second;
             });

        cout << setw(20) << "Seller Name" << setw(20) << "Transactions Today" << endl;
        cout << string(40, '-') << endl;

        int count = 0;
        for (const auto& seller : sellers) {
            if (count >= n) break;
            cout << setw(20) << seller.first << setw(20) << seller.second << endl;
            count++;
        }
        if (count == 0) {
            cout << "No active sellers today." << endl;
        }
        cout << endl;
    }

    // Get transaction by ID
    Transaction* getTransactionById(int id) {
        for (auto& trans : transactions) {
            if (trans.getId() == id) {
                return &trans;
            }
        }
        return nullptr;
    }

    // Get all transactions
    const vector<Transaction>& getAllTransactions() const {
        return transactions;
    }
};

#endif // STORE_H
