#include <gtest/gtest.h>
#include "isolator.h"



bool MyGlobalTrue(const char* par)
{
	return true;
}


namespace IsolatorTest
{
	TEST(Isolator, googletest)
	{
		FAKE_GLOBAL(MyGlobalTrue);
		WHEN_CALLED(MyGlobalTrue(_)).Return(false);

		EXPECT_TRUE(MyGlobalTrue("fake"));

		ISOLATOR_CLEANUP();
	}
}