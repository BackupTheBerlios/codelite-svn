#ifndef COMPILER_ACTION_H
#define COMPILER_ACTION_H

#include "worker_thread.h"
#include "wx/event.h"
#include "cl_process.h"

DECLARE_EVENT_TYPE(wxEVT_BUILD_ADDLINE, wxID_ANY)
DECLARE_EVENT_TYPE(wxEVT_BUILD_STARTED, wxID_ANY)

class CompilerAction : public ThreadRequest {
protected:
	clProcess *m_proc;
	wxEvtHandler *m_owner;

public:
	//construct a compiler action 
	// \param owner the window owner for this action
	CompilerAction(wxEvtHandler *owner) 
		: m_proc(NULL)
		, m_owner(owner)
	{};
	
	virtual ~CompilerAction()
	{};
	
	virtual void Process() = 0;

	void AppendLine(const wxString &line);
	void SendStartMsg();
};
#endif 
