#ifndef PATIENT_HPP
#define PATIENT_HPP

#include "utils.hpp"

#define PATIENT_FILE "patients.txt"

// ========== ROLE 1: PATIENT ADMISSION CLERK (QUEUE) ==========

struct Patient {
    char id[16];        // was int, now text ID like P0028
    char name[50];
    char condition[30];
};

struct PatientQueue {
    Patient data[MAX_PATIENTS];
    int head = 0;
    int tail = 0;
    int count = 0; // circular queue

    bool isFull()  const { return count == MAX_PATIENTS; }
    bool isEmpty() const { return count == 0; }

    void clear() { head = tail = count = 0; }

    bool enqueue(const Patient& p) {
        if (isFull()) return false;
        data[tail] = p;
        tail = (tail + 1) % MAX_PATIENTS;
        count++;
        return true;
    }

    bool dequeue(Patient& out) {
        if (isEmpty()) return false;
        out = data[head];
        head = (head + 1) % MAX_PATIENTS;
        count--;
        return true;
    }

    void print() const {
        if (isEmpty()) {
            cout << "No patients waiting.\n";
            return;
        }
        cout << left << setw(12) << "ID"
             << setw(22) << "Name"
             << "Condition" << "\n";
        line();
        for (int i = 0; i < count; ++i) {
            int idx = (head + i) % MAX_PATIENTS;
            cout << left << setw(12) << data[idx].id
                 << setw(22) << data[idx].name
                 << data[idx].condition << "\n";
        }
    }

    // ---- save / load to text file ----
    // Format: 3 lines per patient → id, name, condition (all text)
    void saveToFile(const char* filename) const {
        ofstream out(filename);
        if (!out) {
            cout << "[Error] Cannot open " << filename << " for writing.\n";
            return;
        }
        for (int i = 0; i < count; ++i) {
            int idx = (head + i) % MAX_PATIENTS;
            const Patient& p = data[idx];
            out << p.id        << '\n'
                << p.name      << '\n'
                << p.condition << '\n';
        }
    }

    void loadFromFile(const char* filename) {
        ifstream in(filename);
        if (!in) {
            cout << "[Info] " << filename
                 << " not found. Starting with empty patient queue.\n";
            clear();
            return;
        }
        clear();
        while (true) {
            string sid;
            if (!getline(in, sid)) break;      // no more patients
            if (sid.empty()) continue;         // skip empty lines

            string sname, scond;
            if (!getline(in, sname)) break;
            if (!getline(in, scond)) break;

            Patient p{};
            strncpy(p.id,        sid.c_str(),   sizeof(p.id)        - 1);
            strncpy(p.name,      sname.c_str(), sizeof(p.name)      - 1);
            strncpy(p.condition, scond.c_str(), sizeof(p.condition) - 1);

            if (!enqueue(p)) break;            // queue full
        }
        cout << "[OK] Loaded patients from " << filename
             << " (count=" << count << ")\n";
    }
};

// NOTE: if you previously changed this to `extern PatientQueue gPatients;`
// and defined `PatientQueue gPatients;` in main.cpp, keep that pattern.
// Otherwise this single-line version also works with C++17:
inline PatientQueue gPatients;

// ------- UI + menu for Role 1 -------

inline void ui_admit_patient() {
    if (gPatients.isFull()) {
        cout << "Patient queue is full.\n";
        return;
    }
    Patient p{};

    cout << "Enter Patient ID (e.g., P0028): ";
    safe_getline(p.id, 16);      // no numeric check anymore

    cout << "Enter Patient Name: ";
    safe_getline(p.name, 50);

    cout << "Enter Condition Type (e.g., Flu/Checkup): ";
    safe_getline(p.condition, 30);

    if (gPatients.enqueue(p)) {
        cout << "Admitted to queue.\n";
        gPatients.saveToFile(PATIENT_FILE);
    } else {
        cout << "Failed to admit.\n";
    }
}

inline void ui_discharge_patient() {
    Patient p{};
    if (gPatients.dequeue(p)) {
        cout << "Discharged earliest admitted patient: ["
             << p.id << "] " << p.name
             << " (" << p.condition << ")\n";
        gPatients.saveToFile(PATIENT_FILE);
    } else {
        cout << "No patients to discharge.\n";
    }
}

inline void menu_patients() {
    while (true) {
        line('=');
        cout << "PATIENT ADMISSION CLERK (Queue)\n";
        line('=');
        cout << "1) Admit Patient\n";
        cout << "2) Discharge Patient (earliest)\n";
        cout << "3) View Patient Queue\n";
        cout << "0) Back\n> ";

        int ch;
        if (!(cin >> ch)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (ch == 0) break;
        else if (ch == 1) ui_admit_patient();
        else if (ch == 2) ui_discharge_patient();
        else if (ch == 3) gPatients.print();
        else cout << "Invalid choice.\n";
    }
}

inline void load_patients_from_file() {
    gPatients.loadFromFile(PATIENT_FILE);
}

#endif
