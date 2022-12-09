#include "pch.h"
#include "CppDllClass.h"
#include <CppClrClass.h>


bool CppDllClass::TrueFromCLR()
{
	return CppClrClass::TrueCS();
}

bool CppDllClass::TrueMyself()
{
	return true;
}