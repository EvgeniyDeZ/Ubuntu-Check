#include <iostream>
#include <thread>
#include <chrono>
#include <string>

using namespace std;

int main(){
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    cout << "     Welcome to Ubuntu_Check" << endl;
    cout << " To close the program type 'Exit'" << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

    // Delay before first launch
    this_thread::sleep_for(chrono::seconds(10));

    while (true) {  
        cout << "First Program" << endl;

        // Check for user input
        string input;
        cin >> input;
        if (input == "Exit") {
            break;
        }

        // Restart every one minute
        this_thread::sleep_for(chrono::minutes(1));  
    }

    return 0;
}