#include "compiler_action.h"

DEFINE_EVENT_TYPE(wxEVT_BUILD_ADDLINE)
DEFINE_EVENT_TYPE(wxEVT_BUILD_STARTED)

void CompilerAction::AppendLine(const wxString &line)
{
	if( !m_owner)
		return;

	wxCommandEvent event(wxEVT_BUILD_ADDLINE);
	event.SetString(line);
	::wxPostEvent(m_owner, event);
}

void CompilerAction::SendStartMsg()
{
	if( !m_owner)
		return;

	wxCommandEvent event(wxEVT_BUILD_STARTED);
	::wxPostEvent(m_owner, event);
}
