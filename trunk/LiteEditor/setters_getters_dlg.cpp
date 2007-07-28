#include "setters_getters_dlg.h"

SettersGettersDlg::SettersGettersDlg(wxWindow* parent, const wxArrayString &members)
: SettersGettersBaseDlg(parent)
, m_members(members)
{
	//append all members to the check list
	m_checkListMembers->Clear();
	m_checkListMembers->Append(m_members);
	//check all items
	for(unsigned int i=0; i<m_checkListMembers->GetCount(); i++)
	{
		m_checkListMembers->Check(i, true);
	}
}
