#include "cl_process.h"

#ifdef __VISUALC__
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif 

clProcess::clProcess(wxEvtHandler* evtHandler, int id)
: wxProcess(evtHandler, id)
, m_pid(-1)
, m_uid(id)
{
}

clProcess::~clProcess()
{
}

long clProcess::GetPid()
{
	return m_pid;
}

void clProcess::SetPid(long pid)
{
	m_pid = pid;
}

void clProcess::Terminate()
{
	wxKill(GetPid(), wxSIGKILL);
	
	// Sleep for 20 ms to allow the process to be killed and 
	// the main frame to handle the event or else we can get 
	// memory leak
	wxMilliSleep( 20 );
}

