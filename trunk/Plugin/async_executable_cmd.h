#ifndef ASYNC_EXECUTABLE_CMD_H
#define ASYNC_EXECUTABLE_CMD_H

#include "wx/event.h"
#include "cl_process.h"
#include "wx/timer.h"

DECLARE_EVENT_TYPE(wxEVT_ASYNC_PROC_ADDLINE, wxID_ANY)
DECLARE_EVENT_TYPE(wxEVT_ASYNC_PROC_STARTED, wxID_ANY)
DECLARE_EVENT_TYPE(wxEVT_ASYNC_PROC_ENDED, wxID_ANY)

class AsyncExeCmd : public wxEvtHandler {
protected:
	clProcess *m_proc;
	wxEvtHandler *m_owner;
	wxTimer *m_timer;
	bool m_busy;
	bool m_stop;
	wxString m_cmdLine;
	
protected:
	virtual void OnTimer(wxTimerEvent &event);
	virtual void OnProcessEnd(wxProcessEvent& event);
	virtual void PrintOutput();

public:
	bool IsBusy() const { return m_busy; }
	void SetBusy(bool busy) { m_busy = busy; }
	void Stop();

public:
	//construct a compiler action 
	// \param owner the window owner for this action
	AsyncExeCmd(wxEvtHandler *owner) 
		: m_proc(NULL)
		, m_owner(owner)
		, m_busy(false)
		, m_stop(false)
	{
		m_timer = new wxTimer(this);
	};
	
	virtual ~AsyncExeCmd()
	{
		delete m_timer;
		if(m_proc) delete m_proc;
	};
	
	virtual void Execute(const wxString &cmdLine);

	wxOutputStream *GetOutputStream() { 
		if(m_proc){
			return m_proc->GetOutputStream(); 
		}
		return NULL;
	}

	void AppendLine(const wxString &line);
	void SendStartMsg();
	void SendEndMsg(int exitCode);
};

#endif // ASYNC_EXECUTABLE_CMD_H

