#include <iostream>
#include <vector>
#include <string>
#include "bank_customer.h"
#include "buyer.h"
#include "seller.h"
#include "transaction.h"
#include "store.h"
#include "bank.h"

enum PrimaryPrompt{LOGIN, REGISTER, ADMIN, EXIT_PROGRAM};
enum UserAction{CHECK_STATUS = 1, UPGRADE_SELLER, BANK_ACCOUNT, PURCHASE_ITEM, MY_ORDERS, SELLER_MENU, LOGOUT, DELETE_ACCOUNT};

using namespace std;

vector<BankCustomer> bankCustomers;
vector<Buyer> buyers;
vector<seller> sellers;
Store store;
Bank bank("Central Bank");
int nextId = 1;

BankCustomer* findBankCustomerByName(const string& name) {
    for (auto& customer : bankCustomers) {
        if (customer.getName() == name) {
            return &customer;
        }
    }
    return nullptr;
}

Buyer* findBuyerByName(const string& name) {
    for (auto& buyer : buyers) {
        if (buyer.getName() == name) {
            return &buyer;
        }
    }
    return nullptr;
}

seller* findSellerByName(const string& name) {
    for (auto& s : sellers) {
        if (s.getName() == name) {
            return &s;
        }
    }
    return nullptr;
}

seller* findSellerById(int sellerId) {
    for (auto& s : sellers) {
        if (s.getId() == sellerId) {
            return &s;
        }
    }
    return nullptr;
}

void displayUserMenu(const string& username) {
    seller* s = findSellerByName(username);
    cout << "\n=== Welcome " << username << " ===" << endl;
    cout << "1. Check Account Status" << endl;
    if (s) {
        cout << "2. Upgrade to Seller Account (Already Upgraded)" << endl;
    } else {
        cout << "2. Upgrade to Seller Account" << endl;
    }
    cout << "3. Bank Account Functions" << endl;
    cout << "4. Purchase Item" << endl;
    cout << "5. My Orders" << endl;
    if (s) {
        cout << "6. Seller Management" << endl;
    }
    cout << "7. Logout" << endl;
    cout << "8. Delete Account" << endl;
    cout << "Enter your choice: ";
}

void checkAccountStatus(const string& username) {
    Buyer* buyer = findBuyerByName(username);
    seller* s = findSellerByName(username);
    BankCustomer* bankCustomer = findBankCustomerByName(username);
    
    cout << "\n=== Account Status ===" << endl;
    cout << "Username: " << username << endl;
    
    if (buyer) {
        cout << "Buyer Account: Active" << endl;
        cout << "Buyer ID: " << buyer->getId() << endl;
        cout << "Bank Account: " << (bankCustomer ? "Linked" : "Not Linked") << endl;
        if (bankCustomer) {
            cout << "Bank Balance: $" << bankCustomer->getBalance() << endl;
        }
    }
    
    if (s) {
        cout << "Seller Account: Active" << endl;
        cout << "Seller ID: " << s->getId() << endl;
    } else {
        cout << "Seller Account: Not Active" << endl;
    }
    cout << endl;
}

void upgradeToSeller(const string& username) {
    Buyer* buyer = findBuyerByName(username);
    if (!buyer) {
        cout << "Error: Buyer account not found!" << endl;
        return;
    }
    
    seller* existingSeller = findSellerByName(username);
    if (existingSeller) {
        cout << "You already have a seller account!" << endl;
        return;
    }
    
    BankCustomer* bankCustomer = findBankCustomerByName(username);
    if (!bankCustomer) {
        cout << "Error: You must have a bank account linked to upgrade to seller!" << endl;
        cout << "Please create a bank account first." << endl;
        return;
    }
    
    cout << "\n=== Seller Account Upgrade ===" << endl;
    string storeName, storeAddress, storePhone, storeEmail;
    
    cout << "Enter Store Name: ";
    cin.ignore();
    getline(cin, storeName);
    
    cout << "Enter Store Address: ";
    getline(cin, storeAddress);
    
    cout << "Enter Store Phone: ";
    getline(cin, storePhone);
    
    cout << "Enter Store Email: ";
    getline(cin, storeEmail);
    
    seller newSeller(*buyer, nextId++, storeName);
    sellers.push_back(newSeller);
    
    cout << "Seller account created successfully!" << endl;
    cout << "Store Name: " << storeName << endl;
    cout << "You can now sell items on the platform." << endl;
}

