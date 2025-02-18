#include "../include/constants.h"

// Function to read top process IDs from file
vector<string> readProcessIDs() {
    ifstream file("processes_ids.txt");
    vector<string> ids;
    string id;
    while (getline(file, id) && ids.size() < MAX_PROCESS_IDS) {
        ids.push_back(id);
    }
    file.close();
    return ids;
}
