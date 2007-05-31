#include "clean_request.h"
#include "procutils.h"
#include "buildmanager.h"
#include "read_proc_input_request.h"
#include "wx/process.h"

CleanRequest::CleanRequest(const wxString &projectName)
: wxEvtHandler()
, m_project(projectName)
{
	m_timer = new wxTimer(this);
}

CleanRequest::~CleanRequest()
{
	delete m_timer;
	if( m_proc ){
		delete m_proc;
		m_proc = NULL;
	}
}

void CleanRequest::OnTimer(wxTimerEvent &event)
{
	wxUnusedVar(event);
	wxString data;
	m_proc->HasInput(data);
}

void CleanRequest::OnProcessEnd(wxProcessEvent& event)
{
	wxUnusedVar(event);
	wxString data;
	//read all input before stopping the timer
	while( m_proc->HasInput(data) )
		;
	m_timer->Stop();
}

//do the actual cleanup
void CleanRequest::Process()
{
	wxString cmd;
	//TODO:: make the builder name configurable
	BuilderPtr builder = BuildManagerST::Get()->GetBuilder(wxT("GNU makefile for g++/gcc"));
	cmd = builder->GetCleanCommand(m_project);

	m_proc = new clProcess(wxNewId(), cmd);
	if(m_proc){
		m_proc->Start();
		m_proc->Connect(wxEVT_END_PROCESS, wxProcessEventHandler(CleanRequest::OnProcessEnd), NULL, this);
		m_timer->Connect(wxEVT_TIMER, wxTimerEventHandler(CleanRequest::OnTimer), NULL, this);
		m_timer->Start(100);
	}
}
