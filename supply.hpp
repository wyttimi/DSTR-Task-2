#ifndef SUPPLY_HPP
#define SUPPLY_HPP

#include "utils.hpp"

#define SUPPLY_FILE "supplies.txt"

// ========== ROLE 2: MEDICAL SUPPLY MANAGER (STACK) ==========

struct Supply {
    char type[30];
    int  quantity;
    char batch[20];
};

struct SupplyStack {
    Supply data[MAX_SUPPLIES];
    int top = -1; // -1 means empty

    bool isFull()  const { return top == MAX_SUPPLIES - 1; }
    bool isEmpty() const { return top == -1; }

    void clear() { top = -1; }

    bool push(const Supply& s) {
        if (isFull()) return false;
        top++;
        data[top] = s;
        return true;
    }

    bool pop(Supply& out) {
        if (isEmpty()) return false;
        out = data[top];
        top--;
        return true;
    }

    void print() const {
        if (isEmpty()) {
            cout << "No supplies available.\n";
            return;
        }
        cout << left << setw(16) << "Type"
             << setw(10) << "Qty"
             << "Batch" << "\n";
        line();
        for (int i = top; i >= 0; --i) {
            cout << left << setw(16) << data[i].type
                 << setw(10) << data[i].quantity
                 << data[i].batch << "\n";
        }
    }

    // ---- save / load to text file ----
    // Format: 3 lines per record → type, quantity, batch
    void saveToFile(const char* filename) const {
        ofstream out(filename);
        if (!out) {
            cout << "[Error] Cannot open " << filename << " for writing.\n";
            return;
        }
        for (int i = 0; i <= top; ++i) {
            const Supply& s = data[i];
            out << s.type     << '\n'
                << s.quantity << '\n'
                << s.batch    << '\n';
        }
    }

    void loadFromFile(const char* filename) {
        ifstream in(filename);
        if (!in) {
            cout << "[Info] " << filename
                 << " not found. Starting with empty supplies.\n";
            clear();
            return;
        }
        clear();
        while (true) {
            string stype;
            if (!getline(in, stype)) break;
            if (stype.empty()) continue;

            int qty;
            if (!(in >> qty)) break;
            in.ignore(numeric_limits<streamsize>::max(), '\n');

            string sbatch;
            if (!getline(in, sbatch)) break;

            Supply s{};
            strncpy(s.type,  stype.c_str(),  sizeof(s.type)  - 1);
            s.quantity = qty;
            strncpy(s.batch, sbatch.c_str(), sizeof(s.batch) - 1);

            if (!push(s)) break;
        }
        cout << "[OK] Loaded supplies from " << filename
             << " (count=" << (top + 1) << ")\n";
    }
};

inline SupplyStack gSupplies;

// ------- UI + menu for Role 2 -------

inline void ui_add_supply(){
    if(gSupplies.isFull()){
        cout<<"Supply store is full.\n";
        return;
    }

    Supply s{};
    cout<<"Enter Supply Type: ";
    safe_getline(s.type, 30);

    // Quantity validation: must be a number and at least 1
    while(true){
        cout<<"Enter Quantity (>= 1): ";
        if(!(cin>>s.quantity)){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout<<"Invalid input. Please enter a number.\n";
            continue;
        }
        if(s.quantity < 1){
            cout<<"Quantity must be at least 1. Please try again.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        break;
    }

    cout<<"Enter Batch: ";
    safe_getline(s.batch, 20);

    if (gSupplies.push(s)) {
        cout << "Recorded (stack top).\n";
        gSupplies.saveToFile(SUPPLY_FILE);      // <-- now it saves when adding
    } else {
        cout << "Failed to add supply.\n";
    }
}


inline void ui_use_supply_by_type() {
    if (gSupplies.isEmpty()) {
        cout << "No supplies to use.\n";
        return;
    }

    // 1) Build a list of unique supply types currently in the stack
    char types[MAX_SUPPLIES][30];
    int typeCount = 0;

    for (int i = 0; i <= gSupplies.top; ++i) {
        bool exists = false;
        for (int j = 0; j < typeCount; ++j) {
            if (strcmp(gSupplies.data[i].type, types[j]) == 0) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            // add new type
            strncpy(types[typeCount], gSupplies.data[i].type, 29);
            types[typeCount][29] = '\0';
            typeCount++;
        }
    }

    if (typeCount == 0) {
        cout << "No supplies available.\n";
        return;
    }

    // 2) Show the list of available types
    cout << "Available supply types:\n";
    for (int i = 0; i < typeCount; ++i) {
        cout << "  " << (i + 1) << ") " << types[i] << "\n";
    }

    // 3) Ask user which type they want to use
    int choice;
    while (true) {
        cout << "Choose a type (1-" << typeCount << "): ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice < 1 || choice > typeCount) {
            cout << "Choice out of range. Try again.\n";
            continue;
        }
        break;
    }

    const char* wanted = types[choice - 1];

    // 4) Find the last-added batch of that type (search from top down)
    int index = -1;
    for (int i = gSupplies.top; i >= 0; --i) {
        if (strcmp(gSupplies.data[i].type, wanted) == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        // Very unlikely, but safe to handle
        cout << "Unexpected error: type not found.\n";
        return;
    }

    // 5) Remove that batch from the stack (cut from the middle)
    Supply used = gSupplies.data[index];

    for (int i = index; i < gSupplies.top; ++i) {
        gSupplies.data[i] = gSupplies.data[i + 1];
    }
    gSupplies.top--;

    cout << "Using supply: " << used.type
         << " x" << used.quantity
         << " (Batch: " << used.batch << ")\n";

    gSupplies.saveToFile(SUPPLY_FILE);
}

inline void menu_supplies() {
    while (true) {
        line('=');
        cout << "MEDICAL SUPPLY MANAGER (Stack)\n";
        line('=');
        cout << "1) Add Supply Stock (push)\n";
        cout << "2) Use Supply by Type (last batch of that type)\n";
        cout << "3) View Current Supplies\n";
        cout << "0) Back\n> ";

        int ch;
        if (!(cin >> ch)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (ch == 0) break;
        else if (ch == 1) ui_add_supply();
        else if (ch == 2) ui_use_supply_by_type();
        else if (ch == 3) gSupplies.print();
        else cout << "Invalid choice.\n";
    }
}

inline void load_supplies_from_file() {
    gSupplies.loadFromFile(SUPPLY_FILE);
}

#endif
