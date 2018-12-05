#define ASMUTILS_LIBRARY

#include <cstdio>
#include "ASMUtils.h"

PVOID ASMUtils::dynamicallyExecute(BYTE* code, unsigned int memCodeSize, bool argsPresent, PVOID arg) {
	SYSTEM_INFO sysInfos;
	GetSystemInfo(&sysInfos);

	typedef PVOID(*DYNAMIC_FUNC_EX)(PVOID);
	typedef PVOID(*DYNAMIC_FUNC_EX_NO_ARG)(void);

	unsigned int AlignedRangeSize = ((memCodeSize / sysInfos.dwPageSize) *sysInfos.dwPageSize) + sysInfos.dwPageSize;
	DYNAMIC_FUNC_EX dynamicCode = reinterpret_cast<DYNAMIC_FUNC_EX>(VirtualAlloc(nullptr, AlignedRangeSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE));
	DYNAMIC_FUNC_EX_NO_ARG dynamicCodeNoArg = reinterpret_cast<DYNAMIC_FUNC_EX_NO_ARG>(dynamicCode);
	memcpy(reinterpret_cast<PVOID>(dynamicCode), code, memCodeSize);
	
	PVOID ret = 0;
	if (argsPresent) {
		ret = (dynamicCode)(arg);
	} else {
		ret = (dynamicCodeNoArg)();
	}

	VirtualFree(reinterpret_cast<PVOID>(dynamicCode), 0, MEM_RELEASE);
	return ret;
}

//TODO template (N = bits (64, 32, ...)
void ASMUtils::reverseAddressx64(DWORD64 address, BYTE* reversedArrayAddress) {
	DWORD64 mask = 0xFF00000000000000;
	int octalAddressLength = sizeof(mask);

	for (int i = 0; i < octalAddressLength; i++) {
		int bitsRightShift = (i * 8);
		int reverseBitsRightShift = (octalAddressLength - 1 - i) * 8;

		auto newByte = static_cast<unsigned char>((address & (mask >> bitsRightShift)) >> reverseBitsRightShift);

		reversedArrayAddress[octalAddressLength - 1 - i] = newByte;
	}
}

void ASMUtils::reverseAddressx86(DWORD address, BYTE* reversedArrayAddress) {
	DWORD mask = 0xFF000000;
	int octalAddressLength = sizeof(mask);

	for (auto i = 0; i < octalAddressLength; i++) {
		int bitsRightShift = (i * 8);
		int reverseBitsRightShift = (octalAddressLength - 1 - i) * 8;

		auto newByte = static_cast<unsigned char>((address & (mask >> bitsRightShift)) >> reverseBitsRightShift);

		reversedArrayAddress[octalAddressLength - 1 - i] = newByte;
	}
}

void ASMUtils::printCode(BYTE* code, unsigned int size) {
	for (unsigned int i = 0; i < size; i++) {
		BYTE b = code[i];
		printf("{0x%x}, ", b);
	}
}

//TODO factorisation
PVOID ASMUtils::writeAssembly(BYTE* code, unsigned int size) {
	SYSTEM_INFO sysInfos;
	GetSystemInfo(&sysInfos);

	DWORDPTR AlignedRangeSize = ((size / sysInfos.dwPageSize) * sysInfos.dwPageSize) + sysInfos.dwPageSize;
	PVOID dynamicCode = VirtualAlloc(nullptr, AlignedRangeSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	memcpy(dynamicCode, code, size);
	
	return dynamicCode;
}

PVOID ASMUtils::writeAssembly(HANDLE pHandle, BYTE* code, unsigned int size) {
	SYSTEM_INFO sysInfos;
	GetSystemInfo(&sysInfos);

	DWORDPTR AlignedRangeSize = ((size / sysInfos.dwPageSize) * sysInfos.dwPageSize) + sysInfos.dwPageSize;
	PVOID dynamicCode = VirtualAllocEx(pHandle, nullptr, AlignedRangeSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(pHandle, dynamicCode, code, size, nullptr);

	return dynamicCode;
}

