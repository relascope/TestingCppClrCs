#include <gtest/gtest.h>
#include "isolator.h"

#include "globals.h"

namespace IsolatorTest
{
	TEST(Isolator, fakenoargs)
	{
		FAKE_GLOBAL(MyGlobalTrueNoArgs);
 		WHEN_CALLED(MyGlobalTrueNoArgs()).Return(false);

		EXPECT_FALSE(MyGlobalTrueNoArgs());

		ISOLATOR_CLEANUP();
	}

	TEST(Isolator, googletest)
	{
		FAKE_GLOBAL(::MyGlobalTrue);
		WHEN_CALLED(::MyGlobalTrue((const char*)(_))).Return(false);

		// function does not take 0 arguements
		//WHEN_CALLED(::MyGlobalTrue(ANY_VAL(const char*))).Return(false); 

		// cannot convert argument 1 from int to const char *
		//WHEN_CALLED(::MyGlobalTrue(_)).Return(false);


		EXPECT_FALSE(::MyGlobalTrue("fake"));

		ISOLATOR_CLEANUP();
	}
}