void bankAccountFunctions(const string& username) {
    Buyer* buyer = findBuyerByName(username);
    if (!buyer) {
        cout << "Error: Buyer account not found!" << endl;
        return;
    }
    
    BankCustomer* bankCustomer = findBankCustomerByName(username);
    
    if (!bankCustomer) {
        cout << "\n=== Create Bank Account ===" << endl;
        cout << "You don't have a bank account. Let's create one." << endl;
        
        string address, phone, email;
        double initialDeposit;
        
        cout << "Enter your address: ";
        cin.ignore();
        getline(cin, address);
        
        cout << "Enter your phone number: ";
        getline(cin, phone);
        
        cout << "Enter your email: ";
        getline(cin, email);
        
        cout << "Enter initial deposit amount: $";
        cin >> initialDeposit;
        
        if (initialDeposit < 0) {
            cout << "Invalid deposit amount!" << endl;
            return;
        }
        
        BankCustomer newBankCustomer(nextId++, username, initialDeposit);
        bankCustomers.push_back(newBankCustomer);
        
        cout << "Bank account created successfully!" << endl;
        cout << "Initial balance: $" << initialDeposit << endl;
        
        // Record transaction
        Transaction trans(store.getNextTransactionId(), newBankCustomer.getId(), 
                         username, initialDeposit, TOPUP);
        bank.addBankTransaction(trans);
        
        // Update bankCustomer pointer to the newly created account
        bankCustomer = &bankCustomers.back();
    }
    
    // Bank account functions loop
    bool inBankMenu = true;
    while (inBankMenu) {
        cout << "\n=== Bank Account Functions ===" << endl;
        cout << "1. Check Balance" << endl;
        cout << "2. Deposit Money" << endl;
        cout << "3. Withdraw Money" << endl;
        cout << "4. Back to Main Menu" << endl;
        cout << "Enter your choice: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1:
                cout << "Current Balance: $" << bankCustomer->getBalance() << endl;
                break;
            case 2: {
                double amount;
                cout << "Enter deposit amount: $";
                cin >> amount;
                if (amount > 0) {
                    bankCustomer->addBalance(amount);
                    cout << "Deposited $" << amount << " successfully!" << endl;
                    cout << "New balance: $" << bankCustomer->getBalance() << endl;
                    
                    // Record transaction
                    Transaction trans(store.getNextTransactionId(), bankCustomer->getId(), 
                                     username, amount, TOPUP);
                    bank.addBankTransaction(trans);
                } else {
                    cout << "Invalid deposit amount!" << endl;
                }
                break;
            }
            case 3: {
                double amount;
                cout << "Enter withdrawal amount: $";
                cin >> amount;
                if (bankCustomer->withdrawBalance(amount)) {
                    cout << "Withdrew $" << amount << " successfully!" << endl;
                    cout << "New balance: $" << bankCustomer->getBalance() << endl;
                    
                    // Record transaction
                    Transaction trans(store.getNextTransactionId(), bankCustomer->getId(), 
                                     username, amount, WITHDRAW);
                    bank.addBankTransaction(trans);
                } else {
                    cout << "Insufficient funds or invalid amount!" << endl;
                }
                break;
            }
            case 4:
                inBankMenu = false;
                break;
            default:
                cout << "Invalid choice!" << endl;
        }
    }
}

// Forward declaration
void sellerInventoryManagement(const string& username);

void purchaseItem() {
    cout << "\n=== Purchase Item ===" << endl;
    cout << "\n=== Available Sellers ===" << endl;
    
    if (sellers.empty()) {
        cout << "No sellers available." << endl;
        return;
    }
    
    // Display all sellers
    for (auto& s : sellers) {
        cout << "\nSeller: " << s.getName() << " (ID: " << s.getId() << ")" << endl;
        s.listAllItems();
    }
    
    int sellerId, itemId, quantity;
    cout << "\nEnter Seller ID: ";
    cin >> sellerId;
    cout << "Enter Item ID: ";
    cin >> itemId;
    cout << "Enter Quantity: ";
    cin >> quantity;
    
    seller* s = findSellerById(sellerId);
    if (!s) {
        cout << "Seller not found!" << endl;
        return;
    }
    
    Item* item = s->getItemById(itemId);
    if (!item) {
        cout << "Item not found!" << endl;
        return;
    }
    
    if (!item->getDisplay()) {
        cout << "Item is not available for purchase!" << endl;
        return;
    }
    
    if (item->getQuantity() < quantity) {
        cout << "Insufficient stock! Available: " << item->getQuantity() << endl;
        return;
    }
    
    double totalAmount = item->getPrice() * quantity;
    cout << "\nTotal Amount: $" << totalAmount << endl;
    cout << "Confirm purchase? (y/n): ";
    char confirm;
    cin >> confirm;
    
    if (confirm == 'y' || confirm == 'Y') {
        s->reduceItemQuantity(itemId, quantity);
        cout << "Purchase successful!" << endl;
        cout << "Total paid: $" << totalAmount << endl;
    }
}

