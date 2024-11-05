#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>
#include <curl/curl.h>

using namespace std;

// Callback function for server response
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

// Function to update the connection status on a virtual pin (V1)
void updateConnectionStatus(bool connected, const string& auth) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize CURL." << endl;
        return;
    }

    string url = "https://blynk.cloud/external/api/update?token=" + auth + "&pin=V1&value=" + (connected ? "1" : "0");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        cerr << "Failed to update connection status: " << curl_easy_strerror(res) << endl;
    } else {
        cout << "Connection status: " << (connected ? "connected" : "can't connect") << " to virtual pin V1." << endl;
    }

    curl_easy_cleanup(curl);
}

// Function to check connection and update the status pin
bool checkBlynkLogin(const string& auth) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize CURL." << endl;
        return false;
    }

    bool loginSuccessful = false;
    string response;
    string url = "https://blynk.cloud/external/api/get?token=" + auth + "&pin=V0";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        cout << "Response from server: " << response << endl;
        loginSuccessful = (response.find("Invalid token") == string::npos); // Check for "Invalid token" in response
    } else {
        cerr << "Connection error: " << curl_easy_strerror(res) << endl;
    }

    curl_easy_cleanup(curl);

    // Update the connection status pin based on login success
    updateConnectionStatus(loginSuccessful, auth);
    return loginSuccessful;
}

// Function to send data to virtual pin V0
void sendDataToVirtualPin(const string& auth, int value) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize CURL." << endl;
        return;
    }

    string url = "https://blynk.cloud/external/api/update?token=" + auth + "&pin=V0&value=" + to_string(value);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        cerr << "Data sending error: " << curl_easy_strerror(res) << endl;
    } else {
        cout << "Data sent to virtual pin V0 successfully." << endl;
    }

    curl_easy_cleanup(curl);
}

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    string auth;
    bool loginSuccessful = false;

    // Prompt the user to enter the auth token
    do {
        cout << "Enter your Blynk auth token: ";
        cin >> auth;
        cout << "\n";

        loginSuccessful = checkBlynkLogin(auth);
        if (!loginSuccessful) {
            cerr << "Invalid token or failed to connect. Please try again.\n" << endl;
        }
    } while (!loginSuccessful);

    // Write login status and auth token to file if login is successful
    ofstream loginStatus("login_status.txt");
    if (loginStatus.is_open()) {
        loginStatus << "logged_in\n" << auth;
        loginStatus.close();
    }

    // Send data to virtual pin V0 after successful login
    sendDataToVirtualPin(auth, 1);

    cout << "Blynk login successful.\n" << endl;

    curl_global_cleanup();
    return 0;
}