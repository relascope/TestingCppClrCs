C++ Testing for C++ Native to C++ CLR to C#.net
=========================================================

Microsoft C++ Testing
---------------------
At the moment, the test cases calling into C# code are building, but failing when running. 

When the Test-Project is configured to compile with .NET CLR (/clr), the tests are not discovered any more in the TestExplorer. 


Google-Test
-----------
Tests succeed, when calling into C#
Compiling wiht /clr possible, tests discovered
Exception in Test-discovery when linking against .obj-file

Projects
========

ClassLibraryCS
--------------
C# class

CppClassLibCLR
--------------
C++ calling C# class

CppNative
---------
C++ native calling C++/CLR
main: here the calls into the C#-class succeed
no symbols exported

CppNativeDll
------------
C++ native Dll calling C++/CLR
Exporting symbols

TestingCppClrCs
---------------
Tests calling into the different projects
references CppNative via obj-file (and therefore also CppClassLibCLR via lib)
references CppNativeDll via lib

GoogleTestNugetConsole
----------------------
Google-Test Project
references CppNativeDll


