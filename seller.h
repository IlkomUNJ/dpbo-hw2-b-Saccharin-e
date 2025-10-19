#pragma once
#include "buyer.h"
#include "item.h"
#include "transaction.h"
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <iomanip>

using namespace std;

class seller : public Buyer {

private:
    // Add seller-specific private members here
    int sellerId;
    std::string sellerName;
    bool idDisplayed(int itemId) const {
        // Example implementation, can be customized
        return itemId > 0; // Assuming valid IDs are positive integers
    }

    vector<Item> items; // Assuming seller has a collection of items


public:
    seller() = default;

    seller(Buyer buyer, int sellerId, const std::string& sellerName)
        : Buyer(buyer.getId(), buyer.getName(), buyer.getAccount()), sellerId(sellerId), sellerName(sellerName) {
            Buyer::setId(buyer.getId());
        }

    virtual ~seller() = default;

    void addNewItem(int newId, const std::string& newName, int newQuantity, double newPrice) {
        Item newItem(newId, newName, newQuantity, newPrice);
        items.push_back(newItem);
    }

    void updateItem(int itemId, const std::string& newName, int newQuantity, double newPrice) {
        for (auto& item : items) {
            if (item.getId() == itemId) {
                item.alterItemById(itemId, newName, newQuantity, newPrice);
            }
        }
    }

    void makeItemVisibleToCustomer(int itemId) {
        for (auto& item : items) {
            if (item.getId() == itemId) {
                item.setDisplay(true);
                break;
            }
        }
    }

    // List all items
    void listAllItems() const {
        if (items.empty()) {
            cout << "\nNo items in inventory." << endl;
            return;
        }
        
        cout << "\n=== My Inventory ===" << endl;
        cout << setw(8) << "ID" << setw(25) << "Name" 
             << setw(12) << "Quantity" << setw(12) << "Price" 
             << setw(10) << "Visible" << endl;
        cout << string(67, '-') << endl;
        
        for (const auto& item : items) {
            cout << setw(8) << item.getId()
                 << setw(25) << item.getName()
                 << setw(12) << item.getQuantity()
                 << "  $" << setw(9) << fixed << setprecision(2) << item.getPrice()
                 << setw(10) << (item.getDisplay() ? "Yes" : "No") << endl;
        }
        cout << endl;
    }

    // Replenish stock
    void replenishItem(int itemId, int additionalQuantity) {
        for (auto& item : items) {
            if (item.getId() == itemId) {
                item.setQuantity(item.getQuantity() + additionalQuantity);
                cout << "Item replenished successfully! New quantity: " 
                     << item.getQuantity() << endl;
                return;
            }
        }
        cout << "Item not found!" << endl;
    }

    // Discard item
    void discardItem(int itemId) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (it->getId() == itemId) {
                cout << "Item '" << it->getName() << "' has been removed from inventory." << endl;
                items.erase(it);
                return;
            }
        }
        cout << "Item not found!" << endl;
    }

    // Set price
    void setItemPrice(int itemId, double newPrice) {
        for (auto& item : items) {
            if (item.getId() == itemId) {
                item.setPrice(newPrice);
                cout << "Price updated successfully to $" << fixed << setprecision(2) 
                     << newPrice << endl;
                return;
            }
        }
        cout << "Item not found!" << endl;
    }

    // Discover top k most popular items per month
    void discoverTopKItemsThisMonth(const vector<Transaction>& allTransactions, int k) const {
        cout << "\n=== Top " << k << " Items This Month ===" << endl;
        
        map<int, pair<string, int>> itemSales;
        
        for (const auto& trans : allTransactions) {
            if (trans.getSellerId() == this->getId() && 
                trans.getType() == PURCHASE &&
                trans.getStatus() != CANCELLED &&
                trans.isWithinDays(30)) {
                itemSales[trans.getItemId()].first = trans.getItemName();
                itemSales[trans.getItemId()].second += trans.getQuantity();
            }
        }

        vector<pair<string, int>> sortedItems;
        for (const auto& pair : itemSales) {
            sortedItems.push_back({pair.second.first, pair.second.second});
        }

        sort(sortedItems.begin(), sortedItems.end(),
             [](const pair<string, int>& a, const pair<string, int>& b) {
                 return a.second > b.second;
             });

        cout << setw(25) << "Item Name" << setw(15) << "Quantity Sold" << endl;
        cout << string(40, '-') << endl;

        int count = 0;
        for (const auto& item : sortedItems) {
            if (count >= k) break;
            cout << setw(25) << item.first << setw(15) << item.second << endl;
            count++;
        }

        if (count == 0) {
            cout << "No sales this month." << endl;
        }
        cout << endl;
    }

    // Discover loyal customers per month
    void discoverLoyalCustomersThisMonth(const vector<Transaction>& allTransactions) const {
        cout << "\n=== Loyal Customers This Month ===" << endl;
        
        map<int, pair<string, int>> customerPurchases;
        
        for (const auto& trans : allTransactions) {
            if (trans.getSellerId() == this->getId() && 
                trans.getType() == PURCHASE &&
                trans.getStatus() != CANCELLED &&
                trans.isWithinDays(30)) {
                customerPurchases[trans.getBuyerId()].first = trans.getBuyerName();
                customerPurchases[trans.getBuyerId()].second++;
            }
        }

        vector<pair<string, int>> sortedCustomers;
        for (const auto& pair : customerPurchases) {
            sortedCustomers.push_back({pair.second.first, pair.second.second});
        }

        sort(sortedCustomers.begin(), sortedCustomers.end(),
             [](const pair<string, int>& a, const pair<string, int>& b) {
                 return a.second > b.second;
             });

        cout << setw(20) << "Customer Name" << setw(15) << "Purchases" << endl;
        cout << string(35, '-') << endl;

        for (const auto& customer : sortedCustomers) {
            cout << setw(20) << customer.first << setw(15) << customer.second << endl;
        }

        if (sortedCustomers.empty()) {
            cout << "No customers this month." << endl;
        }
        cout << endl;
    }

    // Get item by ID
    Item* getItemById(int itemId) {
        for (auto& item : items) {
            if (item.getId() == itemId) {
                return &item;
            }
        }
        return nullptr;
    }

    // Check if seller has item with sufficient stock
    bool hasItemWithStock(int itemId, int requiredQuantity) const {
        for (const auto& item : items) {
            if (item.getId() == itemId && item.getDisplay() && 
                item.getQuantity() >= requiredQuantity) {
                return true;
            }
        }
        return false;
    }

    // Reduce item quantity (for purchase)
    bool reduceItemQuantity(int itemId, int quantity) {
        for (auto& item : items) {
            if (item.getId() == itemId && item.getQuantity() >= quantity) {
                item.setQuantity(item.getQuantity() - quantity);
                return true;
            }
        }
        return false;
    }

    // Add seller-specific members here
};