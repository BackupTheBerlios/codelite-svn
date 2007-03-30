#include "process.h"

#ifdef __VISUALC__
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif 


TagsProcess::TagsProcess(int flags)
: wxProcess(flags)
, m_pid(-1)
{
}

TagsProcess::TagsProcess(wxEvtHandler* evtHandler, int id)
: wxProcess(evtHandler, id)
, m_pid(-1)
{
}

TagsProcess::~TagsProcess()
{
}

long TagsProcess::GetPid()
{
	return m_pid;
}

void TagsProcess::SetPid(long pid)
{
	m_pid = pid;
}


