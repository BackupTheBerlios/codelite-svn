///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __setters_getters_base_dlg__
#define __setters_getters_base_dlg__

#include <wx/wx.h>

#include <wx/checklst.h>
#include <wx/statline.h>
#include <wx/button.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SettersGettersBaseDlg
///////////////////////////////////////////////////////////////////////////////
class SettersGettersBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText2;
		wxTextCtrl* m_textClassName;
		wxStaticText* m_staticText3;
		wxCheckListBox* m_checkListMembers;
		wxCheckBox* m_checkKeepUnderscope;
		wxCheckBox* m_checkStartWithUppercase;
		wxCheckBox* m_checkRemoveMUnderscore;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
	
	public:
		SettersGettersBaseDlg( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("Generate Setters/Getters for class"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 479,398 ), int style = wxDEFAULT_DIALOG_STYLE );
	
};

#endif //__setters_getters_base_dlg__
