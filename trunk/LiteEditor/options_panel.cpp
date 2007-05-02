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

#include "options_panel.h"

///////////////////////////////////////////////////////////////////////////

OptionsPanel::OptionsPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("Special Preprocessors:") ), wxVERTICAL );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	m_macros = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_macros->SetToolTip( wxT("List of Comma separated macros\nto be handled by CTAGS") );
	
	bSizer6->Add( m_macros, 1, wxBOTTOM|wxEXPAND|wxLEFT|wxTOP, 5 );
	
	m_addButton = new wxButton( this, wxID_ANY, wxT("..."), wxDefaultPosition, wxSize( 20,-1 ), 0 );
	bSizer6->Add( m_addButton, 0, wxBOTTOM|wxRIGHT|wxTOP, 5 );
	
	sbSizer2->Add( bSizer6, 1, wxEXPAND, 5 );
	
	bSizer5->Add( sbSizer2, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("General") ), wxVERTICAL );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("File Types:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_staticText3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_fileTypes = new wxTextCtrl( this, wxID_ANY, wxT("*.cxx;*.cc;*.cpp;*.c;*.hpp;*.h;"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_fileTypes, 1, wxALL|wxEXPAND, 5 );
	
	sbSizer4->Add( bSizer7, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Language:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_staticText4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_languagesChoices[] = { wxT("C/C++"), wxT("Java") };
	int m_languagesNChoices = sizeof( m_languagesChoices ) / sizeof( wxString );
	m_languages = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_languagesNChoices, m_languagesChoices, 0 );
	bSizer8->Add( m_languages, 1, wxALL|wxEXPAND, 5 );
	m_languages->SetSelection(0);

	sbSizer4->Add( bSizer8, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	m_parseComments = new wxCheckBox( this, wxID_ANY, wxT("Parse Comments"), wxDefaultPosition, wxDefaultSize, 0 );
	m_parseComments->SetValue(true);
	
	bSizer9->Add( m_parseComments, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer4->Add( bSizer9, 1, wxEXPAND, 5 );
	
	bSizer5->Add( sbSizer4, 0, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer5 );
	this->Layout();
}
