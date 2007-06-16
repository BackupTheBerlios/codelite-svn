#ifndef PROCUTILS_H
#define PROCUTILS_H

#include "map"

#ifdef __WXMSW__
#include <windows.h>
#include <SetupAPI.h>
#include <Psapi.h>
#include <tlhelp32.h>
//#include <vdmdbg.h>

// ToolHelp Function Pointers.
typedef HANDLE (WINAPI *CreateToolhelp32Snapshot_t)(DWORD,DWORD);
typedef BOOL (WINAPI *Process32_t)(HANDLE,LPPROCESSENTRY32);
#endif 

class ProcUtils {

#ifdef __WXMSW__
	CreateToolhelp32Snapshot_t lpfCreateToolhelp32Snapshot;
	Process32_t lpfProcess32First;
	Process32_t lpfProcess32Next;
	HINSTANCE hInstLib;
#endif 

public:
	ProcUtils();
	~ProcUtils();

	void GetProcTree(std::map<unsigned long, bool> &parentsMap, long pid);
};

#endif // PROCUTILS_H
