#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../common/PersistedSettings.h"
#include <windows.h>

TEST(PersistedSettings_Tests, test1)
{
    std::string ssid = "ssid";
    std::string pass = "pass";
    PersistedSettings ps;
    ps.SetApSettings(ssid.c_str(), pass.c_str());
    EXPECT_STREQ(ssid.c_str(), ps.GetSsid());
    EXPECT_STREQ(pass.c_str(), ps.GetPassword());
}

TEST(PersistedSettings_Tests, test2)
{
    PersistedSettings ps;
    ps.SetApSettings(NULL, NULL);
    EXPECT_EQ(NULL, ps.GetSsid());
    EXPECT_EQ(NULL, ps.GetPassword());
}