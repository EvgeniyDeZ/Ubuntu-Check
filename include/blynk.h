#ifndef BLYNK_H
#define BLYNK_H

#include <string>
#include <curl/curl.h>

using namespace std;

// Callback function for server response
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

// Functions and constants that don't change
namespace blynk {
    // URL Blynk Cloud
    const string BLYNK_BASE_URL = "https://blynk.cloud/external/api/";

    // CURL Default Initialization
    inline CURL* initializeCurl() {
        return curl_easy_init();
    }

    // CURL Global Initialization
    inline void initializeCurlGlobal() {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    // Function to get CURL error message
    inline string getCurlErrorMessage(CURLcode res) {
        return string("CURL error: ") + curl_easy_strerror(res);
    }

    // Function to set CURL URL
    inline void setCurlUrl(CURL* curl, const string& url) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    }

    // CURL Default Settings
    inline void configureCurlBasic(CURL* curl, const std::string& url) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    }

    inline void configureCurlWrite(CURL* curl, std::string& response) {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    }

    // Perform CURL operation
    inline CURLcode performCurl(CURL* curl) {
        return curl_easy_perform(curl);
    }

    // Cleanup CURL resources
using namespace std;
    inline void cleanupCurl(CURL* curl) {
        curl_easy_cleanup(curl);
    }
}

#endif