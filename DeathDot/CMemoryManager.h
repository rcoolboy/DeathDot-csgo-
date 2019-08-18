#pragma once
#include "stdafx.h"
// its a default openprocess r3 rwm
class CMemoryManager
{
private:
	HANDLE m_hProcess;
	DWORD m_dwProcessId;
	std::vector<MODULEENTRY32> m_Modules;
public:
	bool Attach(const std::string& strProcessName);

	bool GrabModule(const std::string& strModuleName);

	CMemoryManager()
	{
		m_hProcess = INVALID_HANDLE_VALUE;
		m_dwProcessId = 0;
		m_Modules.clear();
	}

	CMemoryManager(const std::string& strProcessName = "csgo.exe")
	{
		m_hProcess = INVALID_HANDLE_VALUE;
		m_dwProcessId = 0;
		m_Modules.clear();
		if (!Attach(strProcessName))
			throw;
	}

	template <class T>
	inline bool r(DWORD dwAddress, T& Value)
	{
		return ReadProcessMemory(m_hProcess, reinterpret_cast<LPVOID>(dwAddress), reinterpret_cast<LPVOID>(&Value), sizeof(T), NULL) == TRUE;
	}

	template <class T>
	inline bool w(DWORD dwAddress, const T& Value)
	{
		return WriteProcessMemory(m_hProcess, reinterpret_cast<LPVOID>(dwAddress), reinterpret_cast<LPCVOID>(&Value), sizeof(T), NULL) == TRUE;
	}

	template <typename datatype>
	datatype rr(DWORD dwAddress)
	{
		datatype buffer;
		ReadProcessMemory(m_hProcess, (LPCVOID)dwAddress, &buffer, sizeof(datatype), NULL);
		return buffer;
	}

	template <typename datatype>
	void rw(DWORD dwAddress, datatype value)
	{
		WriteProcessMemory(m_hProcess, (LPVOID)dwAddress, &value, sizeof(value), NULL);
	}

	// Getters
	HANDLE GetHandle() { return m_hProcess; }
	DWORD GetProcId() { return m_dwProcessId; }
	std::vector<MODULEENTRY32> GetModules() { return m_Modules; }
};