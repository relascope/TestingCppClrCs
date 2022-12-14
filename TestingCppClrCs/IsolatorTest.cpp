#include "pch.h"
#include "isolator.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


bool MyGlobalTrue(const char* par)
{
	return true;
}


bool MyGlobalTrueNoArgs()
{
	return true;
}

namespace IsolatorTest
{
	TEST_CLASS(TestingCppClrCs)
	{
	public:

		TEST_METHOD(FakeGlobalWithParameter)
		{
			FAKE_GLOBAL(MyGlobalTrue);
			WHEN_CALLED(MyGlobalTrue(_)).Return(false);

			Assert::IsFalse(MyGlobalTrue("fake"));

			ISOLATOR_CLEANUP();
		}

		TEST_METHOD(FakeNoArg)
		{
			FAKE_GLOBAL(MyGlobalTrueNoArgs);
			WHEN_CALLED(MyGlobalTrueNoArgs()).Return(false);

			Assert::IsFalse(MyGlobalTrueNoArgs());

			ISOLATOR_CLEANUP();
		}
	};
}