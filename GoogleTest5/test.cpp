#include "pch.h"
#include "../CppNativeDll/CppDllClass.h"

TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

TEST(CppNative, IntoCS)
{
  EXPECT_TRUE(CppDllClass().TrueFromCS());
}