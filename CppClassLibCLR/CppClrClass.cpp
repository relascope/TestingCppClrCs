#include "pch.h"

#include "CppClrClass.h"

bool CppClrClass::TrueCS()
{
	return CsLib::CsClass::TrueStatic();
}


bool CppClrClass::TrueMyself()
{
	return true;
}