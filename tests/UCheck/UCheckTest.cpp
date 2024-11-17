#include "UC.cpp"
#include <gtest/gtest.h>

using namespace std;

// Test readAuthToken function
TEST(UbuntuCheckTests, ReadAuthTokenTest) {
    ofstream file("login_status.txt");
    file << "logged_in\nqUeuMObjXfv9I4k0RudhmJrSeow2OO2D";
    file.close();
    
    string authToken = readAuthToken();
    EXPECT_EQ(authToken, "qUeuMObjXfv9I4k0RudhmJrSeow2OO2D");
}

// Test readProcessIDs function for correct number of IDs
TEST(UbuntuCheckTests, ReadProcessIDsTest) {
    ofstream file("processes_ids.txt");
    file << "123\n456\n789\n1011\n1213";
    file.close();

    vector<string> ids = readProcessIDs();
    EXPECT_EQ(ids.size(), 5);
    EXPECT_EQ(ids[0], "123");
    EXPECT_EQ(ids[4], "1213");
}

// Test writing data to Blynk
TEST(UbuntuCheckTests, WriteToBlynkTest) {
    string auth = "qUeuMObjXfv9I4k0RudhmJrSeow2OO2D";
    string pin = "V2";
    string value = "12345";
    
    // Call writeToBlynk function to ensure it executes without error
    ASSERT_NO_THROW(writeToBlynk(auth, pin, value));
}

// Test reading data from Blynk
TEST(UbuntuCheckTests, ReadFromBlynkTest) {
    string auth = "qUeuMObjXfv9I4k0RudhmJrSeow2OO2D";
    string pin = "V2";
    
    // Use function readFromBlynk here to return a response
    string response = readFromBlynk(auth, pin);
    EXPECT_NE(response, ""); // Ensure response is non-empty for successful CURL request
}

// Test slider value read from Blynk
TEST(UbuntuCheckTests, ReadSliderValueTest) {
    string auth = "qUeuMObjXfv9I4k0RudhmJrSeow2OO2D";
    string pin = "V4";

    int sliderValue = readSliderValue(auth, pin);
    EXPECT_GE(sliderValue, 0) << "Slider value should be 0 or higher if the function works correctly";
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}