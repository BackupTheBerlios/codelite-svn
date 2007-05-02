///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  1 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ctags_dialog__
#define __ctags_dialog__

// Define WX_GCH in order to support precompiled headers with GCC compiler.
// You have to create the header "wx_pch.h" and include all files needed
// for compile your gui inside it.
// Then, compile it and place the file "wx_pch.h.gch" into the same
// directory that "wx_pch.h".
#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

#include <wx/statline.h>
#include <wx/button.h>
#include "wx/wxFlatNotebook/wxFlatNotebook.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CtagsOptionsDlg
///////////////////////////////////////////////////////////////////////////////
class CtagsOptionsDlg : public wxDialog 
{
	DECLARE_EVENT_TABLE()

protected:
	wxFlatNotebook* m_book;
	wxStaticLine* m_staticline1;
	wxButton* m_okBtn;
	wxButton* m_closeBtn;

	// Virtual event handlers, overide them in your derived class
	virtual void OnButtonOK( wxCommandEvent& event );
	virtual void OnButtonClose( wxCommandEvent& event );
	virtual void OnClose(wxCloseEvent &event);
public:
	CtagsOptionsDlg( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("CTags Options"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize(513, 350), int style = wxDEFAULT_DIALOG_STYLE );
};

#endif //__ctags_dialog__
