#include <Windows.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <unordered_map>

#include "IATUtils.h"

using namespace std;
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



DWORD WINAPI startUpThreadGlobalHook(LPVOID args)
{
	if (AllocConsole()) {
		freopen("CONOUT$", "w", stdout);
	}
	moduleDirectory = GetModuleDirectory(hModule);

	unordered_map<string, bool> includeds;

	char currentAppDir[512] = { '\0' };
	GetCurrentDirectoryA(sizeof(currentAppDir)-1, currentAppDir);

	cout << "Current application directory : " << currentAppDir << endl;
	cout << "Module directory : " << moduleDirectory << endl;

	ifstream fConfig(moduleDirectory + "ptrace_conf.cfg", ios::in);
	ofstream fDummpIAT(moduleDirectory + "DumpIAT.txt", ios::out);

	std::string modules;
	getline(fConfig, modules);

	if (fConfig) {
		cout << "Including functions..." << endl;
		std::string buffer;
		while (getline(fConfig, buffer)) {
			includeds[buffer] = true;
		}
		cout << "Including OK" << endl;

	}
	else {
		cout << "Warning : fichier de config introuvable" << endl;
	}


	
	IATDumpProcess(fDummpIAT, modules);

	std::cout << "IATs Dumps finished ! Take a look to DumpIAT.txt" << std::endl;

	IATTraceInclude(includeds, modules);
	return 0;
}


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
}
