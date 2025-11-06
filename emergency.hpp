#ifndef EMERGENCY_HPP
#define EMERGENCY_HPP

#include "utils.hpp"

#define EMERG_FILE "emergencies.txt"

// ========== ROLE 3: EMERGENCY DEPARTMENT OFFICER (PRIORITY QUEUE) ==========

struct EmergencyCase {
    char patient[50];
    char type[40];
    int  priority;  // higher = more critical
};

struct EmergencyMaxHeap {
    EmergencyCase data[MAX_EMERG + 1]; // 1-based index
    int sz = 0;

    bool isFull()  const { return sz == MAX_EMERG; }
    bool isEmpty() const { return sz == 0; }

    void clear() { sz = 0; }

    void swapCase(EmergencyCase& a, EmergencyCase& b) {
        EmergencyCase tmp = a;
        a = b;
        b = tmp;
    }

    void push(const EmergencyCase& e) {
        if (isFull()) {
            cout << "Emergency queue is full.\n";
            return;
        }
        sz++;
        data[sz] = e;

        int i = sz;
        while (i > 1 && data[i].priority > data[i / 2].priority) {
            swapCase(data[i], data[i / 2]);
            i = i / 2;
        }
    }

    EmergencyCase top() {
        return data[1];
    }

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

    // ---- save / load to text file ----
    // Format: 3 lines per case → patient, type, priority
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

inline EmergencyMaxHeap gEmerg;

// ------- UI + menu for Role 3 -------

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

    if (e.priority < 0)   e.priority = 0;
    if (e.priority > 100) e.priority = 100;

    gEmerg.push(e);
    cout << "Emergency logged.\n";
    gEmerg.saveToFile(EMERG_FILE);
}

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

inline void load_emergencies_from_file() {
    gEmerg.loadFromFile(EMERG_FILE);
}

#endif
