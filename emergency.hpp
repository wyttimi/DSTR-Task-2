#ifndef EMERGENCY_HPP
#define EMERGENCY_HPP

#include "utils.hpp"

#define EMERG_FILE "emergencies.txt"

// ==========================================================================
// ROLE 3: EMERGENCY DEPARTMENT OFFICER (PRIORITY QUEUE)
// --------------------------------------------------------------------------
// This module manages emergency cases using a PRIORITY QUEUE implemented
// as a BINARY MAX-HEAP stored in an array.
//
// Data structure choice:
//   - We use an array-based binary heap (1-based index).
//   - Each node is an EmergencyCase, and the parent always has priority
//     >= its children.
//   - This allows us to always process the highest-priority emergency in
//     O(log n) time for insertion and removal, and O(1) time to access the
//     most critical case at the root (index 1).
// ==========================================================================

struct EmergencyCase {
    char patient[50]; // Name of the patient
    char type[40];    // Type of emergency (e.g., "Heart Attack")
    int  priority;    // Priority level (higher = more critical)
};

struct EmergencyMaxHeap {
    // We use 1-based indexing for simpler parent/child calculations:
    //   parent(i) = i / 2
    //   left(i)   = 2 * i
    //   right(i)  = 2 * i + 1
    EmergencyCase data[MAX_EMERG + 1]; // 1-based index
    int sz = 0; // current number of elements in the heap

    // Check if the heap is full
    bool isFull()  const { return sz == MAX_EMERG; }

    // Check if the heap is empty
    bool isEmpty() const { return sz == 0; }

    // Reset heap to empty
    void clear() { sz = 0; }

    // Simple swap helper for EmergencyCase
    void swapCase(EmergencyCase& a, EmergencyCase& b) {
        EmergencyCase tmp = a;
        a = b;
        b = tmp;
    }

    // ----------------------------------------------------------------------
    // push()
    // ----------------------------------------------------------------------
    // Purpose : Insert a new emergency case into the max-heap.
    // Steps   :
    //   1) Add the new element at the end (data[++sz]).
    //   2) "Sift up" (heapify up) while its priority is greater than
    //      its parent's priority.
    // Behavior:
    //   - If the heap is full, print an error message and do nothing.
    // ----------------------------------------------------------------------
    void push(const EmergencyCase& e) {
        if (isFull()) {
            cout << "Emergency queue is full.\n";
            return;
        }
        sz++;
        data[sz] = e;

        int i = sz;
        // Sift up: maintain max-heap property
        while (i > 1 && data[i].priority > data[i / 2].priority) {
            swapCase(data[i], data[i / 2]);
            i = i / 2;
        }
    }

    // ----------------------------------------------------------------------
    // top()
    // ----------------------------------------------------------------------
    // Purpose : Return the most critical emergency case (root of heap).
    // Note    : Caller should check isEmpty() before calling top().
    // ----------------------------------------------------------------------
    EmergencyCase top() {
        return data[1];
    }

    // ----------------------------------------------------------------------
    // pop()
    // ----------------------------------------------------------------------
    // Purpose : Remove the most critical emergency case from the heap.
    // Steps   :
    //   1) Replace root (index 1) with the last element (data[sz]).
    //   2) Decrease sz.
    //   3) "Sift down" (heapify down) to restore max-heap property.
    // Behavior:
    //   - If the heap is empty, do nothing.
    // ----------------------------------------------------------------------
    void pop() {
        if (isEmpty()) return;
        data[1] = data[sz];
        sz--;

        int i = 1;
        while (true) {
            int left  = 2 * i;
            int right = 2 * i + 1;
            int largest = i;

            if (left  <= sz && data[left].priority  > data[largest].priority)
                largest = left;
            if (right <= sz && data[right].priority > data[largest].priority)
                largest = right;
            if (largest == i) break;

            swapCase(data[i], data[largest]);
            i = largest;
        }
    }

    // ----------------------------------------------------------------------
    // print()
    // ----------------------------------------------------------------------
    // Purpose : Display all emergency cases currently in the heap array.
    // Note    :
    //   - The internal order is by heap structure, NOT by sorted order.
    //   - However, the root (index 1) is always the highest-priority case.
    // ----------------------------------------------------------------------
    void print() const {
        if (isEmpty()) {
            cout << "No emergency cases pending.\n";
            return;
        }
        cout << left << setw(22) << "Patient"
             << setw(18) << "Emergency"
             << "Priority" << "\n";
        line();
        for (int i = 1; i <= sz; ++i) {
            cout << left << setw(22) << data[i].patient
                 << setw(18) << data[i].type
                 << data[i].priority << "\n";
        }
        cout << "(Highest priority case is always processed first.)\n";
    }

    // ----------------------------------------------------------------------
    // saveToFile()
    // ----------------------------------------------------------------------
    // Purpose : Save the heap contents to a text file.
    // Format  : For each emergency case, 3 lines are written:
    //           line 1 -> patient name
    //           line 2 -> emergency type
    //           line 3 -> priority (integer)
    // Note    : The order in the file is the current heap array order.
    // ----------------------------------------------------------------------
    void saveToFile(const char* filename) const {
        ofstream out(filename);
        if (!out) {
            cout << "[Error] Cannot open " << filename << " for writing.\n";
            return;
        }
        for (int i = 1; i <= sz; ++i) {
            const EmergencyCase& e = data[i];
            out << e.patient  << '\n'
                << e.type     << '\n'
                << e.priority << '\n';
        }
    }

