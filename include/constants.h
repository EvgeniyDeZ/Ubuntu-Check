#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <fstream>
#include <cstdlib>
#include <curl/curl.h>
#include <vector>
#include <atomic>
#include <iomanip>
#include <dirent.h> 
#include <unistd.h>
#include <algorithm>

#include <condition_variable>

using namespace std;

// Global atomic flag for thread control
extern atomic<bool> running;
atomic<bool> restartReading{false};

condition_variable cv;
mutex cv_m;

struct ProcessInfo {
    int pid;
    long memoryUsage;
    string command;
};

int ATTEMPTS = 0;
const int MAX_ATTEMPTS = 3;

const int DIVISOR = 2;
const int PERCENT = 100 / DIVISOR;

const int MAX_PROCESS_IDS = 5;
const int SLIDER_REBOOT = 0;
const int SLIDER_SHUTDOWN = 10;
const int DEFAULT_SLIDER = SLIDER_SHUTDOWN / 2;

const int SCANNER_TIME = 60;

const int UPDATE_TIME = 5;

vector<string> readProcessIDs();

void loginThread(atomic<bool>& running, string& auth);
void readingThread(atomic<bool>& running, const string& auth);

bool checkBlynkLogin(const string& auth);
void updateConnectionStatus(bool connected, const string& auth);
void sendDataToVirtualPin(const string& auth, int value);

string readFromBlynk(const string& auth, const string& pin);
void writeToBlynk(const string& auth, const string& pin, const string& value);
int readSliderValue(const string& auth, const string& pin);

void scannerThread(atomic<bool>& running, const string& auth, thread& readingProcess);

#endif