///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __envvar_table__
#define __envvar_table__

#include <wx/wx.h>

#include <wx/listctrl.h>
#include <wx/statline.h>
#include <wx/button.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EnvVarsTableDlg
///////////////////////////////////////////////////////////////////////////////
class EnvVarsTableDlg : public wxDialog 
{
	private:
	
	protected:
		wxListCtrl* m_listVarsTable;
		wxStaticLine* m_staticline4;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
		wxButton* m_buttonNew;
	
	public:
		EnvVarsTableDlg( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("Environment Variables"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 552,330 ), int style = wxDEFAULT_DIALOG_STYLE );
	
};

#endif //__envvar_table__
