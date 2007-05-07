///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  1 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "lexer_page.h"

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( LexerPage, wxPanel )
EVT_LISTBOX( wxID_ANY, LexerPage::_wxFB_OnItemSelected )
END_EVENT_TABLE()

LexerPage::LexerPage( wxWindow* parent, LexerConfPtr lexer, int id, wxPoint pos, wxSize size, int style ) 
: wxPanel(parent, id, pos, size, style)
, m_lexer(lexer)
{
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("Display Item:") ), wxHORIZONTAL );

	m_properties = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	sbSizer5->Add( m_properties, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );

	m_fontPicker = new wxButton( this, wxID_ANY, wxT("Change Font..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_fontPicker, 0, wxALL|wxEXPAND, 5 );

	m_colourPicker = new wxButton( this, wxID_ANY, wxT("Change Colour..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_colourPicker, 0, wxALL|wxEXPAND, 5 );

	sbSizer5->Add( bSizer7, 1, wxEXPAND, 5 );

	bSizer6->Add( sbSizer5, 1, wxEXPAND, 5 );

	this->SetSizer( bSizer6 );
	this->Layout();
}
