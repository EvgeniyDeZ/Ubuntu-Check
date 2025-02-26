#include "../include/constants.h"

// Get a process memory usage in KiB
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

void scannerThread(atomic<bool>& running, const string& auth, thread& readingProcess) {
    while (running) {
        ifstream meminfo("/proc/meminfo");
        string line;
        long totalMemory = 0, availableMemory = 0;

        if (meminfo.is_open()) {
            while (getline(meminfo, line)) {
                if (line.find("MemTotal:") == 0) {
                    totalMemory = stol(line.substr(line.find_first_of("0123456789"))); // Загальна пам'ять
                }
                if (line.find("MemAvailable:") == 0) {
                    availableMemory = stol(line.substr(line.find_first_of("0123456789"))); // Доступна пам'ять
                }
            }
            meminfo.close();
        } else {
            cerr << "Unable to open /proc/meminfo." << endl;
            return;
        }

        long usedMemory = totalMemory - availableMemory;
        double totalGB = static_cast<double>(totalMemory) * 1024 / (1000 * 1000 * 1000); // GB
        double usedGB = static_cast<double>(usedMemory) * 1024 / (1000 * 1000 * 1000);  // GB
        double percentageUsed = (usedGB / totalGB) * 100;

        cout << fixed << setprecision(1);
        cout << "Total Memory: " << totalGB << " GB" << endl;
        cout << "Used Memory: " << usedGB << " GB" << endl;
        cout << "Memory Usage Percentage: " << percentageUsed << "%" << endl;
        cout << " " << endl;

        long thresholdMemory = totalMemory / DIVISOR;
        vector<ProcessInfo> processes;

        DIR* procDir = opendir("/proc");
        if (procDir) {
            struct dirent* entry;
            while ((entry = readdir(procDir)) != nullptr) {
                if (entry->d_type == DT_DIR && isdigit(entry->d_name[0])) {
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

        sort(processes.begin(), processes.end(), [](const ProcessInfo& a, const ProcessInfo& b) {
            return a.memoryUsage > b.memoryUsage;
        });

        bool found = false;
        for (const auto& proc : processes) {
            if (proc.memoryUsage > thresholdMemory) {
                cout << "Program using more than " << PERCENT << "% of RAM:" << endl;
                cout << "PID: " << proc.pid << endl;
                cout << "Command: " << proc.command << endl;
                cout << "Memory Usage: " << proc.memoryUsage / 1024.0 << " MB" << endl;
                found = true;
                break;
            }
        }

        if (!found) {
            cout << "No program is using more than " << PERCENT << "% of RAM." << endl;
        }

        cout << "\nTop 5 memory-consuming programs:" << endl;
        ofstream outFile("output/processes_ids.txt");
        if (outFile.is_open()) {
            for (int i = 0; i < min(5, static_cast<int>(processes.size())); ++i) {
                cout << "PID: " << processes[i].pid << " | Command: " << processes[i].command
                     << " | Memory Usage: " << processes[i].memoryUsage / 1024.0 << " MB" << endl;
                outFile << processes[i].pid << endl; 
            }
            outFile.close();
        } else {
            cerr << "Unable to open file for writing." << endl;
        }

        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

        for (int i = 0; i < SCANNER_TIME && running; ++i) { 
            this_thread::sleep_for(chrono::seconds(1));
        }  

        if (running) {
            running = false;
            cv.notify_all();

            if (readingProcess.joinable()) {
                readingProcess.join();
            }

            running = true;  

            remove("output/processes_ids.txt");

            readingProcess = thread(readingThread, ref(running), ref(auth));
        }     
    }
}