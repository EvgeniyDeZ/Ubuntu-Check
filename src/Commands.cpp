#include "../include/constants.h"
#include "../src/BlynkHandler.cpp"
#include "../src/ProcessManager.cpp"

void readingThread(atomic<bool>& running, const string& auth) {
    while (running) {
        vector<string> processIDs = readProcessIDs();
        
        for (const auto& pid : processIDs) {
            if (!running) break;
            writeToBlynk(auth, "V2", pid);            
            this_thread::sleep_for(chrono::seconds(UPDATE_TIME));
        }
    }
}

void commandThread(atomic<bool>& running, const string& auth) {
    writeToBlynk(auth, "V3", "0");
    writeToBlynk(auth, "V4", to_string(DEFAULT_SLIDER));

    while (running) {
        string terminateCommand = readFromBlynk(auth, "V3");
        string currentPID = readFromBlynk(auth, "V2");

        if (terminateCommand == "1") {
            if (!currentPID.empty()) {
                system(("kill " + currentPID).c_str());
                writeToBlynk(auth, "V3", "0");
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