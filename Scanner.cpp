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

// Get a procces memory usage in KiB
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

int main() {
    ifstream meminfo("/proc/meminfo");
    string line;
    long totalMemory = 0, availableMemory = 0;

    if (meminfo.is_open()) {
        while (getline(meminfo, line)) {
            if (line.find("MemTotal:") == 0) {
                totalMemory = stol(line.substr(line.find_first_of("0123456789"))); // Total memory in KiB
            }
            if (line.find("MemAvailable:") == 0) {
                availableMemory = stol(line.substr(line.find_first_of("0123456789"))); // Available memory in KiB
            }
        }
        meminfo.close();

        // Calculate used memory in KB (considering Available)
        long usedMemory = totalMemory - availableMemory;

        // Convert KB to GB
        double totalGB = static_cast<double>(totalMemory) * 1024 / (1000 * 1000 * 1000); // Decimal conversion for total memory
        double usedGB = static_cast<double>(usedMemory) * 1024 / (1000 * 1000 * 1000);  // Decimal conversion for used memory

        // Calculate the percentage of used memory
        double percentageUsed = (usedGB / totalGB) * 100;

        // Output the results
        cout << fixed << setprecision(1);
        cout << "Total Memory: " << totalGB << " GB" << endl;
        cout << "Used Memory: " << usedGB << " GB" << endl;
        cout << "Memory Usage Percentage: " << percentageUsed << "%" << endl;
        cout << " " << endl;

        // Check for programs using more than 50% of RAM
        long thresholdMemory = totalMemory / 2;
        vector<ProcessInfo> processes;

        DIR* procDir = opendir("/proc");
        if (procDir) {
            struct dirent* entry;
            while ((entry = readdir(procDir)) != nullptr) {
                if (entry->d_type == DT_DIR && isdigit(entry->d_name[0])) { // Check if the directory name is a number (PID)
                    int pid = atoi(entry->d_name);
                    long memUsage = getMemoryUsageKB(pid);
                    if (memUsage > 0) {
                        char comm[256] = "";
                        string commFile = "/proc/" + to_string(pid) + "/comm";
                        ifstream file(commFile);
                        if (file.is_open()) {
                            file.getline(comm, sizeof(comm));
                            file.close();
                        }
                        processes.push_back({pid, memUsage, comm});
                    }
                }
            }
            closedir(procDir);
        }

        // Sort processes by memory usage in descending order
        sort(processes.begin(), processes.end(), [](const ProcessInfo& a, const ProcessInfo& b) {
            return a.memoryUsage > b.memoryUsage;
        });

        // Output the program using more than 50% of RAM, if any
        bool found = false;
        for (const auto& proc : processes) {
            if (proc.memoryUsage > thresholdMemory) {
                cout << "Program using more than 50% of RAM:" << endl;
                cout << "PID: " << proc.pid << endl;
                cout << "Command: " << proc.command << endl;
                cout << "Memory Usage: " << proc.memoryUsage / 1024.0 << " MB" << endl;
                found = true;
                break;
            }
        }

        if (!found) {
            cout << "No program is using more than 50% of RAM." << endl;
        }

        // Output the top 5 memory-consuming programs
        cout << "\nTop 5 memory-consuming programs:" << endl;
        ofstream outFile("processes_ids.txt");
        if (outFile.is_open()) {
            for (int i = 0; i < min(5, static_cast<int>(processes.size())); ++i) {
                cout << "PID: " << processes[i].pid << " | Command: " << processes[i].command
                     << " | Memory Usage: " << processes[i].memoryUsage / 1024.0 << " MB" << endl;
                // Save the PID to the file
                outFile << processes[i].pid << endl; 
            }
            outFile.close();
        } else {
            cout << "Unable to open file for writing." << endl;
        }
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

    } else {
        cout << "Unable to open /proc/meminfo." << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    }

    return 0;
}