void myOrders(const string& username) {
    Buyer* buyer = findBuyerByName(username);
    if (!buyer) {
        cout << "Error: Buyer account not found!" << endl;
        return;
    }
    
    bool inOrderMenu = true;
    while (inOrderMenu) {
        cout << "\n=== My Orders ===" << endl;
        cout << "1. List All Orders" << endl;
        cout << "2. List Paid Orders" << endl;
        cout << "3. List Completed Orders" << endl;
        cout << "4. List Cancelled Orders" << endl;
        cout << "5. Check Spending Last K Days" << endl;
        cout << "6. Back to Main Menu" << endl;
        cout << "Enter your choice: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1:
                buyer->listOrders(store.getAllTransactions(), -1);
                break;
            case 2:
                buyer->listOrders(store.getAllTransactions(), PAID);
                break;
            case 3:
                buyer->listOrders(store.getAllTransactions(), COMPLETED);
                break;
            case 4:
                buyer->listOrders(store.getAllTransactions(), CANCELLED);
                break;
            case 5: {
                int k;
                cout << "Enter number of days: ";
                cin >> k;
                buyer->checkSpendingLastKDays(store.getAllTransactions(), k);
                break;
            }
            case 6:
                inOrderMenu = false;
                break;
            default:
                cout << "Invalid choice!" << endl;
        }
    }
}

void sellerManagement(const string& username) {
    seller* s = findSellerByName(username);
    if (!s) {
        cout << "Error: You must be a seller to access this menu!" << endl;
        return;
    }
    
    bool inSellerMenu = true;
    while (inSellerMenu) {
        cout << "\n=== Seller Management ===" << endl;
        cout << "1. Inventory Management" << endl;
        cout << "2. View Top K Popular Items This Month" << endl;
        cout << "3. View Loyal Customers This Month" << endl;
        cout << "4. Back to Main Menu" << endl;
        cout << "Enter your choice: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1:
                sellerInventoryManagement(username);
                break;
            case 2: {
                int k;
                cout << "Enter K (number of top items): ";
                cin >> k;
                s->discoverTopKItemsThisMonth(store.getAllTransactions(), k);
                break;
            }
            case 3:
                s->discoverLoyalCustomersThisMonth(store.getAllTransactions());
                break;
            case 4:
                inSellerMenu = false;
                break;
            default:
                cout << "Invalid choice!" << endl;
        }
    }
}

