#include "clean_request.h"
#include "buildmanager.h"
#include "read_proc_input_request.h"
#include "wx/process.h"

CleanRequest::CleanRequest(wxEvtHandler *owner, const wxString &projectName)
: CompilerAction(owner)
, m_project(projectName)
{
	
}

CleanRequest::~CleanRequest()
{
	if( m_proc ){
		delete m_proc;
		m_proc = NULL;
	}
}


//do the actual cleanup
void CleanRequest::Process()
{
	wxString cmd;
	SetBusy(true);

	//TODO:: make the builder name configurable
	BuilderPtr builder = BuildManagerST::Get()->GetBuilder(wxT("GNU makefile for g++/gcc"));
	cmd = builder->GetCleanCommand(m_project);

	SendStartMsg();

	m_proc = new clProcess(wxNewId(), cmd);
	if(m_proc){
		if(m_proc->Start() == 0){
			SetBusy(false);
			return;
		}
		Connect(wxEVT_TIMER, wxTimerEventHandler(CleanRequest::OnTimer), NULL, this);
		m_proc->Connect(wxEVT_END_PROCESS, wxProcessEventHandler(CleanRequest::OnProcessEnd), NULL, this);
		m_timer->Start(10);
	}
}
