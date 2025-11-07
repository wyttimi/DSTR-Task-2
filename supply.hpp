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
//   - Each element represents one batch of a particular supply type.
//   - "Use Last Added Supply" simply pops the most recently added batch.
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
    // Format  : type, quantity, batch (3 lines per supply).
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

// Global stack instance (C++17 inline variable)
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
// ui_use_last_supply()
// --------------------------------------------------------------------------
// Purpose : Use the last added (top) supply batch.
// Steps   :
//   1) Check if stack is empty.
//   2) Pop the top element.
//   3) Show what was used and save updated stack to SUPPLY_FILE.
// --------------------------------------------------------------------------
inline void ui_use_last_supply() {
    if (gSupplies.isEmpty()) {
        cout << "No supplies to use.\n";
        return;
    }

    Supply used{};
    if (!gSupplies.pop(used)) {
        cout << "Failed to use supply.\n";
        return;
    }

    cout << "Using last added supply batch:\n";
    cout << "  Type : " << used.type << "\n";
    cout << "  Qty  : " << used.quantity << "\n";
    cout << "  Batch: " << used.batch << "\n";

    gSupplies.saveToFile(SUPPLY_FILE);
}

// --------------------------------------------------------------------------
// menu_supplies()
// --------------------------------------------------------------------------
// Purpose : Display the sub-menu for the Medical Supply Manager role and
//           allow the user to operate on the supply stack.
// Options :
//   1) Add Supply Stock (push)
//   2) Use 'Last Added' Supply (pop)
//   3) View Current Supplies
//   0) Back (return to main menu)
// --------------------------------------------------------------------------
inline void menu_supplies() {
    while (true) {
        line('=');
        cout << "MEDICAL SUPPLY MANAGER (Stack)\n";
        line('=');
        cout << "1) Add Supply Stock (push)\n";
        cout << "2) Use 'Last Added' Supply (pop)\n";
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
        else if (ch == 2) ui_use_last_supply();
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