void sellerInventoryManagement(const string& username) {
    seller* s = findSellerByName(username);
    if (!s) {
        cout << "Error: You must be a seller to access inventory management!" << endl;
        return;
    }
    
    bool inInventoryMenu = true;
    while (inInventoryMenu) {
        cout << "\n=== Seller Inventory Management ===" << endl;
        cout << "1. Add New Item" << endl;
        cout << "2. Update Existing Item" << endl;
        cout << "3. Make Item Visible to Customers" << endl;
        cout << "4. Back to Main Menu" << endl;
        cout << "Enter your choice: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1: {
                cout << "\n=== Add New Item ===" << endl;
                int itemId, quantity;
                string itemName;
                double price;
                
                cout << "Enter Item ID: ";
                cin >> itemId;
                
                cout << "Enter Item Name: ";
                cin.ignore();
                getline(cin, itemName);
                
                cout << "Enter Quantity: ";
                cin >> quantity;
                
                cout << "Enter Price: $";
                cin >> price;
                
                if (quantity < 0 || price < 0) {
                    cout << "Invalid quantity or price!" << endl;
                    break;
                }
                
                s->addNewItem(itemId, itemName, quantity, price);
                cout << "Item added successfully!" << endl;
                cout << "Item ID: " << itemId << endl;
                cout << "Item Name: " << itemName << endl;
                cout << "Quantity: " << quantity << endl;
                cout << "Price: $" << price << endl;
                break;
            }
            case 2: {
                cout << "\n=== Update Existing Item ===" << endl;
                int itemId, quantity;
                string itemName;
                double price;
                
                cout << "Enter Item ID to Update: ";
                cin >> itemId;
                
                cout << "Enter New Item Name: ";
                cin.ignore();
                getline(cin, itemName);
                
                cout << "Enter New Quantity: ";
                cin >> quantity;
                
                cout << "Enter New Price: $";
                cin >> price;
                
                if (quantity < 0 || price < 0) {
                    cout << "Invalid quantity or price!" << endl;
                    break;
                }
                
                s->updateItem(itemId, itemName, quantity, price);
                cout << "Item updated successfully!" << endl;
                break;
            }
            case 3: {
                cout << "\n=== Make Item Visible ===" << endl;
                int itemId;
                
                cout << "Enter Item ID to make visible: ";
                cin >> itemId;
                
                s->makeItemVisibleToCustomer(itemId);
                cout << "Item is now visible to customers!" << endl;
                break;
            }
            case 4:
                inInventoryMenu = false;
                break;
            default:
                cout << "Invalid choice!" << endl;
        }
    }
}

void handleLogin() {
    string username, password;
    cout << "\n=== Login ===" << endl;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;
    
    Buyer* buyer = findBuyerByName(username);
    if (!buyer) {
        cout << "Invalid username or password!" << endl;
        return;
    }
    
    cout << "Login successful!" << endl;
    bool loggedIn = true;
    while (loggedIn) {
        displayUserMenu(username);
        int choice;
        cin >> choice;
        
        switch (choice) {
            case CHECK_STATUS:
                checkAccountStatus(username);
                break;
            case UPGRADE_SELLER:
                upgradeToSeller(username);
                break;
            case BANK_ACCOUNT:
                bankAccountFunctions(username);
                break;
            case PURCHASE_ITEM:
                purchaseItem();
                break;
            case MY_ORDERS:
                myOrders(username);
                break;
            case SELLER_MENU:
                sellerManagement(username);
                break;
            case LOGOUT:
                cout << "Logging out..." << endl;
                loggedIn = false;
                break;
            case DELETE_ACCOUNT:
                cout << "Are you sure you want to delete your account? (y/n): ";
                char confirm;
                cin >> confirm;
                if (confirm == 'y' || confirm == 'Y') {
                    cout << "Account deleted successfully!" << endl;
                    loggedIn = false;
                }
                break;
            default:
                cout << "Invalid choice!" << endl;
        }
        cout << endl;
    }
}

void handleRegister() {
    string username, password, fullName, address, phone, email;
    
    cout << "\n=== Registration ===" << endl;
    cout << "Enter username: ";
    cin >> username;
    
    if (findBuyerByName(username)) {
        cout << "Username already exists! Please choose a different username." << endl;
        return;
    }
    
    cout << "Enter password: ";
    cin >> password;
    
    cout << "Enter your full name: ";
    cin.ignore();
    getline(cin, fullName);
    
    cout << "Enter home address: ";
    getline(cin, address);
    
    cout << "Enter phone number: ";
    getline(cin, phone);
    
    cout << "Enter email: ";
    getline(cin, email);
    
    BankCustomer tempBankCustomer(0, "", 0.0);
    
    Buyer newBuyer(nextId++, username, tempBankCustomer);
    buyers.push_back(newBuyer);
    
    cout << "Buyer account created successfully!" << endl;
    cout << "Full Name: " << fullName << endl;
    cout << "Username: " << username << endl;
    cout << "You can now login with your username and password." << endl;
    cout << "Note: You can upgrade to seller later after creating a bank account." << endl;
}

