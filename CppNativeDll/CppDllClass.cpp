#include "pch.h"
#include "CppDllClass.h"
#include <CppClrClass.h>


bool CppDllClass::TrueFromCS()
{
	return CppClrClass::TrueCS();
}

bool CppDllClass::TrueFromCLR()
{
	return CppClrClass().TrueMyself();
}

bool CppDllClass::TrueMyself()
{
	return true;
}

bool CppDllClass::TrueStatic()
{
	return true;
}