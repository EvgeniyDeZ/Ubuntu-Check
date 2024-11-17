#include "Scan.cpp"
#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

// Function to check if a file exists
bool fileExists(const string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

// Test getMemoryUsageKB function
TEST(MemoryUsageTests, GetMemoryUsageKB) {
    // Use the current process for testing
    int pid = getpid();
    long memoryUsage = getMemoryUsageKB(pid);
    EXPECT_GT(memoryUsage, 0) << "Memory usage should be greater than 0 for the current process.";
}

// Test reading memory information from /proc/meminfo
TEST(MemoryUsageTests, ReadMemoryInfo) {
    ifstream meminfo("/proc/meminfo");
    EXPECT_TRUE(meminfo.is_open()) << "Failed to open /proc/meminfo.";

    string line;
    bool memTotalWrong = false, memAvailableWrong = false;

    while (getline(meminfo, line)) {
        if (line.find("MemTotal:") == 0) {
            memTotalWrong = true;
        }
        if (line.find("MemAvailable:") == 0) {
            memAvailableWrong = true;
        }
    }

    EXPECT_TRUE(memTotalWrong) << "MemTotal not found in /proc/meminfo.";
    EXPECT_TRUE(memAvailableWrong) << "MemAvailable not found in /proc/meminfo.";
}

// Test sorting processes by memory usage
TEST(ProcessSortingTests, SortProcessesByMemoryUsage) {
    vector<ProcessInfo> processes = {
        {1, 1000, "Process1"},
        {2, 3000, "Process2"},
        {3, 2000, "Process3"}
    };

    sort(processes.begin(), processes.end(), [](const ProcessInfo& a, const ProcessInfo& b) {
        return a.memoryUsage > b.memoryUsage;
    });

    EXPECT_EQ(processes[0].pid, 2);
    EXPECT_EQ(processes[1].pid, 3);
 
    EXPECT_EQ(processes[2].pid, 1);
}

// Test absence of programs using more than 50% of RAM
TEST(MemoryThresholdTests, NoProgramExceedsThreshold) {
    long totalMemory = 8000000; // Example: 8 GB in KiB
    long thresholdMemory = totalMemory / 2; // 50%

    vector<ProcessInfo> processes = {
        {1, 100000, "Process1"},
        {2, 200000, "Process2"},
        {3, 300000, "Process3"}
    };

    bool found = false;
    for (const auto& proc : processes) {
        if (proc.memoryUsage > thresholdMemory) {
            found = true;
            break;
        }
    }

    EXPECT_FALSE(found) << "No program should exceed the threshold memory.";
}

// Main function to run tests
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
