
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class ProductDetails {
protected:
    int productID;
    string name;
    float price;
    int stock;

public:
    ProductDetails(int id, string n, float p, int s) : productID(id), name(n), price(p), stock(s) {}

    virtual float getPrice() { return price; }
    int getProductID() { return productID; }
    string getName() { return name; }
    int getStock() { return stock; }

    void reduceStock() {
        if (stock > 0) {
            stock--;
        } else {
            cout << "Error: Stock cannot go below zero!" << endl;
        }
    }

    void increaseStock() { stock++; }

    void updateStock(int newStock) { stock = newStock; }

    virtual void displayDetails() {
        cout << "Product ID: " << productID << " | Name: " << name
             << " | Price: Rs " << price << " | Stock: " << stock << endl;
    }
};

class DiscountedProduct : public ProductDetails {
    float discountPercentage;

public:
    DiscountedProduct(int id, string n, float p, int s, float discount)
        : ProductDetails(id, n, p, s), discountPercentage(discount) {}

    float getPrice() override {
        return price * (1 - discountPercentage / 100);
    }

    void displayDetails() override {
        cout << "Product ID: " << productID << " | Name: " << name
             << " | Original Price: Rs " << price
             << " | Discount: " << discountPercentage << "%"
             << " | Final Price: Rs " << getPrice()
             << " | Stock: " << stock << endl;
    }
};

template <typename T>
class Cart {
private:
    T items[100];
    int itemCount;

public:
    Cart() : itemCount(0) {}

    void addItem(T item) { items[itemCount++] = item; }

    void removeItem(int productID) {
        for (int i = 0; i < itemCount; ++i) {
            if (items[i]->getProductID() == productID) {
                for (int j = i; j < itemCount - 1; ++j) {
                    items[j] = items[j + 1];
                }
                --itemCount;
                break;
            }
        }
    }

    float calculateTotalCost() {
        float total = 0;
        for (int i = 0; i < itemCount; ++i) {
            total += items[i]->getPrice();
        }
        return total;
    }

    void checkout() {
        cout << "Total cost of items in cart: Rs " << calculateTotalCost() << endl;
        itemCount = 0;
    }
};

class StaffPortal {
private:
    const string staffKey = "ELEXP";
    ProductDetails* products[100];
    int productCount;

    void saveProductsToFile() {
        ofstream file("products.txt");
        for (int i = 0; i < productCount; ++i) {
            file << products[i]->getProductID() << "," << products[i]->getName()
                 << "," << products[i]->getPrice() << "," << products[i]->getStock() << "\n";
        }
        file.close();
    }

    void loadProductsFromFile() {
        ifstream file("products.txt");
        int id, stock;
        string name;
        float price;
        productCount = 0;
        while (file >> id) {
            file.ignore();
            getline(file, name, ',');
            file >> price;
            file.ignore();
            file >> stock;

            products[productCount++] = new ProductDetails(id, name, price, stock);
        }
        file.close();
    }

public:
    StaffPortal() : productCount(0) {
        loadProductsFromFile();
    }

    bool authenticate(const string& key) {
        return key == staffKey;
    }

    void addProduct() {
        if (productCount < 100) {
            int id, stock;
            string name;
            float price;
            char choice;
            cout << "Enter product ID: ";
            cin >> id;
            cout << "Enter product name: ";
            cin.ignore();
            getline(cin, name);
            cout << "Enter product price: ";
            cin >> price;
            cout << "Enter stock quantity: ";
            cin >> stock;
            cout << "Is this a discounted product? (y/n): ";
            cin >> choice;

            if (choice == 'y' || choice == 'Y') {
                float discount;
                cout << "Enter discount percentage: ";
                cin >> discount;
                products[productCount++] = new DiscountedProduct(id, name, price, stock, discount);
            } else {
                products[productCount++] = new ProductDetails(id, name, price, stock);
            }

            saveProductsToFile();
            cout << "Product added successfully!" << endl;
        } else {
            cout << "Store is full!" << endl;
        }
    }

    void updateStock() {
        int id, newStock;
        cout << "Enter Product ID to update stock: ";
        cin >> id;

        bool found = false;
        for (int i = 0; i < productCount; ++i) {
            if (products[i]->getProductID() == id) {
                cout << "Current stock for " << products[i]->getName() << ": " << products[i]->getStock() << endl;
                cout << "Enter new stock quantity: ";
                cin >> newStock;

                if (newStock >= 0) {
                    products[i]->updateStock(newStock);
                    saveProductsToFile();
                    cout << "Stock updated successfully!" << endl;
                } else {
                    cout << "Invalid stock value!" << endl;
                }
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "Product not found!" << endl;
        }
    }

    void saveStockChanges() {
        saveProductsToFile();
    }

    ProductDetails* getProduct(int index) {
        if (index >= 0 && index < productCount) {
            return products[index];
        }
        return nullptr;
    }

    int getProductCount() {
        return productCount;
    }
};

int main() {
    StaffPortal staff;
    Cart<ProductDetails*> myCart;
    int choice;

    do {
        cout << "\n1. Customer Portal\n2. Staff Portal\n3. Exit\nEnter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                int customerChoice;
                do {
                    cout << "\n1. Add product to cart\n2. Remove product from cart\n3. View total cost\n4. Checkout\n5. Back\nEnter your choice: ";
                    cin >> customerChoice;

                    switch (customerChoice) {
                        case 1: {
                            cout << "\nAvailable Products:" << endl;
                            for (int i = 0; i < staff.getProductCount(); ++i) {
                                staff.getProduct(i)->displayDetails();
                            }

                            int productID;
                            cout << "Enter Product ID to add to cart: ";
                            cin >> productID;

                            bool found = false;
                            for (int i = 0; i < staff.getProductCount(); ++i) {
                                if (staff.getProduct(i)->getProductID() == productID) {
                                    int quantity;
                                    cout << "Enter quantity to add: ";
                                    cin >> quantity;
                                    myCart.addItem(staff.getProduct(i));  // Add single item (can modify for quantity)
                                    found = true;
                                    break;
                                }
                            }
                            if (!found) {
                                cout << "Product not found!" << endl;
                            }
                            break;
                        }
                        case 2: {
                            int productID;
                            cout << "Enter Product ID to remove from cart: ";
                            cin >> productID;
                            myCart.removeItem(productID);
                            break;
                        }
                        case 3:
                            cout << "Total cost of items in cart: Rs " << myCart.calculateTotalCost() << endl;
                            break;
                        case 4:
                            myCart.checkout();
                            staff.saveStockChanges();
                            break;
                    }
                } while (customerChoice != 5);
                break;
            }

            case 2: {
                string key;
                cout << "Enter staff key: ";
                cin >> key;
                if (staff.authenticate(key)) {
                    int staffChoice;
                    do {
                        cout << "\n1. Add new product\n2. Update stock\n3. Back\nEnter your choice: ";
                        cin >> staffChoice;

                        switch (staffChoice) {
                            case 1:
                                staff.addProduct();
                                break;
                            case 2:
                                staff.updateStock();
                                break;
                        }
                    } while (staffChoice != 3);
                } else {
                    cout << "Invalid key!" << endl;
                }
                break;
            }

            case 3:
                cout << "Exiting program. Goodbye!" << endl;
                break;

            default:
                cout << "Invalid choice! Please enter 1, 2, or 3." << endl;
        }

    } while (choice != 3);

    return 0;
}
