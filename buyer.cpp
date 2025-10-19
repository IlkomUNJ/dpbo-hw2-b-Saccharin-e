#include "buyer.h"
#include "transaction.h"
#include <iostream>
#include <iomanip>

using namespace std;

void Buyer::listOrders(const vector<Transaction>& allTransactions, int filterStatus) const {
    cout << "\n=== My Orders ===" << endl;
    cout << setw(8) << "ID" << setw(15) << "Date" << setw(15) << "Seller" 
         << setw(20) << "Item" << setw(10) << "Qty"
         << setw(12) << "Amount" << setw(12) << "Status" << endl;
    cout << string(92, '-') << endl;

    bool found = false;
    for (const auto& trans : allTransactions) {
        if (trans.getBuyerId() == this->id && trans.getType() == PURCHASE) {
            // Apply filter if specified
            if (filterStatus != -1 && trans.getStatus() != filterStatus) {
                continue;
            }

            cout << setw(8) << trans.getId()
                 << setw(15) << trans.getDateOnlyString()
                 << setw(15) << trans.getSellerName()
                 << setw(20) << trans.getItemName()
                 << setw(10) << trans.getQuantity()
                 << "  $" << setw(9) << fixed << setprecision(2) << trans.getAmount()
                 << setw(12) << trans.getStatusString() << endl;
            found = true;
        }
    }

    if (!found) {
        cout << "No orders found." << endl;
    }
    cout << endl;
}

void Buyer::checkSpendingLastKDays(const vector<Transaction>& allTransactions, int k) const {
    cout << "\n=== Spending in Last " << k << " Days ===" << endl;
    
    double totalSpending = 0.0;
    int orderCount = 0;

    for (const auto& trans : allTransactions) {
        if (trans.getBuyerId() == this->id && 
            trans.getType() == PURCHASE && 
            trans.getStatus() != CANCELLED &&
            trans.isWithinDays(k)) {
            totalSpending += trans.getAmount();
            orderCount++;
        }
    }

    cout << "Total Orders: " << orderCount << endl;
    cout << "Total Spending: $" << fixed << setprecision(2) << totalSpending << endl;
    cout << endl;
}
