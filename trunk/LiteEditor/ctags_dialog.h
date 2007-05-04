///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  1 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ctags_dialog__
#define __ctags_dialog__

#include "wx/dialog.h"
#include <wx/statline.h>
#include <wx/button.h>
#include "wx/textctrl.h"
#include "wx/stattext.h"
#include "wx/choice.h"
#include "wx/checkbox.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CtagsOptionsDlg
///////////////////////////////////////////////////////////////////////////////
  
class CtagsOptionsDlg : public wxDialog 
{
	DECLARE_EVENT_TABLE()
private:
	// Options page
	wxTextCtrl* m_macros;
	wxStaticText* m_staticText3;
	wxTextCtrl* m_fileTypes;
	wxStaticText* m_staticText4;
	wxChoice* m_languages;
	wxCheckBox* m_parseComments;

	// methods
	wxPanel *CreateGeneralPage();

protected:
	wxStaticLine* m_staticline1;
	wxButton* m_okBtn;
	wxButton* m_closeBtn;

	// Virtual event handlers, overide them in your derived class
	virtual void OnButtonOK( wxCommandEvent& event );
	virtual void OnButtonClose( wxCommandEvent& event );
	virtual void OnClose(wxCloseEvent &event);
public:
	CtagsOptionsDlg( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("CTags Options"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize(513, 300), int style = wxDEFAULT_DIALOG_STYLE );
};

#endif //__ctags_dialog__
