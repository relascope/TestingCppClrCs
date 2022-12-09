#include "../CppNativeDll/CppDllClass.h"

#include <gtest/gtest.h>
#include "isolator.h"
TEST(CLR, CS)
{
  EXPECT_TRUE(CppDllClass().TrueFromCS());
}


TEST(CLR, FakeStatic)
{
  WHEN_CALLED(CppDllClass::TrueStatic()).Return(false);

  EXPECT_FALSE(CppDllClass::TrueStatic());
}

TEST(CLRCS, isolator)
{
  CppDllClass cl;

  WHEN_CALLED(cl.TrueFromCS()).Return(false);

  EXPECT_FALSE(cl.TrueFromCS());

  ISOLATOR_CLEANUP();
}