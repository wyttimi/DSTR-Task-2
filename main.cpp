// main.cpp
// ---------------------------------------------------------------------------
// CT077-3-2-DSTR Lab Evaluation Work #2
// Hospital Patient Care Management System
//
// This file contains the main() function and the TOP-LEVEL MENU of the system.
// It calls the menus for each role:
//
//   Role 1: Patient Admission Clerk      -> uses Queue      (patient.hpp)
//   Role 2: Medical Supply Manager       -> uses Stack      (supply.hpp)
//   Role 3: Emergency Dept Officer       -> uses PriorityQ  (emergency.hpp)
//   Role 4: Ambulance Dispatcher         -> uses CircQueue  (ambulance.hpp)
//
// Data for each role is loaded from text files when the program starts, and
// the role modules are responsible for saving updated data back to the files.
// ---------------------------------------------------------------------------

#include "patient.hpp"    // Patient queue functions + load_patients_from_file()
#include "supply.hpp"     // Supply stack functions + load_supplies_from_file()
#include "emergency.hpp"  // Emergency priority queue + load_emergencies_from_file()
#include "ambulance.hpp"  // Ambulance circular queue + load_ambulances_from_file()

int main() {
    // Improve I/O performance (optional, but common in C++)
    ios::sync_with_stdio(false);
    cin.tie(nullptr);   // Disable automatic flush before reading from cin

    // -----------------------------------------------------------------------
    // STEP 1: Load existing data from text files (if the files exist).
    // Each role has its own text file and its own load_..._from_file() function:
    //
    //   patients.txt     -> patient queue
    //   supplies.txt     -> supply stack
    //   emergencies.txt  -> emergency priority queue
    //   ambulances.txt   -> ambulance circular queue
    //
    // If the files do not exist, the roles will start with empty structures.
    // -----------------------------------------------------------------------
    load_patients_from_file();
    load_supplies_from_file();
    load_emergencies_from_file();
    load_ambulances_from_file();

    // -----------------------------------------------------------------------
    // STEP 2: Main loop for the whole system.
    // This loop displays the main menu and lets the user choose which role
    // they want to use. It continues until the user selects "0) Exit".
    // -----------------------------------------------------------------------
    while (true) {
        line('=');   // Print a line of '=' characters for nicer formatting
        cout << "HOSPITAL PATIENT CARE MANAGEMENT SYSTEM\n";
        line('=');
        cout << "1) Patient Admission Clerk (Queue)\n";
        cout << "2) Medical Supply Manager (Stack)\n";
        cout << "3) Emergency Dept Officer (Priority Queue)\n";
        cout << "4) Ambulance Dispatcher (Circular Queue)\n";
        cout << "0) Exit\n> ";

        int ch;

        // -------------------------------------------------------------------
        // Input validation for the main menu choice:
        // If the user types something that is not an integer, we clear the
        // error flags and ignore the bad input, then ask again.
        // -------------------------------------------------------------------
        if (!(cin >> ch)) {
            cin.clear();   // clear error state
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard line
            continue;      // back to the top of the while loop
        }
        // Remove any extra characters left in the input buffer (e.g. newline)
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // -------------------------------------------------------------------
        // STEP 3: Check user choice.
        // If 0 -> exit program.
        // Otherwise, call the corresponding role menu.
        // -------------------------------------------------------------------
        if (ch == 0) {
            cout << "Goodbye!\n";
            break;   // exit the while loop and end the program
        }

        switch (ch) {
            case 1:
                // Role 1: Patient Admission Clerk (Queue)
                // Handles admitting, discharging, and viewing patient queue.
                menu_patients();
                break;

            case 2:
                // Role 2: Medical Supply Manager (Stack)
                // Handles adding supplies and using supplies by type
                // (using the most recently added batch for that type).
                menu_supplies();
                break;

            case 3:
                // Role 3: Emergency Department Officer (Priority Queue)
                // Handles logging emergency cases and processing the most
                // critical case first based on priority level.
                menu_emergency();
                break;

            case 4:
                // Role 4: Ambulance Dispatcher (Circular Queue)
                // Handles registering ambulances, rotating the shift, and
                // displaying the current rotation order.
                menu_ambulance();
                break;

            default:
                // Any other number is invalid
                cout << "Invalid choice.\n";
                break;
        }
    }

    // Program ends here. All data saving is handled by each role's functions.
    return 0;
}
