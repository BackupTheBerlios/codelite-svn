///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  1 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __lexer_page__
#define __lexer_page__

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

#include <wx/button.h>
#include "lexer_configuration.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class LexerPage
///////////////////////////////////////////////////////////////////////////////
class LexerPage : public wxPanel 
{
	LexerConfPtr m_lexer;
	DECLARE_EVENT_TABLE()
private:
	
	// Private event handlers
	void _wxFB_OnItemSelected( wxCommandEvent& event ){ OnItemSelected( event ); }

protected:
	wxListBox* m_properties;
	wxButton* m_fontPicker;
	wxButton* m_colourPicker;

	// Virtual event handlers, overide them in your derived class
	virtual void OnItemSelected( wxCommandEvent& event ){ event.Skip(); }


public:
	LexerPage( wxWindow* parent, LexerConfPtr lexer, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 285,300 ), int style = wxTAB_TRAVERSAL );
};

#endif //__lexer_page__
