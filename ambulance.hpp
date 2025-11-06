#ifndef AMBULANCE_HPP
#define AMBULANCE_HPP

#include "utils.hpp"

#define AMB_FILE "ambulances.txt"

// ==========================================================================
// ROLE 4: AMBULANCE DISPATCHER (CIRCULAR QUEUE)
// --------------------------------------------------------------------------
// This module manages the scheduling of ambulances using a CIRCULAR QUEUE
// implemented with a fixed-size array.
//
// Data structure choice:
//   - We use an array-based circular queue (head, tail, count).
//   - This gives O(1) enqueue and dequeue operations.
//   - It is suitable because ambulances are rotated in a round-robin fashion,
//     so after the first ambulance serves, it goes to the back of the queue.
// ==========================================================================

struct Ambulance {
    char plate[16];   // Ambulance plate number or ID (e.g., "AMB-101")
};

struct AmbulanceCQueue {
    // Fixed-size array storing ambulances on active duty
    Ambulance data[MAX_AMBULANCES];

    // head  : index of the front ambulance (currently first in rotation)
    // tail  : index where the next ambulance will be inserted
    // count : number of ambulances currently in the queue
    int head  = 0;
    int tail  = 0;
    int count = 0;

    // Check if the circular queue is full
    bool isFull()  const { return count == MAX_AMBULANCES; }

    // Check if the circular queue is empty
    bool isEmpty() const { return count == 0; }

    // Reset the circular queue to empty state
    void clear() { head = tail = count = 0; }

    // ----------------------------------------------------------------------
    // enqueue()
    // ----------------------------------------------------------------------
    // Purpose : Add an ambulance to the end (tail) of the circular queue.
    // Input   : a - Ambulance to insert.
    // Return  : true if successful, false if the queue is full.
    // ----------------------------------------------------------------------
    bool enqueue(const Ambulance& a) {
        if (isFull()) return false;
        data[tail] = a;
        tail = (tail + 1) % MAX_AMBULANCES; // move tail circularly
        count++;
        return true;
    }

    // ----------------------------------------------------------------------
    // dequeue()
    // ----------------------------------------------------------------------
    // Purpose : Remove the ambulance at the front (head) of the queue.
    // Output  : out - Ambulance that was removed.
    // Return  : true if successful, false if the queue is empty.
    // ----------------------------------------------------------------------
    bool dequeue(Ambulance& out) {
        if (isEmpty()) return false;
        out = data[head];
        head = (head + 1) % MAX_AMBULANCES; // move head circularly
        count--;
        return true;
    }

    // ----------------------------------------------------------------------
    // rotateOnce()
    // ----------------------------------------------------------------------
    // Purpose : Rotate the ambulance schedule so that the first ambulance
    //           moves to the back of the queue.
    // Behavior:
    //   - If there is 0 or 1 ambulance, rotation has no effect.
    //   - Otherwise, we dequeue the head and enqueue it at the tail.
    // ----------------------------------------------------------------------
    void rotateOnce() {
        if (count <= 1) return;
        Ambulance first{};
        dequeue(first);
        enqueue(first);
    }

    // ----------------------------------------------------------------------
    // print()
    // ----------------------------------------------------------------------
    // Purpose : Display the current rotation order of ambulances from head
    //           to tail.
    // ----------------------------------------------------------------------
    void print() const {
        if (isEmpty()) {
            cout << "No ambulances registered.\n";
            return;
        }
        cout << "Rotation Order (head -> tail):\n";
        line();
        for (int i = 0; i < count; ++i) {
            int idx = (head + i) % MAX_AMBULANCES;
            cout << (i + 1) << ". " << data[idx].plate << "\n";
        }
    }

    // ----------------------------------------------------------------------
    // saveToFile()
    // ----------------------------------------------------------------------
    // Purpose : Save the current circular queue of ambulances to a text file.
    // Format  : One line per ambulance plate/ID, in current rotation order.
    // ----------------------------------------------------------------------
    void saveToFile(const char* filename) const {
        ofstream out(filename);
        if (!out) {
            cout << "[Error] Cannot open " << filename << " for writing.\n";
            return;
        }
        for (int i = 0; i < count; ++i) {
            int idx = (head + i) % MAX_AMBULANCES;
            out << data[idx].plate << '\n';
        }
    }

