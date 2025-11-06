#ifndef SUPPLY_HPP
#define SUPPLY_HPP

#include "utils.hpp"

#define SUPPLY_FILE "supplies.txt"

// ==========================================================================
// ROLE 2: MEDICAL SUPPLY MANAGER (STACK)
// --------------------------------------------------------------------------
// This module manages medical supplies using a LIFO (Last-In-First-Out)
// STACK implemented with a fixed-size array.
//
// Data structure choice:
//   - We use an array-based stack (top index).
//   - This gives O(1) push (add supply) and pop (use supply).
//   - Conceptually, each record represents a "batch" of a certain supply type.
//   - We also allow choosing a supply type, then using the most recently
//     added batch of that type (searching from top down).
// ==========================================================================

struct Supply {
    char type[30];   // Name of supply type (e.g. "Surgical Masks")
    int  quantity;   // Quantity in this batch (must be >= 1)
    char batch[20];  // Batch identifier (e.g. "MASK-BATCH-001")
};

struct SupplyStack {
    // Fixed-size array to store supply batches
    Supply data[MAX_SUPPLIES];

    // top index: -1 means the stack is empty.
    // When we push(), we increment top and store at data[top].
    // When we pop(), we read data[top] and decrement top.
    int top = -1; // -1 means empty

    // Check if the stack is full
    bool isFull()  const { return top == MAX_SUPPLIES - 1; }

    // Check if the stack is empty
    bool isEmpty() const { return top == -1; }

    // Reset stack to empty state
    void clear() { top = -1; }

    // ----------------------------------------------------------------------
    // push()
    // ----------------------------------------------------------------------
    // Purpose : Add a new supply batch to the top of the stack.
    // Input   : s - Supply struct to be pushed.
    // Return  : true if successful, false if the stack is full.
    // ----------------------------------------------------------------------
    bool push(const Supply& s) {
        if (isFull()) return false;
        top++;
        data[top] = s;
        return true;
    }

    // ----------------------------------------------------------------------
    // pop()
    // ----------------------------------------------------------------------
    // Purpose : Remove the last added (top) supply batch from the stack.
    // Output  : out - Supply that was removed.
    // Return  : true if successful, false if the stack is empty.
    // NOTE    : This basic pop is not used for "use by type" but is kept
    //           for completeness of the stack implementation.
    // ----------------------------------------------------------------------
    bool pop(Supply& out) {
        if (isEmpty()) return false;
        out = data[top];
        top--;
        return true;
    }

    // ----------------------------------------------------------------------
    // print()
    // ----------------------------------------------------------------------
    // Purpose : Display all supply batches currently stored in the stack,
    //           starting from the top (most recently added).
    // ----------------------------------------------------------------------
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

    // ----------------------------------------------------------------------
    // saveToFile()
    // ----------------------------------------------------------------------
    // Purpose : Save the contents of the supply stack to a text file.
    // Format  : For each supply batch, 3 lines are written:
    //           line 1 -> type
    //           line 2 -> quantity
    //           line 3 -> batch
    // Note    : Batches are written from index 0 up to top (bottom to top).
    // ----------------------------------------------------------------------
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

    // ----------------------------------------------------------------------
    // loadFromFile()
    // ----------------------------------------------------------------------
    // Purpose : Load supply batches from a text file into the stack.
    // Behavior:
    //   - If the file does not exist, the stack starts empty.
    //   - If the file exists, it reads records until EOF or the stack is full.
    // Format  : Must match saveToFile() format:
    //           type, quantity, batch (3 lines per supply).
    // ----------------------------------------------------------------------
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

// --------------------------------------------------------------------------
// Global supply stack instance
// --------------------------------------------------------------------------
// As with patients, we use an inline global variable here (C++17 feature)
// so that all functions in this header operate on the same stack instance.
// --------------------------------------------------------------------------
inline SupplyStack gSupplies;

// ====================== UI FUNCTIONS FOR ROLE 2 ============================

// --------------------------------------------------------------------------
// ui_add_supply()
// --------------------------------------------------------------------------
// Purpose : Interactively add a new supply batch to the stack.
// Steps   :
//   1) Ask for supply type (text).
//   2) Ask for quantity with validation (must be a number >= 1).
//   3) Ask for batch ID.
//   4) Push the record onto the stack and save to SUPPLY_FILE.
// --------------------------------------------------------------------------
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
        gSupplies.saveToFile(SUPPLY_FILE);    // auto-save after adding
    } else {
        cout << "Failed to add supply.\n";
    }
}


// --------------------------------------------------------------------------
// ui_use_supply_by_type()
// --------------------------------------------------------------------------
// Purpose : Let the user choose a supply type and then use the most recently
//           added batch of that type.
//
// Logic   :
//   1) Build a list of UNIQUE supply types currently in the stack.
//   2) Display them as a numbered menu.
//   3) Ask the user to choose one.
//   4) Search from TOP downwards to find the last-added batch of that type.
//   5) Remove that batch from the stack (shift elements to close the gap).
//   6) Display the used batch and save the updated stack to SUPPLY_FILE.
// --------------------------------------------------------------------------
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

    // Save the updated stack to file after using a batch
    gSupplies.saveToFile(SUPPLY_FILE);
}

// --------------------------------------------------------------------------
// menu_supplies()
// --------------------------------------------------------------------------
// Purpose : Display the sub-menu for the Medical Supply Manager role and
//           allow the user to perform operations on the supply stack.
// Options :
//   1) Add Supply Stock (push)
//   2) Use Supply by Type (uses last-added batch of chosen type)
//   3) View Current Supplies
//   0) Back (return to main menu)
// --------------------------------------------------------------------------
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

// --------------------------------------------------------------------------
// load_supplies_from_file()
// --------------------------------------------------------------------------
// Convenience wrapper for main.cpp to load supplies into the global stack
// from SUPPLY_FILE at program startup.
// --------------------------------------------------------------------------
inline void load_supplies_from_file() {
    gSupplies.loadFromFile(SUPPLY_FILE);
}

#endif
