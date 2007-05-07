///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  1 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __options_base_dlg__
#define __options_base_dlg__

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

#include <wx/choice.h>
#include <wx/button.h>
#include <wx/panel.h>
#include "wx/wxFlatNotebook/wxFlatNotebook.h"
#include <wx/statline.h>
#include "lexer_configuration.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class OptionsBaseDlg
///////////////////////////////////////////////////////////////////////////////
class OptionsBaseDlg : public wxDialog 
{
	DECLARE_EVENT_TABLE()
private:

	// Private event handlers
	void _wxFB_OnButtonOK( wxCommandEvent& event ){ OnButtonOK( event ); }
	void _wxFB_OnButtonCancel( wxCommandEvent& event ){ OnButtonCancel( event ); }
	void _wxFB_OnButtonApply( wxCommandEvent& event ){ OnButtonApply( event ); }


protected:
	wxFlatNotebook* m_book;
	wxPanel* m_general;
	wxCheckBox* m_checkBoxDisplayFoldMargin;
	wxCheckBox* m_checkBoxMarkFoldedLine;
	wxStaticText* m_staticText1;
	wxChoice* m_foldStyleChoice;
	wxCheckBox* m_displayBookmarkMargin;
	wxStaticText* m_staticText6;
	wxChoice* m_bookmarkShape;
	wxStaticText* m_staticText4;
	wxButton* m_bgColourPicker;
	wxStaticText* m_staticText5;
	wxButton* m_fgColourPicker;
	wxCheckBox* m_highlighyCaretLine;
	wxCheckBox* m_displayLineNumbers;
	wxCheckBox* m_showIndentationGuideLines;
	wxPanel* m_syntaxHighlightPage;
	wxStaticLine* m_staticline1;
	wxButton* m_okButton;
	wxButton* m_cancelButton;
	wxButton* m_applyButton;

	// Virtual event handlers, overide them in your derived class
	virtual void OnButtonOK( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnButtonCancel( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnButtonApply( wxCommandEvent& event ){ event.Skip(); }

private:
	wxPanel *CreateSyntaxHighlightPage();
	wxPanel *CreateGeneralPage();
	wxPanel *CreateLexerPage(wxPanel *parent, LexerConfPtr lexer);

public:
	OptionsBaseDlg( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("Options"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 465,489 ), int style = wxDEFAULT_DIALOG_STYLE );

};

#endif //__options_base_dlg__
