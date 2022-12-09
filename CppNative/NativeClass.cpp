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

#include <stdio.h>

int main(int argc, char** argv)
{
	if (NativeClass().TrueFromCS())
		printf("Hello World\n");
	else
		printf("something went terribly wrong");

	return 0;
}
