#include "procutils.h"
#include "map"

ProcUtils::ProcUtils()
{
#ifdef __WXMSW__
	//load kernel32.dll once
	if(hInstLib){
		hInstLib = LoadLibraryA( "Kernel32.DLL" ) ;
		if( hInstLib == NULL ){
			return;
		}
	}

	// Get procedure addresses.
	// We are linking to these functions of Kernel32
	// explicitly, because otherwise a module using
	// this code would fail to load under Windows NT,
	// which does not have the Toolhelp32
	// functions in the Kernel 32.
	lpfCreateToolhelp32Snapshot = (CreateToolhelp32Snapshot_t) GetProcAddress(hInstLib, "CreateToolhelp32Snapshot") ;
	lpfProcess32First = (Process32_t) GetProcAddress(hInstLib, "Process32First") ;
	lpfProcess32Next =  (Process32_t) GetProcAddress(hInstLib, "Process32Next") ;
	if( lpfProcess32Next == NULL || lpfProcess32First == NULL || lpfCreateToolhelp32Snapshot == NULL ){
		FreeLibrary( hInstLib ) ;
		hInstLib = NULL;
		return;
	}
#endif
}

ProcUtils::~ProcUtils()
{
#ifdef __WXMSW__
	if(hInstLib != NULL){
		FreeLibrary( hInstLib );
	}
#endif
}

#ifdef __WXMSW__
void ProcUtils::KillProcTree(long pid)
{
	if(!hInstLib){
		return;
	}

	OSVERSIONINFO osver ;
	HANDLE hProcessSnap = NULL;

	// Check to see if were running under Windows95 or
	// Windows NT.
	osver.dwOSVersionInfoSize = sizeof( osver ) ;
	if( !GetVersionEx( &osver ) ){
		return ;
	}
	
	if( osver.dwPlatformId != VER_PLATFORM_WIN32_NT ){
		return;
	}

	//get child processes of this node
	HANDLE hProcessSnap = lpfCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(!hProcessSnap){
		return;
	}

	//Fill in the size of the structure before using it.
    PROCESSENTRY32 pe;
    wxZeroMemory(pe);
    pe.dwSize = sizeof(PROCESSENTRY32);

	// Walk the snapshot of the processes, and for each process,
    // kill it if its parent is pid.
    if (!lpfProcess32First(hProcessSnap, &pe)) {
        // Can't get first process.
        CloseHandle (hProcessSnap);
        return;
    }

	std::map<DWORD, bool> parentsMap;
	parentsMap[pid] = true;

	do {
		if (parentsMap.find(pe.th32ParentProcessID) != parentsMap.end()) {
			if (wxKill(pe.th32ProcessID, sig, krc)){
				//keep this process id as well
				parentsMap[pe.th32ProcessID] = true;
			}
        }
    } while (lpfProcess32Next (hProcessSnap, &pe));
}
#else
void ProcUtils::KillProcTree(long)
{
	//dont do nothing on other platforms
}
#endif
