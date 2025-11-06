#ifndef AMBULANCE_HPP
#define AMBULANCE_HPP

#include "utils.hpp"

#define AMB_FILE "ambulances.txt"

// ========== ROLE 4: AMBULANCE DISPATCHER (CIRCULAR QUEUE) ==========

struct Ambulance {
    char plate[16];
};

struct AmbulanceCQueue {
    Ambulance data[MAX_AMBULANCES];
    int head  = 0;
    int tail  = 0;
    int count = 0;

    bool isFull()  const { return count == MAX_AMBULANCES; }
    bool isEmpty() const { return count == 0; }

    void clear() { head = tail = count = 0; }

    bool enqueue(const Ambulance& a) {
        if (isFull()) return false;
        data[tail] = a;
        tail = (tail + 1) % MAX_AMBULANCES;
        count++;
        return true;
    }

    bool dequeue(Ambulance& out) {
        if (isEmpty()) return false;
        out = data[head];
        head = (head + 1) % MAX_AMBULANCES;
        count--;
        return true;
    }

    void rotateOnce() {
        if (count <= 1) return;
        Ambulance first{};
        dequeue(first);
        enqueue(first);
    }

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

    // ---- save / load to text file ----
    // Format: one line per plate
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
            if (!enqueue(a)) break;
        }
        cout << "[OK] Loaded ambulances from " << filename
             << " (count=" << count << ")\n";
    }
};

inline AmbulanceCQueue gAmb;

// ------- UI + menu for Role 4 -------

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

inline void ui_rotate_shift() {
    if (gAmb.isEmpty()) {
        cout << "No ambulances to rotate.\n";
        return;
    }
    gAmb.rotateOnce();
    cout << "Shift rotated. Next up is now at head.\n";
    gAmb.saveToFile(AMB_FILE);
}

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

inline void load_ambulances_from_file() {
    gAmb.loadFromFile(AMB_FILE);
}

#endif
