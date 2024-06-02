#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>

using namespace std;

class Order {
public:
    static int nextId;
    int id;
    int shoeId;
    string date;
    bool state;

    Order(int shoe, const string& date_, bool state_)
        : id(nextId++), shoeId(shoe), date(date_), state(state_) {}

    Order(int id_, int shoe, const string& date_, bool state_)
        : id(id_), shoeId(shoe), date(date_), state(state_) {}

    int getId() const {
        return id;
    }

    string getDate() const {
        return date;
    }

    bool getState() const {
        return state;
    }

    int getShoeId() const {
        return shoeId;
    }

    void print() const {
        cout << "Order ID: " << id << endl;
        cout << "Date: " << date << endl;
        cout << "State: ";
        if (state) {
            cout << "Completed" << endl;
        }
        else {
            cout << "Not completed" << endl;
        }
        cout << "Shoe ID: " << shoeId << endl;
    }
};
int Order::nextId = 0;

class Shoe {
public:
    static int nextId;
    int id;
    string name;
    int price;
    string color;
    vector<Order> orders;

    Shoe(const string& name_, int num, const string& color_)
        : id(nextId++), name(name_), price(num), color(color_) {}

    Shoe(int id_, const string& name_, int num, const string& color_)
        : id(id_), name(name_), price(num), color(color_) {}

    int getId() const {
        return id;
    }

    string getName() const {
        return name;
    }

    int getPrice() const {
        return price;
    }

    string getColor() const {
        return color;
    }

    const vector<Order>& getOrders() const {
        return orders;
    }

    void addOrder(const Order& newOrder) {
        orders.push_back(newOrder);
    }

    void print() const {
        cout << "Shoe ID: " << id << endl;
        cout << "Name: " << name << endl;
        cout << "Price: " << price << endl;
        cout << "Color: " << color << endl;
        cout << "Orders:" << endl << endl;
        for (const auto& order : orders) {
            order.print();
            cout << endl;
        }
    }
};
int Shoe::nextId = 0;

int findShoeIndexById(const vector<Shoe>& shoes, int id) {
    int foundIndex = -1;
    for (size_t i = 0; i < shoes.size(); ++i) {
        if (shoes[i].getId() == id) {
            foundIndex = i;
            break;
        }
    }
    return foundIndex;
}

int findOrderIndexById(const vector<Order>& orders, int id) {
    int foundIndex = -1;
    for (size_t i = 0; i < orders.size(); ++i) {
        if (orders[i].getId() == id) {
            foundIndex = i;
            break;
        }
    }
    return foundIndex;
}

vector<Shoe> readFile() {
    vector<Shoe> shoes;
    ifstream masterFile("shoes.txt");
    ifstream slaveFile("orders.txt");

    int maxOrderId = 0;
    int maxShoeId = 0;

    if (masterFile.is_open() && slaveFile.is_open()) {
        string line;
        while (getline(masterFile, line)) {
            stringstream ss(line);
            string token;

            getline(ss, token, ';');
            int id = stoi(token);
            maxShoeId = max(maxShoeId, id);

            getline(ss, token, ';');
            string name = token;

            getline(ss, token, ';');
            int price = stoi(token);

            getline(ss, token, ';');
            string color = token;

            Shoe shoe(id, name, price, color);

            shoes.push_back(shoe);
        }

        while (getline(slaveFile, line)) {
            stringstream ss(line);
            string token;

            getline(ss, token, ';');
            int orderId = stoi(token);
            maxOrderId = max(maxOrderId, orderId);

            getline(ss, token, ';');
            string date = token;

            getline(ss, token, ';');
            bool state = stoi(token);

            getline(ss, token, ';');
            int shoeId = stoi(token);

            for (auto& shoe : shoes) {
                if (shoe.getId() == shoeId) {
                    Order order(orderId, shoeId, date, state);
                    shoe.addOrder(order);
                    break;
                }
            }
        }

        masterFile.close();
        slaveFile.close();

        Order::nextId = maxOrderId + 1;
        Shoe::nextId = maxShoeId + 1;
    }
    else {
        cerr << "Error: Unable to open file for reading" << endl;
    }

    return shoes;
}

