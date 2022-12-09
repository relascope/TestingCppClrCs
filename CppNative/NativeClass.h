#pragma once


#ifdef CPPNATIVE_EXPORT
#define MYDLL __declspec(dllexport)
#else
#define MYDLL __declspec(dllimport)
#endif

class MYDLL NativeClass
{
public:
	bool TrueInstance();

	static bool TrueStatic();

	static bool TrueFromCS();

	static bool TrueFromCppCLR();
};

