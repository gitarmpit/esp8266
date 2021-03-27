#include "../common/memmem.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

TEST(MemMemTests, MemMem1)
{
    const char* mem1 = "-----23";
    const char* mem2 = "23";
    int rc = MemMem(mem1, mem2, (int)strlen(mem1), (int)strlen(mem2));
    EXPECT_EQ(rc, 5);
}

TEST(MemMemTests, MemMem2)
{
    const char* mem1 = "23";
    const char* mem2 = "-----23";
    int rc = MemMem(mem1, mem2, (int)strlen(mem1), (int)strlen(mem2));
    EXPECT_EQ(rc, -1);
}
TEST(MemMemTests, MemMem3)
{
    const char* mem1 = "23";
    const char* mem2 = "23";
    int rc = MemMem(mem1, mem2, (int)strlen(mem1), (int)strlen(mem2));
    EXPECT_EQ(rc, 0);
}

TEST(MemMemTests, MemMem4)
{
    const char* mem1 = "23";
    const char* mem2 = "234";
    int rc = MemMem(mem1, mem2, (int)strlen(mem1), (int)strlen(mem2));
    EXPECT_EQ(rc, -1);
}
