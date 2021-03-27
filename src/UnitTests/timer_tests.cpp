#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../common/Timer.h"
#include <windows.h>

TEST(Timer_Tests, test1)
{
    Timer timer;
    Sleep(300);
    int elapsed = timer.ElapsedMs();
    EXPECT_TRUE(elapsed > 290 && elapsed < 310);
    timer.Reset();
    Sleep(100);
    elapsed = timer.ElapsedMs();
    EXPECT_TRUE(elapsed > 90 && elapsed < 110);
}