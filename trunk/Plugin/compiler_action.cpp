#include "compiler_action.h"

DEFINE_EVENT_TYPE(wxEVT_BUILD_ADDLINE)
DEFINE_EVENT_TYPE(wxEVT_BUILD_STARTED)

void CompilerAction::AppendLine(const wxString &line)
{
	if( !m_owner)
		return;

	wxCommandEvent event(wxEVT_BUILD_ADDLINE);
	event.SetString(line);
	m_owner->ProcessEvent(event);
}

void CompilerAction::SendStartMsg()
{
	if( !m_owner)
		return;

	wxCommandEvent event(wxEVT_BUILD_STARTED);
	m_owner->ProcessEvent(event);
}

void CompilerAction::OnTimer(wxTimerEvent &event)
{
	wxUnusedVar(event);
	if( m_stop ){
		m_proc->Terminate();
		return;
	}

	wxString data;

	m_proc->HasInput(data);
	if(!data.IsEmpty()){
		//filter out non-intersting messages such as 'Entering Directory' ..
		if(data.Contains(wxT("Entering directory")) || data.Contains(wxT("Leaving directory"))){
			return;
		}
		AppendLine(data);
	}
}

void CompilerAction::OnProcessEnd(wxProcessEvent& event)
{
	wxUnusedVar(event);
	wxString data;
	//read all input before stopping the timer
	
	if( !m_stop ){
		while( m_proc->HasInput(data) ){

			if(!data.IsEmpty()){
				if(data.Contains(wxT("Entering directory")) || data.Contains(wxT("Leaving directory"))){
					data.Clear();
					continue;
				}
				AppendLine(data);
			}
			data.Clear();
		}
	}

	m_timer->Stop();
	m_busy = false;
	m_stop = false;
}
