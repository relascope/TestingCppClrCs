#include "pch.h"

#include "CppClrClass.h"
using namespace System;
using namespace ::System::Reflection;

bool CppClrClass::TrueCS()
{
	return CsLib::CsClass::TrueStatic();
}


bool CppClrClass::TrueMyself()
{
	return true;
}

using namespace System::IO;

Assembly^ OnAssemblyResolve(System::Object^ obj, System::ResolveEventArgs^ args)
{
	auto requestedName = gcnew AssemblyName(args->Name);

  auto assemblyPath = Path::GetFullPath(Path::Combine("./", requestedName->Name + ".dll"));
  if (!File::Exists(assemblyPath)) {
    assemblyPath = Path::GetFullPath(Path::Combine("./", requestedName->Name + ".exe"));
    if (!File::Exists(assemblyPath))
      return nullptr;
  }
  
  auto assembly = Assembly::LoadFrom(assemblyPath);
  return assembly;
}

void CppClrClass::SetUpAssemblyResolve()
{
	AppDomain::CurrentDomain->AssemblyResolve += gcnew System::ResolveEventHandler(&OnAssemblyResolve);
}