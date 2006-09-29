#ifndef CODELITE_TAGSPROCESS_H
#define CODELITE_TAGSPROCESS_H

#include <wx/process.h>

namespace codelite
{

class TagsProcess : public wxProcess
{
	long m_pid;

public:
	TagsProcess(wxEvtHandler* evtHandler, int id);
	TagsProcess(int flags);
	virtual ~TagsProcess();

	long GetPid();
	void SetPid(long pid);
};

} // codelite

#endif // CODELITE_TAGSPROCESS_H
