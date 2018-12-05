#pragma once
#include <Windows.h>

#ifndef DWORDPTR
#ifdef _WIN64
#define DWORDPTR DWORD64
#else
#define DWORDPTR DWORD
#endif
#endif

#if defined(ASMUTILS_LIBRARY)
#  define ASMUTILSSHARED_EXPORT __declspec( dllexport )
#else
#  define ASMUTILSSHARED_EXPORT __declspec( dllimport )
#endif

class ASMUTILSSHARED_EXPORT ASMUtils {
public:
	static void reverseAddressx64(DWORD64 address, BYTE* reversedArrayAddress);
	static void reverseAddressx86(DWORD address, BYTE* reversedArrayAddress);
	static PVOID dynamicallyExecute(BYTE* code, unsigned int memCodeSize, bool argsPresent, PVOID arg);
	static PVOID writeAssembly(BYTE* code, unsigned int size);
	static PVOID writeAssembly(HANDLE pHandle, BYTE* code, unsigned int size);
	static void printCode(BYTE* code, unsigned int size);
	~ASMUtils() = default;

private:
	ASMUtils() = default;
	
};

