#include "setters_getters_dlg.h"

SettersGettersDlg::SettersGettersDlg(wxWindow* parent, const std::vector<TagEntryPtr> &tags, const wxFileName &file, int lineno)
: SettersGettersBaseDlg(parent)
, m_members(tags)
, m_file(file)
, m_lineno(lineno)
{
	//convert the tags to string array
	wxArrayString members;
	for(size_t i=0; i<tags.size() ; i++){
		members.Add(tags.at(i)->GetName());
	}
	
	//append all members to the check list
	m_checkListMembers->Clear();
	m_checkListMembers->Append(members);
	
	//check all items
	for(unsigned int i=0; i<m_checkListMembers->GetCount(); i++)
	{
		m_checkListMembers->Check(i, true);
	}
	
	if(tags.empty() == false){
		m_textClassName->SetValue(tags.at(0)->GetParent());
	}
}