    // ----------------------------------------------------------------------
    // loadFromFile()
    // ----------------------------------------------------------------------
    // Purpose : Load ambulances from a text file into the circular queue.
    // Behavior:
    //   - If the file does not exist, the queue starts empty.
    //   - If the file exists, each non-empty line is treated as a plate/ID.
    // Format  : One ambulance plate/ID per line in the text file.
    // ----------------------------------------------------------------------
    void loadFromFile(const char* filename) {
        ifstream in(filename);
        if (!in) {
            cout << "[Info] " << filename
                 << " not found. Starting with empty ambulances.\n";
            clear();
            return;
        }
        clear();
        string plate;
        while (getline(in, plate)) {
            if (plate.empty()) continue;
            Ambulance a{};
            strncpy(a.plate, plate.c_str(), sizeof(a.plate) - 1);
            if (!enqueue(a)) break; // stop if the queue is full
        }
        cout << "[OK] Loaded ambulances from " << filename
             << " (count=" << count << ")\n";
    }
};

// --------------------------------------------------------------------------
// Global ambulance circular queue instance
// --------------------------------------------------------------------------
// We use an inline global variable so that the same circular queue is shared
// across all functions in this header and in main.cpp.
// --------------------------------------------------------------------------
inline AmbulanceCQueue gAmb;

// ====================== UI FUNCTIONS FOR ROLE 4 ============================

// --------------------------------------------------------------------------
// ui_register_ambulance()
// --------------------------------------------------------------------------
// Purpose : Interactively register a new ambulance by plate/ID and add it
//           into the circular queue.
// Steps   :
//   1) Check if queue is full.
//   2) Ask user to enter the ambulance plate/ID.
//   3) Enqueue it and save to AMB_FILE on success.
// --------------------------------------------------------------------------
inline void ui_register_ambulance() {
    if (gAmb.isFull()) {
        cout << "Ambulance roster full.\n";
        return;
    }

    Ambulance a{};
    cout << "Enter Ambulance Plate/ID: ";
    safe_getline(a.plate, 16);

    if (gAmb.enqueue(a)) {
        cout << "Ambulance added to active-duty list.\n";
        gAmb.saveToFile(AMB_FILE);
    } else {
        cout << "Failed to register.\n";
    }
}

// --------------------------------------------------------------------------
// ui_rotate_shift()
// --------------------------------------------------------------------------
// Purpose : Rotate the ambulance shift so that the next ambulance moves to
//           the front of the rotation.
// Steps   :
//   1) Check if queue is empty.
//   2) Call rotateOnce() to move head to tail.
//   3) Save the updated order to AMB_FILE.
// --------------------------------------------------------------------------
inline void ui_rotate_shift() {
    if (gAmb.isEmpty()) {
        cout << "No ambulances to rotate.\n";
        return;
    }
    gAmb.rotateOnce();
    cout << "Shift rotated. Next up is now at head.\n";
    gAmb.saveToFile(AMB_FILE);
}

// --------------------------------------------------------------------------
// menu_ambulance()
// --------------------------------------------------------------------------
// Purpose : Display the sub-menu for the Ambulance Dispatcher role and allow
//           the user to operate on the circular queue of ambulances.
// Options :
//   1) Register Ambulance (enqueue)
//   2) Rotate Ambulance Shift
//   3) Display Ambulance Schedule
//   0) Back (return to main menu)
// --------------------------------------------------------------------------
inline void menu_ambulance() {
    while (true) {
        line('=');
        cout << "AMBULANCE DISPATCHER (Circular Queue)\n";
        line('=');
        cout << "1) Register Ambulance (enqueue)\n";
        cout << "2) Rotate Ambulance Shift\n";
        cout << "3) Display Ambulance Schedule\n";
        cout << "0) Back\n> ";

        int ch;
        if (!(cin >> ch)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (ch == 0) break;
        else if (ch == 1) ui_register_ambulance();
        else if (ch == 2) ui_rotate_shift();
        else if (ch == 3) gAmb.print();
        else cout << "Invalid choice.\n";
    }
}

// --------------------------------------------------------------------------
// load_ambulances_from_file()
// --------------------------------------------------------------------------
// Convenience wrapper for main.cpp to load ambulances into the global
// circular queue from AMB_FILE at program startup.
// --------------------------------------------------------------------------
inline void load_ambulances_from_file() {
    gAmb.loadFromFile(AMB_FILE);
}

#endif
