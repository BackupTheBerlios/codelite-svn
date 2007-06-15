#include "async_executable_cmd.h"
#include "wx/tokenzr.h"

DEFINE_EVENT_TYPE(wxEVT_ASYNC_PROC_ADDLINE)
DEFINE_EVENT_TYPE(wxEVT_ASYNC_PROC_STARTED)
DEFINE_EVENT_TYPE(wxEVT_ASYNC_PROC_ENDED)

AsyncExeCmd::~AsyncExeCmd()
{
	delete m_timer;
	m_timer = NULL;
	if(m_proc) {
		delete m_proc;
		m_proc = NULL;
	}
};

void AsyncExeCmd::AppendLine(const wxString &line)
{
	if( !m_owner)
		return;

	wxCommandEvent event(wxEVT_ASYNC_PROC_ADDLINE);
	event.SetString(line);
	m_owner->ProcessEvent(event);
}

void AsyncExeCmd::Stop()
{
	m_stop = true;
	//kill the build process
	if(m_proc){
		m_proc->Terminate();
	}
}

void AsyncExeCmd::SendStartMsg()
{
	if( !m_owner)
		return;

	wxCommandEvent event(wxEVT_ASYNC_PROC_STARTED);
	event.SetString(m_cmdLine + wxT("\n"));
	m_owner->ProcessEvent(event);
}

void AsyncExeCmd::SendEndMsg(int exitCode)
{
	if( !m_owner)
		return;

	wxCommandEvent event(wxEVT_ASYNC_PROC_ENDED);

	wxString message(wxT("Program exited with return code: "));
	message << exitCode << wxT("\n");
	event.SetString(message);
	m_owner->ProcessEvent(event);
}

void AsyncExeCmd::OnTimer(wxTimerEvent &event)
{
	wxUnusedVar(event);
	if( m_stop ){
		m_proc->Terminate();
		return;
	}
	PrintOutput();
}

void AsyncExeCmd::PrintOutput()
{
	wxString data, errors;
	m_proc->HasInput(data, errors);

	if(!data.IsEmpty()){
		wxStringTokenizer tt(data, wxT("\n"));
		while(tt.HasMoreTokens()){
			AppendLine(tt.NextToken() + wxT("\n"));
		}
	}

	if(!errors.IsEmpty()){
		wxStringTokenizer tt(errors, wxT("\n"));
		while(tt.HasMoreTokens()){
			AppendLine(tt.NextToken() + wxT("\n"));
		}
	}
	
	data.Clear();
	errors.Clear();
}

void AsyncExeCmd::ProcessEnd(wxProcessEvent& event)
{
	//read all input before stopping the timer
	if( !m_stop ){
		PrintOutput();
	}
	
	m_timer->Stop();
	m_busy = false;
	m_stop = false;

	SendEndMsg(event.GetExitCode());
}

void AsyncExeCmd::Execute(const wxString &cmdLine)
{
	m_cmdLine = cmdLine;
	SetBusy(true);
	SendStartMsg();

	m_proc = new clProcess(wxNewId(), m_cmdLine);
	if(m_proc){
		if(m_proc->Start(false) == 0){
			delete m_proc;
			m_proc = NULL;
			SetBusy(false);
		}else{
			Connect(wxEVT_TIMER, wxTimerEventHandler(AsyncExeCmd::OnTimer), NULL, this);
			m_timer->Start(10);
		}
	}
}

void AsyncExeCmd::Terminate()
{
	m_proc->Terminate();
}

