#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <cstdio>
#include <memory>
#include <array>
#include <atomic>

using namespace std;

// Function to run the second program and capture its output
string runMemoryCheck() {
    array<char, 128> buffer;
    string result;
    // Open the process using popen to read output
    unique_ptr<FILE, int(*)(FILE*)> pipe(popen("./Scanner", "r"), pclose);
    if (!pipe) {
        cerr << "Failed to open pipe." << endl;
        return "Error running memory check.";
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int main() {
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    cout << "                  Welcome to Ubuntu_Check" << endl;
    cout << "              To close the program type 'Exit'" << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

    // Delay before first launch
    this_thread::sleep_for(chrono::seconds(2));
    
    // To control the execution of threads
    atomic<bool> running{true}; 

    // Start the memory check loop
    thread memoryCheckThread([&]() {
        while (running) {
            string memoryInfo = runMemoryCheck();
            cout << memoryInfo;

            // Check every second if running is still true
            for (int i = 0; i < 60 && running; ++i) {
                this_thread::sleep_for(chrono::seconds(1));
            }
        }
    });

    // Start a thread for user input
    thread inputThread([&]() {
        string input;
        while (running) {
            cin >> input;
            if (input == "Exit") {
                running = false; // Signal to stop both threads
            }
        }
    });

    // Wait for the memory check thread to finish
    memoryCheckThread.join();
    
    // Notify user input thread to finish
    inputThread.join();

    return 0;
}

