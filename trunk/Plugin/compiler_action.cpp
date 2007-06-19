#include "compiler_action.h"
#include "wx/tokenzr.h"

DEFINE_EVENT_TYPE(wxEVT_BUILD_ADDLINE)
DEFINE_EVENT_TYPE(wxEVT_BUILD_STARTED)
DEFINE_EVENT_TYPE(wxEVT_BUILD_ENDED)

void CompilerAction::AppendLine(const wxString &line)
{
	if( !m_owner)
		return;

	wxCommandEvent event(wxEVT_BUILD_ADDLINE);
	event.SetString(line);
	m_owner->ProcessEvent(event);
}

void CompilerAction::Stop()
{
	m_stop = true;
	//kill the build process
	if(m_proc){
#ifdef __WXGTK__
		wxExecute(wxT("pkill make"));
#else
		m_proc->Terminate();
#endif 
		CleanUp();
	}
}

void CompilerAction::SendStartMsg()
{
	if( !m_owner)
		return;

	wxCommandEvent event(wxEVT_BUILD_STARTED);
	m_owner->ProcessEvent(event);
}

void CompilerAction::SendEndMsg()
{
	if( !m_owner)
		return;

	wxCommandEvent event(wxEVT_BUILD_ENDED);
	m_owner->ProcessEvent(event);
}

void CompilerAction::OnTimer(wxTimerEvent &event)
{
	wxUnusedVar(event);
	if( m_stop ){
		m_proc->Terminate();
		return;
	}
	PrintOutput();
}

void CompilerAction::PrintOutput()
{
	wxString data, errors;
	m_proc->HasInput(data, errors);
	//loop over the lines read from the compiler
	wxStringTokenizer tkz(data, wxT("\n"));
	while(tkz.HasMoreTokens()){
		wxString line = tkz.NextToken();
		if(line.Contains(wxT("Entering directory")) || line.Contains(wxT("Leaving directory"))){
			//skip it
			continue;
		}else{
			//print it
			AppendLine(line + wxT("\n"));
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

void CompilerAction::OnProcessEnd(wxProcessEvent& event)
{
	wxUnusedVar(event);
	//read all input before stopping the timer
	if( !m_stop ){
		PrintOutput();
		CleanUp();
	}
	event.Skip();
}

void CompilerAction::CleanUp()
{
	m_timer->Stop();
	m_busy = false;
	m_stop = false;
	SendEndMsg();
}