void writeFile(const vector<Shoe>& shoes) {
    ofstream masterFile("shoes.txt");
    ofstream slaveFile("orders.txt");

    if (masterFile.is_open() && slaveFile.is_open()) {
        for (Shoe shoe : shoes) {
            masterFile << shoe.getId() << ";" << shoe.getName() << ";" << shoe.getPrice() << ";" << shoe.getColor() << endl;

            const vector<Order>& orders = shoe.getOrders();
            for (Order order : orders) {
                slaveFile << order.getId() << ";" << order.getDate() << ";" << order.getState() << ";" << order.getShoeId() << endl;
            }
        }
        masterFile.close();
        slaveFile.close();
    }
    else {
        cerr << "Error: Unable to open file for writing" << endl;
    }
}

void deleteShoe(vector<Shoe>& shoes, int index) {
    int foundIndex = findShoeIndexById(shoes, index);
    if (foundIndex >= 0 && foundIndex < shoes.size()) {
        shoes.erase(shoes.begin() + foundIndex);
    }
    else {
        cerr << "Error: Invalid shoe id" << endl;
    }
}

void deleteOrder(vector<Shoe>& shoes, int shoeId, int orderId) {
    int foundShoeIndex = findShoeIndexById(shoes, shoeId);
    if (foundShoeIndex >= 0 && foundShoeIndex < shoes.size()) {
        vector<Order>& orders = shoes[foundShoeIndex].orders;
        int foundOrderIndex = findOrderIndexById(orders, orderId);
        if (foundOrderIndex >= 0 && foundOrderIndex < orders.size()) {
            orders.erase(orders.begin() + foundOrderIndex);
        }
        else {
            cerr << "Error: Invalid order id" << endl;
        }
    }
    else {
        cerr << "Error: Invalid shoe id" << endl;
    }
}

Order updateOrder(Order order, const string& field, const string& value) {
    if (field == "date") {
        order.date = value;
    }
    else if (field == "state") {
        order.state = stoi(value);
    }
    else {
        cerr << "Error: Invalid field name" << endl;
    }
    return order;
}

void updateShoe(Shoe& shoe, const string& field, const string& value) {
    if (field == "name") {
        shoe.name = value;
    }
    else if (field == "price") {
        istringstream iss(value);
        int num;
        if (iss >> num) {
            shoe.price = num;
        }
        else {
            cerr << "Error: Invalid price value" << endl;
        }
    }
    else if (field == "color") {
        shoe.color = value;
    }
    else {
        cerr << "Error: Invalid field name" << endl;
    }
}

void countRecords(const vector<Shoe>& shoes) {
    int totalShoes = shoes.size();
    int totalOrders = 0;
    for (const auto& shoe : shoes) {
        totalOrders += shoe.getOrders().size();
    }
    cout << "Total shoes: " << totalShoes << endl;
    cout << "Total orders: " << totalOrders << endl;
}

void printFileContents(const string& filename) {
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    }
    else {
        cerr << "Error: Unable to open file for reading" << endl;
    }
}

