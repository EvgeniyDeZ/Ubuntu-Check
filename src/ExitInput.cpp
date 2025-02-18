#include "../include/constants.h"

// Thread for reading command
void inputThread(atomic<bool>& running) {
    string input;
    while (running) {
        cin >> input;
        if (input == "Exit") {
            running = false;
            break;
        } 
    }
}
