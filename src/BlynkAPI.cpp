#include "../include/constants.h"
#include "../include/blynk.h"

using namespace blynk;

// Function to update the connection status on a virtual pin (V0)
void updateConnectionStatus(bool connected, const string& auth) {
    CURL* curl = initializeCurl();
    if (!curl) {
        cerr << "Failed to initialize CURL." << endl;
        return;
    }

    string url = BLYNK_BASE_URL + "update?token=" + auth + "&pin=V0&value=" + (connected ? "1" : "0");
    configureCurlBasic(curl, url);

    CURLcode res = performCurl(curl);
    if (res != CURLE_OK) {
        cerr << "Failed to update connection status: " << curl_easy_strerror(res) << endl;
    } else {
        cout << "Connection status: " << (connected ? "connected" : "can't connect") << " to virtual pin V0." << endl;
    }

    cleanupCurl(curl);
}

// Function to send data to virtual pin V1
void sendDataToVirtualPin(const string& auth, int value) {
    CURL* curl = initializeCurl();
    if (!curl) {
        cerr << "Failed to initialize CURL." << endl;
        return;
    }

    string url = BLYNK_BASE_URL + "update?token=" + auth + "&pin=V1&value=" + to_string(value);
    configureCurlBasic(curl, url);

    CURLcode res = performCurl(curl);
    if (res != CURLE_OK) {
        cerr << "Data sending error: " << curl_easy_strerror(res) << endl;
    } else {
        cout << "Data sent to virtual pin V1 successfully." << endl;
    }

    cleanupCurl(curl);
}

// Function to check connection and update the status
bool checkBlynkLogin(const string& auth) {
    CURL* curl = initializeCurl();
    if (!curl) {
        cerr << "Failed to initialize CURL." << endl;
        return false;
    }

    bool loginSuccessful = false;
    string response;
    string url = BLYNK_BASE_URL + "get?token=" + auth + "&pin=V0";

    configureCurlBasic(curl, url);
    configureCurlWrite(curl, response);

    CURLcode res = performCurl(curl);
    if (res == CURLE_OK) {
        cout << "Response from server: " << response << endl;
        loginSuccessful = (response.find("Invalid token") == string::npos);
    } else {
        cerr << "Connection error: " << curl_easy_strerror(res) << endl;
    }

    cleanupCurl(curl);

    updateConnectionStatus(loginSuccessful, auth);
    return loginSuccessful;
}