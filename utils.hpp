#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <iomanip>
#include <cstring>
#include <limits>
#include <fstream>
#include <string>

using namespace std;

// Configuration (array sizes)
const int MAX_PATIENTS   = 100;   // Role 1 (Queue)
const int MAX_SUPPLIES   = 100;   // Role 2 (Stack)
const int MAX_EMERG      = 100;   // Role 3 (Priority Queue)
const int MAX_AMBULANCES = 20;    // Role 4 (Circular Queue)

// Draw a line
inline void line(char ch = '-', int n = 60) {
    for (int i = 0; i < n; ++i) cout << ch;
    cout << "\n";
}

// Safe getline for char arrays
inline void safe_getline(char* buf, int cap) {
    buf[0] = '\0';
    cin.getline(buf, cap);

    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        buf[0] = '\0';
    }

    int len = strlen(buf);
    while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) {
        buf[len - 1] = '\0';
        --len;
    }
}

#endif
