#include <Windows.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <unordered_map>

#include "IATUtils.h"

HMODULE hModule;
std::string moduleDirectory;

std::string GetModuleDirectory(HMODULE hMod) {
	std::string result;
	char *outputDir;
	char buf;
	unsigned int maxSize = 15;
	unsigned int outputMaxSize;
	outputDir = (char*)calloc(maxSize, sizeof(char));

	while (maxSize == (outputMaxSize = GetModuleFileNameA(hModule, outputDir, maxSize))) {
		maxSize *= 3.F / 2;
		outputDir = (char*)realloc(outputDir, maxSize*sizeof(char));

		if (outputMaxSize == 0) {
			printf("Erreur lors de la recuperation du chemin vers le module 0x%p\n", hMod);
			free(outputDir);
			return "";
		}
	}

	int i;
	for (i = strlen(outputDir); i >= 0 && outputDir[i] != '\\'; i--);
	if (i > 0) {
		outputDir[i + 1] = '\0';
	}
	result = std::string(outputDir);
	free(outputDir);
	return result;
}

DWORD WINAPI startUpThreadGlobalHook(LPVOID args) {
	if (AllocConsole()) {
		freopen("CONOUT$", "w", stdout);
	}
	moduleDirectory = GetModuleDirectory(hModule);

	std::unordered_map<std::string, bool> includeds;

	char currentAppDir[512] = { '\0' };
	GetCurrentDirectoryA(sizeof(currentAppDir)-1, currentAppDir);

	std::cout << "Current application directory : " << currentAppDir << std::endl;
	std::cout << "Module directory : " << moduleDirectory << std::endl;

	std::ifstream fConfig(moduleDirectory + "ptrace_conf.cfg", std::ios::in);
	std::ofstream fDummpIAT(moduleDirectory + "DumpIAT.txt", std::ios::out);

	std::string modules;
	std::getline(fConfig, modules);

	if (fConfig) {
		std::cout << "Including functions..." << std::endl;
		std::string buffer;
		while (std::getline(fConfig, buffer)) {
			includeds[buffer] = true;
		}
		std::cout << "Including OK" << std::endl;

	} else {
		std::cout << "Warning : fichier de config introuvable" << std::endl;
	}


	//IATDumpProcess(fDummpIAT, modules);

	std::cout << "IATs Dumps finished ! Take a look to DumpIAT.txt" << std::endl;

	IATTraceInclude(includeds, modules, nullptr);
	return 0;
}

//Standalone mode
int main(void) {
	CreateThread(NULL, 0, startUpThreadGlobalHook, NULL, 0, NULL);
	auto input = std::string{};
	while (true) {
		getline(std::cin, input);
		if (!input.empty()) {
			WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), input.c_str(), input.size(), nullptr, nullptr);;
			system(input.c_str());
		}
	}

	return 0;
}

/*
BOOL APIENTRY DllMain(HMODULE hMod, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	hModule = hMod;

	switch (ul_reason_for_call) {
		
		case DLL_PROCESS_ATTACH: {
			CreateThread(NULL, 0, startUpThreadGlobalHook, NULL, 0, NULL);
			break;
		}
		
		case DLL_PROCESS_DETACH: {
			break;
		}
	}
	return TRUE;
}*/
