// UCheck
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <cstdio>
#include <memory>
#include <array>
#include <atomic>
#include <fstream>
#include <cstdlib>
#include <curl/curl.h>

using namespace std;

// Function to run the second program and capture its output
string runMemoryCheck() {
    array<char, 128> buffer;
    string result;
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

// Function to read auth token from file if logged in
string readAuthToken() {
    ifstream loginStatus("login_status.txt");
    string status, auth;
    if (loginStatus.is_open()) {
        getline(loginStatus, status);
        if (status == "logged_in") {
            getline(loginStatus, auth);
        }
        loginStatus.close();
    }
    return auth;
}

// Run Login program if not logged in
void runLoginProgram() {
    system("./Login");
}

int main() {
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    cout << "                  Welcome to Ubuntu_Check" << endl;
    cout << "              To close the program type 'Exit'" << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

    // Check if logged in
    string auth = readAuthToken();
    if (auth.empty()) {
        cout << "Blynk not logged in. Running Login program...\n" << endl;
        runLoginProgram();
        auth = readAuthToken(); // Re-read auth token after login
        if (auth.empty()) {
            cerr << "Failed to log in. Exiting." << endl;
            return 1;
        }
    }

    cout << "Blynk connection established. Monitoring memory..." << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

    // Delay before first launch
    this_thread::sleep_for(chrono::seconds(2));

    atomic<bool> running{true}; 

    thread memoryCheckThread([&]() {
        while (running) {
            string memoryInfo = runMemoryCheck();
            cout << memoryInfo;

            for (int i = 0; i < 60 && running; ++i) {
                this_thread::sleep_for(chrono::seconds(1));
            }
        }
    });

    thread inputThread([&]() {
        string input;
        while (running) {
            cin >> input;
            if (input == "Exit") {
                running = false;
            }
        }
    });

    memoryCheckThread.join();
    inputThread.join();

    return 0;
}