void handleAdminAccess() {
    cout << "\n=== Admin Access ===" << endl;
    cout << "Enter admin password: ";
    string password;
    cin >> password;
    
    if (password != "admin123") {
        cout << "Access denied! Invalid password." << endl;
        return;
    }
    
    int choice = 0;
    while (choice != 9) {
        cout << "\n=== Admin Dashboard ===" << endl;
        cout << "1. Store Analytics" << endl;
        cout << "2. Bank Reports" << endl;
        cout << "3. User Statistics" << endl;
        cout << "9. Back to Main Menu" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        
        switch (choice) {
            case 1: {
                // Store Analytics submenu
                int storeChoice = 0;
                while (storeChoice != 6) {
                    cout << "\n--- Store Analytics ---" << endl;
                    cout << "1. Transactions (Last K Days)" << endl;
                    cout << "2. Paid but Not Completed Orders" << endl;
                    cout << "3. Most Frequent Items" << endl;
                    cout << "4. Most Active Buyers" << endl;
                    cout << "5. Most Active Sellers" << endl;
                    cout << "6. Back" << endl;
                    cout << "Enter your choice: ";
                    cin >> storeChoice;
                    
                    switch (storeChoice) {
                        case 1: {
                            int days;
                            cout << "Enter number of days: ";
                            cin >> days;
                            store.listTransactionsLastKDays(days);
                            break;
                        }
                        case 2:
                            store.listPaidNotCompleted();
                            break;
                        case 3: {
                            int m;
                            cout << "Enter number of top items to show: ";
                            cin >> m;
                            store.listMostFrequentItems(m);
                            break;
                        }
                        case 4: {
                            int n;
                            cout << "Enter number of top buyers to show: ";
                            cin >> n;
                            store.listMostActiveBuyers(n);
                            break;
                        }
                        case 5: {
                            int n;
                            cout << "Enter number of top sellers to show: ";
                            cin >> n;
                            store.listMostActiveSellers(n);
                            break;
                        }
                        case 6:
                            break;
                        default:
                            cout << "Invalid choice!" << endl;
                    }
                }
                break;
            }
            case 2: {
                // Bank Reports submenu
                int bankChoice = 0;
                while (bankChoice != 5) {
                    cout << "\n--- Bank Reports ---" << endl;
                    cout << "1. Transactions (Last Week)" << endl;
                    cout << "2. All Bank Customers" << endl;
                    cout << "3. Dormant Accounts (30+ days)" << endl;
                    cout << "4. Top Active Users Today" << endl;
                    cout << "5. Back" << endl;
                    cout << "Enter your choice: ";
                    cin >> bankChoice;
                    
                    switch (bankChoice) {
                        case 1:
                            bank.listTransactionsLastWeek();
                            break;
                        case 2:
                            bank.listAllCustomers(bankCustomers);
                            break;
                        case 3:
                            bank.listDormantAccounts(bankCustomers);
                            break;
                        case 4: {
                            int n;
                            cout << "Enter number of top users to show: ";
                            cin >> n;
                            bank.listTopUsersToday(n);
                            break;
                        }
                        case 5:
                            break;
                        default:
                            cout << "Invalid choice!" << endl;
                    }
                }
                break;
            }
            case 3: {
                // User Statistics
                cout << "\n--- User Statistics ---" << endl;
                cout << "Total Buyers: " << buyers.size() << endl;
                cout << "Total Sellers: " << sellers.size() << endl;
                cout << "Total Bank Customers: " << bankCustomers.size() << endl;
                cout << "Total Store Transactions: " << store.getTransactionCount() << endl;
                break;
            }
            case 9:
                cout << "Exiting admin panel..." << endl;
                break;
            default:
                cout << "Invalid choice!" << endl;
        }
    }
}

int main() {
    PrimaryPrompt prompt = LOGIN;
    while (prompt != EXIT_PROGRAM) {
        cout << "\n=== Online Store System ===" << endl;
        cout << "1. Login" << endl;
        cout << "2. Register" << endl;
        cout << "3. Admin Access" << endl;
        cout << "4. Exit" << endl;
        cout << "Enter your choice: ";
        
        int choice;
        cin >> choice;
        prompt = static_cast<PrimaryPrompt>(choice - 1);
        
        switch (prompt) {
            case LOGIN:
                handleLogin();
                break;
            case REGISTER:
                handleRegister();
                break;
            case ADMIN:
                handleAdminAccess();
                break;
            case EXIT_PROGRAM:
                cout << "Thank you for using the Online Store System!" << endl;
                break;
            default:
                cout << "Invalid option." << endl;
                break;
        }
        cout << endl;
    }

    return 0;
}