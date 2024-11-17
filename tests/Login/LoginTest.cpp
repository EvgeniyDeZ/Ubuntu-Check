#include "Log.cpp"
#include <gtest/gtest.h>

using namespace std;

// Test updateConnectionStatus function
TEST(LogTests, UpdateConnectionStatusTest) {
    string auth = "qUeuMObjXfv9I4k0RudhmJrSeow2OO2D";
    
    // Call updateConnectionStatus function to ensure it executes without error
    ASSERT_NO_THROW(updateConnectionStatus(true, auth));
    ASSERT_NO_THROW(updateConnectionStatus(false, auth));
}

// Test sendDataToVirtualPin function
TEST(LogTests, SendDataToVirtualPinTest) {
    string auth = "qUeuMObjXfv9I4k0RudhmJrSeow2OO2D";
    int value = 12345;
    
    // Call sendDataToVirtualPin function to ensure it executes without error
    ASSERT_NO_THROW(sendDataToVirtualPin(auth, value));
}

// Test checkBlynkLogin function for valid and invalid tokens
TEST(LogTests, CheckBlynkLoginTest) {
    string validAuth = "qUeuMObjXfv9I4k0RudhmJrSeow2OO2D";
    string invalidAuth = "invalidtoken";

    // Use function checkBlynkLogin here to return a response
    bool validLogin = checkBlynkLogin(validAuth);
    EXPECT_TRUE(validLogin); // Ensure valid token returns true

    // Test a second call to ensure state consistency
    validLogin = checkBlynkLogin(validAuth);
    EXPECT_TRUE(validLogin) << "Rechecking with valid token should still return true";

    bool invalidLogin = checkBlynkLogin(invalidAuth);
    EXPECT_FALSE(invalidLogin); // Ensure invalid token returns false
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}