int main() {
    vector<Shoe> shoes = readFile();

    int choice;
    do {
        cout << "1) Create shoe" << endl;
        cout << "2) Create order" << endl;
        cout << "3) Delete shoe" << endl;
        cout << "4) Delete order" << endl;
        cout << "5) Print shoe info" << endl;
        cout << "6) Print order info" << endl;
        cout << "7) Update shoe field" << endl;
        cout << "8) Update order field" << endl;
        cout << "9) Count shoes" << endl;
        cout << "10) Print slave file" << endl;
        cout << "11) Print master file" << endl;
        cout << "Enter your choice (1-11): ";
        cin >> choice;

        switch (choice) {
        case 1: {
            string name, color;
            int price;
            cout << "Enter name: ";
            cin >> name;
            cout << "Enter price: ";
            cin >> price;
            cout << "Enter color: ";
            cin >> color;
            shoes.emplace_back(name, price, color);
            cout << "Created a shoe:" << endl;
            writeFile(shoes);
            shoes.back().print();
            break;
        }
        case 2: {
            int shoeId;
            string date;
            bool state;
            cout << "Enter shoe id to add shoe: ";
            cin >> shoeId;
            cout << "Enter date: ";
            cin >> date;
            cout << "Enter state (0/1): ";
            cin >> state;
            int foundIndex = -1;
            foundIndex = findShoeIndexById(shoes, shoeId);
            if (foundIndex >= 0 && foundIndex < shoes.size()) {
                cout << "Created an order" << endl;
                shoes[foundIndex].addOrder(Order(shoes[foundIndex].getId(), date, state));
                shoes[foundIndex].orders.back().print();
            }
            else {
                cerr << "Error: Invalid shoe id" << endl;
            }
            writeFile(shoes);
            break;
        }
        case 3: {
            int id;
            cout << "Enter shoe id to delete: ";
            cin >> id;
            deleteShoe(shoes, id);
            writeFile(shoes);
            break;
        }
        case 4: {
            int orderId;
            cout << "Enter order id: ";
            cin >> orderId;
            int foundShoeIndex = -1;
            int foundOrderIndex = -1;
            for (size_t i = 0; i < shoes.size(); ++i) {
                const vector<Order>& orders = shoes[i].getOrders();
                foundOrderIndex = findOrderIndexById(orders, orderId);
                if (foundOrderIndex >= 0 && foundOrderIndex < orders.size()) {
                    foundShoeIndex = i;
                    break;
                }
            }
            if (foundShoeIndex >= 0 && foundShoeIndex < shoes.size()) {
                const vector<Order>& orders = shoes[foundShoeIndex].getOrders();
                deleteOrder(shoes, shoes[foundShoeIndex].getId(), orders[foundOrderIndex].getId());
                writeFile(shoes);
            }
            else {
                cerr << "Error: Invalid order id" << endl;
            }
            break;
        }
        case 5: {
            int id;
            cout << "Enter shoe id: ";
            cin >> id;
            int foundIndex = findShoeIndexById(shoes, id);
            if (foundIndex >= 0 && foundIndex < shoes.size()) {
                writeFile(shoes);
                shoes[foundIndex].print();
            }
            else {
                cerr << "Error: Invalid id" << endl;
            }
            break;
        }
        case 6: {
            int orderId;
            cout << "Enter order id: ";
            cin >> orderId;
            int foundOrderIndex = -1;
            for (const auto& shoe : shoes) {
                foundOrderIndex = findOrderIndexById(shoe.orders, orderId);
                if (foundOrderIndex != -1) {
                    shoe.orders[foundOrderIndex].print();
                    writeFile(shoes);
                    break;
                }
            }
            //cerr << "Error: Invalid order id" << endl;
            break;
        }
        case 7: {
            int id;
            string field, value;
            cout << "Enter shoe id to update: ";
            cin >> id;
            int foundIndex = -1;
            foundIndex = findShoeIndexById(shoes, id);
            if (foundIndex >= 0 && foundIndex < shoes.size()) {
                cout << "Enter field to update (name/price/color): ";
                cin >> field;
                if (field == "name" || field == "price" || field == "color") {
                    cout << "Enter new value: ";
                    cin >> value;
                    updateShoe(shoes[foundIndex], field, value);
                    writeFile(shoes);
                }
                else {
                    cerr << "Error: Invalid field name" << endl;
                }
            }
            else {
                cerr << "Error: Invalid shoe id" << endl;
            }
            break;
        }
        case 8: {
            int orderId;
            string field, value;
            cout << "Enter order id to update: ";
            cin >> orderId;
            int foundOrderIndex = -1;
            int foundShoeIndex = -1;
            for (size_t i = 0; i < shoes.size(); ++i) {
                const vector<Order>& orders = shoes[i].getOrders();
                foundOrderIndex = findOrderIndexById(orders, orderId);
                if (foundOrderIndex >= 0 && foundOrderIndex < orders.size()) {
                    foundShoeIndex = i;
                    break;
                }
            }
            if (foundShoeIndex >= 0 && foundShoeIndex < shoes.size()) {
                const vector<Order>& orders = shoes[foundShoeIndex].getOrders();
                cout << "Enter field to update (date/state): ";
                cin >> field;
                if (field == "date" || field == "state") {
                    cout << "Enter new value: ";
                    cin >> value;
                    shoes[foundShoeIndex].orders[foundOrderIndex] = updateOrder(orders[foundOrderIndex], field, value);
                    writeFile(shoes);
                }
                else {
                    cerr << "Error: Invalid field name" << endl;
                }
            }
            else {
                cerr << "Error: Invalid order id" << endl;
            }
            break;
        }
        case 9: {
            countRecords(shoes);
            writeFile(shoes);
            break;
        }
        case 10: {
            printFileContents("orders.txt");
            writeFile(shoes);
            break;
        }
        case 11: {
            printFileContents("shoes.txt");
            writeFile(shoes);
            break;
        }
        default:
            cerr << "Invalid choice. Enter a number between 1 and 11" << endl;
            writeFile(shoes);
        }
        cout << "==================================================================================" << endl;
    } while (choice != 0);

    writeFile(shoes);

    return 0;
}