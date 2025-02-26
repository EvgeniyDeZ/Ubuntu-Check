#include "../include/constants.h"
#include "../src/BlynkHandler.cpp"

// Function to read top process IDs from file
vector<string> readProcessIDs() {
    ifstream file("output/processes_ids.txt");
    vector<string> ids;
    string id;
    while (getline(file, id) && ids.size() < MAX_PROCESS_IDS) {
        ids.push_back(id);
    }
    file.close();
    return ids;
}

void readingThread(atomic<bool>& running, const string& auth) {
    ofstream clearFile("output/processes_ids.txt", ios::trunc);
    if (!clearFile.is_open()) {
        cerr << "Unable to clear processes_ids.txt." << endl;
    }
    clearFile.close();
    
    while (running) {
        vector<string> processIDs = readProcessIDs();

        for (const auto& pid : processIDs) {
            if (!running) return;
            writeToBlynk(auth, "V2", pid);

            unique_lock<mutex> lk(cv_m);
            cv.wait_for(lk, chrono::seconds(UPDATE_TIME), [&] { return !running; });
        }        
    }
}


void commandThread(atomic<bool>& running, const string& auth, thread& readingProcess, thread& scannerProcess) {
    writeToBlynk(auth, "V3", "0");
    writeToBlynk(auth, "V4", to_string(DEFAULT_SLIDER));

    while (running) {
        string terminateCommand = readFromBlynk(auth, "V3");
        string currentPID = readFromBlynk(auth, "V2");

        if (terminateCommand == "1") {
            if (!currentPID.empty()) {
                system(("kill " + currentPID).c_str());
                writeToBlynk(auth, "V3", "0");

                // Stop reading thread before restarting
                running = false;
                // Wake readingProcess and scannerProcess immediately
                cv.notify_all();  
                
                if (scannerProcess.joinable()) {
                    scannerProcess.join();
                }
                if (readingProcess.joinable()) {
                    readingProcess.join();
                }
                
                running = true;
                
                // Delete the output file
                remove("output/processes_ids.txt");
                
                // Restart scanner and reading threads after termination
                scannerProcess = thread(scannerThread, ref(running), ref(auth), ref(readingProcess));  
                readingProcess = thread(readingThread, ref(running), ref(auth));
            }
        }

        int sliderValue = readSliderValue(auth, "V4");
        if (sliderValue == SLIDER_REBOOT) {
            cout << "Rebooting system..." << endl;
            system("reboot");
            running = false;
        } else if (sliderValue == SLIDER_SHUTDOWN) {
            cout << "Shutting down system..." << endl;
            system("shutdown now");
            running = false;
        }
    }
}


// Thread for exit program
void inputThread(atomic<bool>& running) {
    string input;
    while (running) {
        cin >> input;
        if (input == "Exit") {
            running = false;
            cv.notify_all();  // Wake up all sleeping threads
            break;
        } 
    }
}