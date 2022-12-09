#include "pch.h"
#include "CppUnitTest.h"
#include "../CppNative/NativeClass.h"
#include "../CppNativeDll/CppDllClass.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestingCppClrCs
{
	TEST_CLASS(TestingCppClrCs)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			NativeClass sut;

			Assert::IsTrue(sut.TrueInstance());

			Assert::IsTrue(NativeClass::TrueStatic());
		}

		TEST_METHOD(TestCS)
		{
			Assert::IsTrue(NativeClass::TrueFromCS());
		}

		TEST_METHOD(TestCsFromDLL)
		{
			Assert::IsTrue(CppDllClass().TrueFromCLR());
		}

		TEST_METHOD(TestNativeDLL)
		{
			Assert::IsTrue(CppDllClass().TrueMyself());
		}

		TEST_METHOD(TestCppClr)
		{
			Assert::IsTrue(NativeClass::TrueFromCppCLR());
		}
	};
}
