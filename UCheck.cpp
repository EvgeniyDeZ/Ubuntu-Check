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
#include <vector>

using namespace std;

// Callback function for server response
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
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

// Function to read data from Blynk
string readFromBlynk(const string& auth, const string& pin) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize CURL." << endl;
        return "";
    }

    string url = "https://blynk.cloud/external/api/get?token=" + auth + "&pin=" + pin;
    string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        cerr << "Failed to read data from Blynk: " << curl_easy_strerror(res) << endl;
        return "";
    }
    return response;
}

// Function to read top process IDs from file
vector<string> readProcessIDs() {
    ifstream file("processes_ids.txt");
    vector<string> ids;
    string id;
    while (getline(file, id) && ids.size() < 5) {
        ids.push_back(id);
    }
    return ids;
}

// Function to write data to Blynk
void writeToBlynk(const string& auth, const string& pin, const string& value) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize CURL." << endl;
        return;
    }

    string url = "https://blynk.cloud/external/api/update?token=" + auth + "&" + pin + "=" + value;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
}

// Run Memory Scanner
void runMemoryCheck() {
    system("./Scanner");
}

// Run Login program
void runLoginProgram() {
    system("./Login");
}

// Function to read slider value from Blynk
int readSliderValue(const string& auth, const string& pin) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize CURL." << endl;
        return -1;
    }

    string url = "https://blynk.cloud/external/api/get?token=" + auth + "&pin=" + pin;
    string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    int sliderValue = -1;
    if (res == CURLE_OK) {
        try {
            sliderValue = stoi(response);  // Convert response to integer
        } catch (const exception& e) {
            cerr << "Error converting slider response to integer: " << e.what() << endl;
        }
    } else {
        cerr << "Failed to read slider value: " << curl_easy_strerror(res) << endl;
    }

    curl_easy_cleanup(curl);
    return sliderValue;
}

int main() {
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    cout << "                          Welcome to Ubuntu_Check" << endl;
    cout << "                      To close the program type 'Exit'" << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

    // Check if logged in
    string auth = readAuthToken();
    if (auth.empty()) {
        cout << "Blynk not logged in. Running Login program...\n" << endl;
        runLoginProgram();
        auth = readAuthToken();
        if (auth.empty()) {
            cerr << "Failed to log in. Exiting." << endl;
            return 1;
        }
    }

    cout << "Blynk connection established. Monitoring memory and listening for commands..." << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

    atomic<bool> running{true};

    // Thread to monitor memory usage
    thread memoryCheckThread([&]() {
        while (running) {
            runMemoryCheck();

            for (int i = 0; i < 60 && running; ++i) {
                this_thread::sleep_for(chrono::seconds(1));
            }
        }
    });

    // Thread to display top 5 process IDs on V2
    thread displayIDThread([&]() {
        vector<string> processIDs = readProcessIDs();
        int index = 0;

        while (running) {
            if (processIDs.empty()) {
                processIDs = readProcessIDs();
            }

            if (!processIDs.empty()) {
                writeToBlynk(auth, "V2", processIDs[index]);

                processIDs = readProcessIDs();

                // Cycle to the next ID
                index = (index + 1) % processIDs.size();
            }
            this_thread::sleep_for(chrono::seconds(5));
        }
    });

    // Thread to check V3 for termination command and close the displayed process
    thread terminateAppThread([&]() {
        while (running) {
            string toggleValue = readFromBlynk(auth, "V3");
            if (toggleValue == "1") {
                vector<string> processIDs = readProcessIDs();
                int index = 0;
                string currentPID = readFromBlynk(auth, "V2");

                if (!currentPID.empty()) {
                    string command = "kill " + currentPID;
                    system(command.c_str());

                    // Reset the toggle on Blynk to avoid repeated kills
                    writeToBlynk(auth, "V3", "0");

                    // Refresh the process list after killing one app
                    processIDs = readProcessIDs();
                }
            }
            std::this_thread::sleep_for(chrono::seconds(1));
        }
    });

    // Thread to check for shutdown and reboot commands
    thread controlThread([&]() {
        while (running) {
            int sliderValue = readSliderValue(auth, "V4");

            if (sliderValue == 0) {
                cout << "Rebooting system..." << endl;
                system("reboot");
                running = false;
            } else if (sliderValue == 10) {
                cout << "Shutting down system..." << endl;
                system("shutdown now");
                running = false;
            }

            // Check every 5 seconds
            this_thread::sleep_for(chrono::seconds(5));
        }
    });

    // Thread for handling user input to exit the program
    thread inputThread([&]() {
        string input;
        while (running) {
            cin >> input;
            if (input == "Exit") {
                running = false;
            }
        }
    });

    // Wait for threads to finish
    memoryCheckThread.join();
    displayIDThread.join();
    terminateAppThread.join();
    controlThread.join();
    inputThread.join();

    return 0;
}
