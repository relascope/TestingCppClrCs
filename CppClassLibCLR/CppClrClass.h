#pragma once

//__declspec(dllexport) bool __cdecl myfuncex(void) {
//	return true;
//};

#ifdef CPPCLREXPORT
#define MYDLL __declspec(dllexport)
#else
#define MYDLL __declspec(dllimport)
#endif

	class MYDLL CppClrClass
	{
	public:
		static bool TrueCS();
		// TODO: Add your methods for this class here.

		bool TrueMyself();

		static void SetUpAssemblyResolve();
	};
 