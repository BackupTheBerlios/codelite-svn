///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __advanced_settings__
#define __advanced_settings__

#include <wx/wx.h>

#include <wx/wxFlatNotebook/wxFlatNotebook.h>
#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/button.h>
#include "compiler_page.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class AdvancedDlg
///////////////////////////////////////////////////////////////////////////////
class AdvancedDlg : public wxDialog 
{
	private:
	
	protected:
		wxFlatNotebook* m_notebook; 
		wxPanel* m_compilersPage;
		wxFlatNotebook* m_compilersNotebook; 
		wxStaticLine* m_staticline10;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
	
	public:
		AdvancedDlg( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("Advanced"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 581,415 ), int style = wxDEFAULT_DIALOG_STYLE );
	
};

#endif //__advanced_settings__
