#include "include/constants.h"
#include "src/Commands.cpp"
#include "src/ExitInput.cpp"
#include "src/Login.cpp"
#include "src/Scanner.cpp"

int main() {
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    cout << "                          Welcome to Ubuntu_Check" << endl;
    cout << "                      To close the program type 'Exit'" << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

    atomic<bool> running{true};
    string auth;

    // Thread for login
    thread loginProcess(loginThread, ref(running), ref(auth));

    loginProcess.join();  // Waiting for end loginning

    // Running main Threads
    thread inputProcess(inputThread, ref(running));
    thread readingProcess(readingThread, ref(running), ref(auth));
    thread commandProcess(commandThread, ref(running), cref(auth));
    thread scannerProcess(scannerThread, ref(running));    

    inputProcess.join();
    readingProcess.join();
    commandProcess.join();
    scannerProcess.join();

    return 0;
}