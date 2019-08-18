#include "stdafx.h"


bool CMemoryManager::Attach(const std::string& strProcessName)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hSnapshot == INVALID_HANDLE_VALUE) return false;
	PROCESSENTRY32 ProcEntry;
	ProcEntry.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hSnapshot, &ProcEntry))
	{
		if (!strcmp(ProcEntry.szExeFile, strProcessName.c_str()))
		{
			CloseHandle(hSnapshot);
			m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcEntry.th32ProcessID);
			m_dwProcessId = ProcEntry.th32ProcessID;
			return true;
		}
	}
	else
	{
		CloseHandle(hSnapshot);
		return false;
	}
	while (Process32Next(hSnapshot, &ProcEntry))
	{
		if (!strcmp(ProcEntry.szExeFile, strProcessName.c_str()))
		{
			m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcEntry.th32ProcessID);
			m_dwProcessId = ProcEntry.th32ProcessID;
			return true;
		}
	}
	CloseHandle(hSnapshot);
	return false;
}

bool CMemoryManager::GrabModule(const std::string& strModuleName)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_dwProcessId);
	if (hSnapshot == INVALID_HANDLE_VALUE) return false;
	MODULEENTRY32 ModEntry;
	ModEntry.dwSize = sizeof(MODULEENTRY32);
	if (Module32First(hSnapshot, &ModEntry))
	{
		if (!strcmp(ModEntry.szModule, strModuleName.c_str()))
		{
			CloseHandle(hSnapshot);
			m_Modules.push_back(ModEntry);
			return true;
		}
	}
	else
	{
		CloseHandle(hSnapshot);
		return false;
	}
	while (Module32Next(hSnapshot, &ModEntry))
	{
		if (!strcmp(ModEntry.szModule, strModuleName.c_str()))
		{
			CloseHandle(hSnapshot);
			m_Modules.push_back(ModEntry);
			return true;
		}
	}
	CloseHandle(hSnapshot);
	return false;
}