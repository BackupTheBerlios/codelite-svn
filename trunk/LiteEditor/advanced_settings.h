///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __advanced_settings__
#define __advanced_settings__

#include <wx/wx.h>

#include <wx/button.h>
#include <wx/statline.h>
#include <wx/wxFlatNotebook/wxFlatNotebook.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class AdvancedDlg
///////////////////////////////////////////////////////////////////////////////
class AdvancedDlg : public wxDialog 
{
	DECLARE_EVENT_TABLE();
	
protected:
	wxFlatNotebook* m_notebook; 
	wxPanel* m_compilersPage;
	wxStaticText* m_staticText1;
	wxButton* m_buttonNewCompiler;
	wxStaticLine* m_staticline2;
	wxFlatNotebook* m_compilersNotebook; 
	wxStaticLine* m_staticline10;
	wxButton* m_buttonOK;
	wxButton* m_buttonCancel;

	void OnButtonNewClicked(wxCommandEvent &);
	void OnButtonOKClicked(wxCommandEvent &);
	void OnDeleteCompiler(wxCommandEvent &);
	void LoadCompilers();
	
public:
	AdvancedDlg( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("Advanced"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 642,587 ), int style = wxDEFAULT_DIALOG_STYLE );

};

#endif //__advanced_settings__
