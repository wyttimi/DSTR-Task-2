#include "patient.hpp"
#include "supply.hpp"
#include "emergency.hpp"
#include "ambulance.hpp"

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Load data from text files at start
    load_patients_from_file();
    load_supplies_from_file();
    load_emergencies_from_file();
    load_ambulances_from_file();

    while (true) {
        line('=');
        cout << "HOSPITAL PATIENT CARE MANAGEMENT SYSTEM\n";
        line('=');
        cout << "1) Patient Admission Clerk (Queue)\n";
        cout << "2) Medical Supply Manager (Stack)\n";
        cout << "3) Emergency Dept Officer (Priority Queue)\n";
        cout << "4) Ambulance Dispatcher (Circular Queue)\n";
        cout << "0) Exit\n> ";

        int ch;
        if (!(cin >> ch)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (ch == 0) {
            cout << "Goodbye!\n";
            break;
        }

        switch (ch) {
            case 1: menu_patients();   break;
            case 2: menu_supplies();   break;
            case 3: menu_emergency();  break;
            case 4: menu_ambulance();  break;
            default: cout << "Invalid choice.\n"; break;
        }
    }

    return 0;
}
