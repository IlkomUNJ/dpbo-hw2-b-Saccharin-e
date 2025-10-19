#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace std;

enum TransactionStatus { PAID, COMPLETED, CANCELLED };
enum TransactionType { PURCHASE, TOPUP, WITHDRAW };

class Transaction {
private:
    int id;
    int buyerId;
    int sellerId;
    int itemId;
    string buyerName;
    string sellerName;
    string itemName;
    double amount;
    int quantity;
    TransactionStatus status;
    TransactionType type;
    time_t timestamp;

public:
    Transaction(int id, int buyerId, int sellerId, int itemId, 
                const string& buyerName, const string& sellerName, 
                const string& itemName, double amount, int quantity,
                TransactionType type = PURCHASE)
        : id(id), buyerId(buyerId), sellerId(sellerId), itemId(itemId),
          buyerName(buyerName), sellerName(sellerName), itemName(itemName),
          amount(amount), quantity(quantity), status(PAID), type(type) {
        timestamp = time(nullptr);
    }

    // Banking transaction constructor (topup/withdraw)
    Transaction(int id, int customerId, const string& customerName, 
                double amount, TransactionType type)
        : id(id), buyerId(customerId), sellerId(-1), itemId(-1),
          buyerName(customerName), sellerName(""), itemName(""),
          amount(amount), quantity(0), status(COMPLETED), type(type) {
        timestamp = time(nullptr);
    }

    // Getters
    int getId() const { return id; }
    int getBuyerId() const { return buyerId; }
    int getSellerId() const { return sellerId; }
    int getItemId() const { return itemId; }
    string getBuyerName() const { return buyerName; }
    string getSellerName() const { return sellerName; }
    string getItemName() const { return itemName; }
    double getAmount() const { return amount; }
    int getQuantity() const { return quantity; }
    TransactionStatus getStatus() const { return status; }
    TransactionType getType() const { return type; }
    time_t getTimestamp() const { return timestamp; }

    // Setters
    void setStatus(TransactionStatus newStatus) { status = newStatus; }

    // Utility functions
    string getStatusString() const {
        switch (status) {
            case PAID: return "Paid";
            case COMPLETED: return "Completed";
            case CANCELLED: return "Cancelled";
            default: return "Unknown";
        }
    }

    string getTypeString() const {
        switch (type) {
            case PURCHASE: return "Purchase";
            case TOPUP: return "Top-up";
            case WITHDRAW: return "Withdraw";
            default: return "Unknown";
        }
    }

    string getDateString() const {
        char buffer[80];
        struct tm* timeinfo = localtime(&timestamp);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        return string(buffer);
    }

    string getDateOnlyString() const {
        char buffer[80];
        struct tm* timeinfo = localtime(&timestamp);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
        return string(buffer);
    }

    bool isWithinDays(int days) const {
        time_t now = time(nullptr);
        double seconds = difftime(now, timestamp);
        return (seconds / (60 * 60 * 24)) <= days;
    }

    bool isToday() const {
        time_t now = time(nullptr);
        struct tm* nowInfo = localtime(&now);
        struct tm* transInfo = localtime(&timestamp);
        return (nowInfo->tm_year == transInfo->tm_year &&
                nowInfo->tm_mon == transInfo->tm_mon &&
                nowInfo->tm_mday == transInfo->tm_mday);
    }
};

#endif // TRANSACTION_H
