#ifndef PATIENT_HPP
#define PATIENT_HPP

#include "utils.hpp"

#define PATIENT_FILE "patients.txt"

// ==========================================================================
// ROLE 1: PATIENT ADMISSION CLERK (QUEUE)
// --------------------------------------------------------------------------
// This module manages the registration and discharge of patients using a
// FIRST-IN-FIRST-OUT (FIFO) queue implemented with a circular array.
//
// Data structure choice:
//   - We use an array-based circular queue (head, tail, count).
//   - This gives O(1) time complexity for enqueue and dequeue.
//   - It is suitable because patients must be treated in order of arrival.
// ==========================================================================

struct Patient {
    // Patient ID is stored as text so we can use IDs like "P0028"
    char id[16];        // was int, now text ID like P0028
    char name[50];      // patient name
    char condition[30]; // condition description (e.g., "Flu", "Checkup")
};

struct PatientQueue {
    // Fixed-size array storing patient records
    Patient data[MAX_PATIENTS];

    // head : index of the front element
    // tail : index where the next element will be inserted
    // count: current number of elements in the queue
    int head = 0;
    int tail = 0;
    int count = 0; // circular queue

    // Check if the queue is full
    bool isFull()  const { return count == MAX_PATIENTS; }

    // Check if the queue is empty
    bool isEmpty() const { return count == 0; }

    // Reset the queue to empty state
    void clear() { head = tail = count = 0; }

    // ----------------------------------------------------------------------
    // enqueue()
    // ----------------------------------------------------------------------
    // Purpose : Add a new patient to the tail of the queue.
    // Input   : p - Patient struct to be inserted.
    // Return  : true if successfully enqueued, false if the queue is full.
    // ----------------------------------------------------------------------
    bool enqueue(const Patient& p) {
        if (isFull()) return false;
        data[tail] = p;
        tail = (tail + 1) % MAX_PATIENTS; // move tail circularly
        count++;
        return true;
    }

    // ----------------------------------------------------------------------
    // dequeue()
    // ----------------------------------------------------------------------
    // Purpose : Remove the earliest (front) patient from the queue.
    // Output  : out - Patient that was removed from the queue.
    // Return  : true if successfully dequeued, false if the queue is empty.
    // ----------------------------------------------------------------------
    bool dequeue(Patient& out) {
        if (isEmpty()) return false;
        out = data[head];
        head = (head + 1) % MAX_PATIENTS; // move head circularly
        count--;
        return true;
    }

    // ----------------------------------------------------------------------
    // print()
    // ----------------------------------------------------------------------
    // Purpose : Display all patients currently in the queue in order from
    //           front (earliest) to back (latest).
    // ----------------------------------------------------------------------
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

    // ----------------------------------------------------------------------
    // saveToFile()
    // ----------------------------------------------------------------------
    // Purpose : Save the current queue of patients to a text file.
    // Format  : For each patient, 3 lines are written:
    //           line 1 -> id
    //           line 2 -> name
    //           line 3 -> condition
    // ----------------------------------------------------------------------
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

    // ----------------------------------------------------------------------
    // loadFromFile()
    // ----------------------------------------------------------------------
    // Purpose : Load patients from a text file into the queue.
    // Behavior:
    //   - If the file does not exist, the queue starts empty.
    //   - If the file exists, it reads records until EOF or the queue is full.
    // Format  : Must match saveToFile() format:
    //           id, name, condition (3 lines per patient).
    // ----------------------------------------------------------------------
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

            if (!enqueue(p)) break;            // stop if queue full
        }
        cout << "[OK] Loaded patients from " << filename
             << " (count=" << count << ")\n";
    }
};

// --------------------------------------------------------------------------
// Global patient queue instance
// --------------------------------------------------------------------------
// NOTE: If you changed this to `extern PatientQueue gPatients;` in the header
// and defined `PatientQueue gPatients;` in main.cpp, keep that style.
//
// In this version, we use an inline global variable (C++17 feature) so only
// one instance of gPatients exists across translation units.
// --------------------------------------------------------------------------
inline PatientQueue gPatients;

// ====================== UI FUNCTIONS FOR ROLE 1 ============================

// --------------------------------------------------------------------------
// ui_admit_patient()
// --------------------------------------------------------------------------
// Purpose : Interactively admit a new patient by asking the user for
//           Patient ID, Name, and Condition, then enqueueing into the queue.
// Behavior: If the queue is full, it prints an error message.
//           On success, it also saves the queue to PATIENT_FILE.
// --------------------------------------------------------------------------
inline void ui_admit_patient() {
    if (gPatients.isFull()) {
        cout << "Patient queue is full.\n";
        return;
    }
    Patient p{};

    cout << "Enter Patient ID (e.g., P0028): ";
    safe_getline(p.id, 16);      // no numeric check anymore (text ID)

    cout << "Enter Patient Name: ";
    safe_getline(p.name, 50);

    cout << "Enter Condition Type (e.g., Flu/Checkup): ";
    safe_getline(p.condition, 30);

    if (gPatients.enqueue(p)) {
        cout << "Admitted to queue.\n";
        gPatients.saveToFile(PATIENT_FILE);   // auto-save after change
    } else {
        cout << "Failed to admit.\n";
    }
}

// --------------------------------------------------------------------------
// ui_discharge_patient()
// --------------------------------------------------------------------------
// Purpose : Remove the earliest admitted patient from the queue and display
//           their details.
// Behavior: If the queue is empty, it prints an error message.
//           On success, it also saves the queue to PATIENT_FILE.
// --------------------------------------------------------------------------
inline void ui_discharge_patient() {
    Patient p{};
    if (gPatients.dequeue(p)) {
        cout << "Discharged earliest admitted patient: ["
             << p.id << "] " << p.name
             << " (" << p.condition << ")\n";
        gPatients.saveToFile(PATIENT_FILE);   // auto-save after discharge
    } else {
        cout << "No patients to discharge.\n";
    }
}

// --------------------------------------------------------------------------
// menu_patients()
// --------------------------------------------------------------------------
// Purpose : Display the sub-menu for the Patient Admission Clerk role and
//           allow the user to perform operations on the patient queue.
// Options :
//   1) Admit Patient
//   2) Discharge Patient (earliest)
//   3) View Patient Queue
//   0) Back (return to main menu)
// --------------------------------------------------------------------------
inline void menu_patients() {
    while (true) {
        line('=');
        cout << "PATIENT ADMISSION CLERK (FIFO)\n";
        line('=');
        cout << "1) Admit Patient\n";
        cout << "2) Discharge Patient (earliest)\n";
        cout << "3) View Patient Queue\n";
        cout << "0) Back\n> ";

        int ch;
        if (!(cin >> ch)) {
            // Input validation: if not an integer, clear and retry
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        // Remove extra characters (like '\n') from the buffer
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (ch == 0) break;                 // return to main menu
        else if (ch == 1) ui_admit_patient();
        else if (ch == 2) ui_discharge_patient();
        else if (ch == 3) gPatients.print();
        else cout << "Invalid choice.\n";
    }
}

// --------------------------------------------------------------------------
// load_patients_from_file()
// --------------------------------------------------------------------------
// Convenience wrapper for main.cpp to load patients into the global queue
// from PATIENT_FILE at program startup.
// --------------------------------------------------------------------------
inline void load_patients_from_file() {
    gPatients.loadFromFile(PATIENT_FILE);
}

#endif
