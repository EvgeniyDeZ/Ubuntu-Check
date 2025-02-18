#include "../include/constants.h"
#include "../src/BlynkAPI.cpp"

void loginThread(atomic<bool>& running, string& auth) {
    bool loginSuccessful = false;

    ifstream loginFile("output/login_status.txt");
    if (loginFile.is_open()) {
        string status;
        getline(loginFile, status);
        if (status == "logged_in") {
            getline(loginFile, auth);
            loginSuccessful = checkBlynkLogin(auth);
        }
        loginFile.close();
    }

    while (!loginSuccessful && running && ATTEMPTS < MAX_ATTEMPTS) {
        cout << "Enter your Blynk auth token: ";
        cin >> auth;
        cout << "\n";

        loginSuccessful = checkBlynkLogin(auth);
        if (loginSuccessful) {
            break;
        } else {
            cerr << "Invalid token or failed to connect. Please try again.\n" << endl;
            ATTEMPTS++;
        }
    }

    if (!loginSuccessful) {
        cerr << "Maximum login attempts reached. Exiting..." << endl;
        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
        running = false;
        return;
    }

    ofstream loginStatus("output/login_status.txt");
    if (loginStatus.is_open()) {
        loginStatus << "logged_in\n" << auth;
        loginStatus.close();
    }

    sendDataToVirtualPin(auth, 1);
    cout << "Blynk login successful.\n" << endl;


    cout << "Blynk connection established. Monitoring memory and listening for commands..." << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
}