#include "../include/constants.h"
#include "../include/blynk.h"

using namespace blynk;

// Function to read data from Blynk
string readFromBlynk(const string& auth, const string& pin) {
    CURL* curl = initializeCurl();
    if (!curl) {
        cerr << "Failed to initialize CURL." << endl;
        return "";
    }

    string url = BLYNK_BASE_URL + "get?token=" + auth + "&pin=" + pin;
    string response;
    setCurlUrl(curl, url);
    configureCurlWrite(curl, response);

    CURLcode res = performCurl(curl);
    cleanupCurl(curl);

    return (res == CURLE_OK) ? response : "";
}

// Function to write data to Blynk
void writeToBlynk(const string& auth, const string& pin, const string& value) {
    CURL* curl = initializeCurl();
    if (!curl) {
        cerr << "Failed to initialize CURL." << endl;
        return;
    }

    string url = BLYNK_BASE_URL + "update?token=" + auth + "&" + pin + "=" + value;
    setCurlUrl(curl, url);
    performCurl(curl);
    cleanupCurl(curl);
}

// Function to read slider value from Blynk
int readSliderValue(const string& auth, const string& pin) {
    string response = readFromBlynk(auth, pin);
    try {
        return stoi(response);
    } catch (...) {
        return -1;
    }
}