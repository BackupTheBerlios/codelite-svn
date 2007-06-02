#include "compile_request.h"
#include "buildmanager.h"
#include "read_proc_input_request.h"
#include "wx/process.h"

CompileRequest::CompileRequest(wxEvtHandler *owner, const wxString &projectName)
: CompilerAction(owner)
, m_project(projectName)
{
}

CompileRequest::~CompileRequest()
{
	if( m_proc ){
		delete m_proc;
		m_proc = NULL;
	}
}

//do the actual cleanup
void CompileRequest::Process()
{
	wxString cmd;
	SetBusy(true);

	//TODO:: make the builder name configurable
	BuilderPtr builder = BuildManagerST::Get()->GetBuilder(wxT("GNU makefile for g++/gcc"));
	cmd = builder->GetBuildCommand(m_project);

	SendStartMsg();

	m_proc = new clProcess(wxNewId(), cmd);
	if(m_proc){
		m_proc->Start();
		Connect(wxEVT_TIMER, wxTimerEventHandler(CompileRequest::OnTimer), NULL, this);
		m_proc->Connect(wxEVT_END_PROCESS, wxProcessEventHandler(CompileRequest::OnProcessEnd), NULL, this);
		m_timer->Start(10);
	}
}
