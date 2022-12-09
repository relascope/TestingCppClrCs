#include "NativeClass.h"
#include <CppClrClass.h>

bool NativeClass::TrueInstance() { return true; }

bool NativeClass::TrueStatic() { return true; }

bool NativeClass::TrueFromCS()
{
	return CppClrClass::TrueCS();
}

bool NativeClass::TrueFromCppCLR()
{
	return CppClrClass().TrueMyself();
}
