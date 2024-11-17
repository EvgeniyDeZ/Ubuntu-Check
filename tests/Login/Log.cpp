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

// Function to update the connection status on a virtual pin (V0)
void updateConnectionStatus(bool connected, const string& auth) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize CURL." << endl;
        return;
    }

    string url = "https://blynk.cloud/external/api/update?token=" + auth + "&pin=V0&value=" + (connected ? "1" : "0");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        cerr << "Failed to update connection status: " << curl_easy_strerror(res) << endl;
    } else {
        cout << "Connection status: " << (connected ? "connected" : "can't connect") << " to virtual pin V0." << endl;
    }

    curl_easy_cleanup(curl);
}

// Function to send data to virtual pin V1
void sendDataToVirtualPin(const string& auth, int value) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize CURL." << endl;
        return;
    }

    string url = "https://blynk.cloud/external/api/update?token=" + auth + "&pin=V1&value=" + to_string(value);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        cerr << "Data sending error: " << curl_easy_strerror(res) << endl;
    } else {
        cout << "Data sent to virtual pin V1 successfully." << endl;
    }

    curl_easy_cleanup(curl);
}

// Function to check connection and update the status
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
        // Check if there is "Invalid token" in response
        loginSuccessful = (response.find("Invalid token") == string::npos); 
    } else {
        cerr << "Connection error: " << curl_easy_strerror(res) << endl;
    }

    curl_easy_cleanup(curl);

    updateConnectionStatus(loginSuccessful, auth);
    return loginSuccessful;
}