    // ----------------------------------------------------------------------
    // loadFromFile()
    // ----------------------------------------------------------------------
    // Purpose : Load emergency cases from a text file into the heap.
    // Behavior:
    //   - If the file does not exist, the heap starts empty.
    //   - If the file exists, it reads records and inserts each case using
    //     push(), which restores heap order automatically.
    // Format  : Must match saveToFile() format:
    //           patient, type, priority (3 lines per case).
    // ----------------------------------------------------------------------
    void loadFromFile(const char* filename) {
        ifstream in(filename);
        if (!in) {
            cout << "[Info] " << filename
                 << " not found. Starting with empty emergencies.\n";
            clear();
            return;
        }
        clear();
        while (true) {
            string spatient;
            if (!getline(in, spatient)) break;
            if (spatient.empty()) continue;

            string stype;
            if (!getline(in, stype)) break;

            int prio;
            if (!(in >> prio)) break;
            in.ignore(numeric_limits<streamsize>::max(), '\n');

            EmergencyCase e{};
            strncpy(e.patient, spatient.c_str(), sizeof(e.patient) - 1);
            strncpy(e.type,    stype.c_str(),    sizeof(e.type)    - 1);
            e.priority = prio;

            push(e); // uses heap push, keeps order correct
        }
        cout << "[OK] Loaded emergencies from " << filename
             << " (count=" << sz << ")\n";
    }
};

// --------------------------------------------------------------------------
// Global emergency max-heap instance
// --------------------------------------------------------------------------
// As with other roles, we use an inline global variable here (C++17 feature)
// so all UI/menu functions operate on the same emergency priority queue.
// --------------------------------------------------------------------------
inline EmergencyMaxHeap gEmerg;

// ====================== UI FUNCTIONS FOR ROLE 3 ============================

// --------------------------------------------------------------------------
// ui_log_emergency()
// --------------------------------------------------------------------------
// Purpose : Interactively log a new emergency case.
// Steps   :
//   1) Ask for patient name.
//   2) Ask for type of emergency.
//   3) Ask for a priority level (1-10, higher = more critical).
//   4) Insert into the max-heap using push().
//   5) Save to EMERG_FILE.
// --------------------------------------------------------------------------
inline void ui_log_emergency() {
    if (gEmerg.isFull()) {
        cout << "Emergency list full.\n";
        return;
    }

    EmergencyCase e{};
    cout << "Patient Name: ";
    safe_getline(e.patient, 50);

    cout << "Type of Emergency: ";
    safe_getline(e.type, 40);

    cout << "Priority Level (1-10, higher is more critical): ";
    while (!(cin >> e.priority)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter a valid number for priority: ";
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // Clamp priority to a safe range (0 to 100)
    if (e.priority < 0)   e.priority = 0;
    if (e.priority > 100) e.priority = 100;

    gEmerg.push(e);
    cout << "Emergency logged.\n";
    gEmerg.saveToFile(EMERG_FILE);
}

// --------------------------------------------------------------------------
// ui_process_most_critical()
// --------------------------------------------------------------------------
// Purpose : Process (remove) the highest-priority emergency case.
// Steps   :
//   1) Check if heap is empty; if so, show a message.
//   2) Get the root (most critical) case using top().
//   3) Remove it using pop().
//   4) Display the processed case and save to file.
// --------------------------------------------------------------------------
inline void ui_process_most_critical() {
    if (gEmerg.isEmpty()) {
        cout << "No emergencies in queue.\n";
        return;
    }
    EmergencyCase top = gEmerg.top();
    gEmerg.pop();
    cout << "ATTEND MOST CRITICAL => "
         << top.patient << " (" << top.type
         << ") with priority " << top.priority << "\n";
    gEmerg.saveToFile(EMERG_FILE);
}

// --------------------------------------------------------------------------
// menu_emergency()
// --------------------------------------------------------------------------
// Purpose : Display the sub-menu for the Emergency Department Officer role
//           and allow the user to operate on the emergency priority queue.
// Options :
//   1) Log Emergency Case (insert into heap)
//   2) Process Most Critical Case (remove max)
//   3) View Pending Emergency Cases
//   0) Back (return to main menu)
// --------------------------------------------------------------------------
inline void menu_emergency() {
    while (true) {
        line('=');
        cout << "EMERGENCY DEPT OFFICER (Priority Queue - Max Heap)\n";
        line('=');
        cout << "1) Log Emergency Case (push)\n";
        cout << "2) Process Most Critical Case (pop-max)\n";
        cout << "3) View Pending Emergency Cases\n";
        cout << "0) Back\n> ";

        int ch;
        if (!(cin >> ch)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (ch == 0) break;
        else if (ch == 1) ui_log_emergency();
        else if (ch == 2) ui_process_most_critical();
        else if (ch == 3) gEmerg.print();
        else cout << "Invalid choice.\n";
    }
}

// --------------------------------------------------------------------------
// load_emergencies_from_file()
// --------------------------------------------------------------------------
// Convenience wrapper for main.cpp to load emergencies into the global
// max-heap from EMERG_FILE at program startup.
// --------------------------------------------------------------------------
inline void load_emergencies_from_file() {
    gEmerg.loadFromFile(EMERG_FILE);
}

#endif
