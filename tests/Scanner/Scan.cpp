#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <dirent.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <algorithm>

using namespace std;

struct ProcessInfo {
    int pid;
    long memoryUsage;
    string command;
};

long getMemoryUsageKB(int pid) {
    string statusFile = "/proc/" + to_string(pid) + "/status";
    ifstream file(statusFile);
    string line;
    long memUsage = 0;

    if (file.is_open()) {
        while (getline(file, line)) {
            if (line.find("VmRSS:") == 0) {
                memUsage = stol(line.substr(line.find_first_of("0123456789"))); // Memory usage in KiB
                break;
            }
        }
        file.close();
    }

    return memUsage;
}