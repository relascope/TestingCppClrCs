#include "../CppNativeDll/CppDllClass.h"

#include <gtest/gtest.h>

TEST(CLR, CS)
{
  EXPECT_TRUE(CppDllClass().TrueFromCS());
}
