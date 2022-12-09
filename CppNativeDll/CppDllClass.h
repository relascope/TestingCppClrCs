#pragma once

#ifdef CPPNATIVEDLL_EXPORTS
#define MYDLL __declspec(dllexport)
#else
#define MYDLL __declspec(dllimport)
#endif

class MYDLL CppDllClass
{
public:
	bool TrueFromCLR();

	bool TrueFromCS();

	bool TrueMyself();

	static bool TrueStatic();
};

