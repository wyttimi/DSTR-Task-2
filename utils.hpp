#ifndef UTILS_HPP
#define UTILS_HPP

// ---------------------------------------------------------------------------
// utils.hpp
// ---------------------------------------------------------------------------
// This header contains common includes, constants, and helper functions
// that are shared by all roles in the Hospital Patient Care Management
// System.
//
// It provides:
//   - Global configuration constants for array sizes
//   - A simple line() function for formatting console output
//   - A safe_getline() function to read C-style strings from input safely
// ---------------------------------------------------------------------------

#include <iostream>   // for cin, cout
#include <iomanip>    // for setw formatting
#include <cstring>    // for strlen, strcpy, strncpy, etc.
#include <limits>     // for numeric_limits (used in input validation)
#include <fstream>    // for ifstream/ofstream file I/O
#include <string>     // for std::string (mainly in loadFromFile functions)

using namespace std;

// ---------------------------------------------------------------------------
// Configuration (array sizes)
// These constants define the maximum number of elements for each role's
// data structure. They are used to size fixed arrays in other headers.
//
//   MAX_PATIENTS   -> maximum number of patients in the patient queue
//   MAX_SUPPLIES   -> maximum number of supply records in the supply stack
//   MAX_EMERG      -> maximum number of emergency records in the heap
//   MAX_AMBULANCES -> maximum number of ambulances in the circular queue
// ---------------------------------------------------------------------------
const int MAX_PATIENTS   = 100;   // Role 1 (Queue)
const int MAX_SUPPLIES   = 100;   // Role 2 (Stack)
const int MAX_EMERG      = 100;   // Role 3 (Priority Queue)
const int MAX_AMBULANCES = 20;    // Role 4 (Circular Queue)

// ---------------------------------------------------------------------------
// line()
// ---------------------------------------------------------------------------
// Purpose : Print a horizontal line of a given character for formatting.
// Params  : ch - the character to print (default: '-')
//           n  - how many times to repeat the character (default: 60)
// Usage   : line('=');   --> prints "====...====\n"
// ---------------------------------------------------------------------------
inline void line(char ch = '-', int n = 60) {
    for (int i = 0; i < n; ++i) cout << ch;
    cout << "\n";
}

// ---------------------------------------------------------------------------
// safe_getline()
// ---------------------------------------------------------------------------
// Purpose : Safely read a line of text into a C-style char array. This is
//           used instead of std::getline(string) because the rest of the
//           code stores names/IDs in fixed-size char arrays.
//
// Params  : buf - pointer to the char array where the input is stored
//           cap - capacity (maximum size) of the buffer
//
// Behavior:
//   - Clears any previous input error state.
//   - Reads at most (cap - 1) characters, leaving room for '\0'.
//   - If input fails, it clears the stream and discards the remaining line.
//   - Removes trailing '\n' or '\r' characters if present.
// ---------------------------------------------------------------------------
inline void safe_getline(char* buf, int cap) {
    buf[0] = '\0';
    cin.getline(buf, cap);

    if (cin.fail()) {
        // If getline fails (e.g. input too long), reset the stream
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        buf[0] = '\0';
    }

    int len = strlen(buf);
    // Strip newline or carriage return characters at the end
    while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) {
        buf[len - 1] = '\0';
        --len;
    }
}

#endif
