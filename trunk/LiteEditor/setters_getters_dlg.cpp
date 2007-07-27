///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifdef WX_PRECOMP

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "setters_getters_dlg.h"

///////////////////////////////////////////////////////////////////////////

SettersGettersDlg::SettersGettersDlg( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Class Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textClassName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	bSizer3->Add( m_textClassName, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Select members to generate setters/getters:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_staticText3, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxArrayString m_checkListMembersChoices;
	m_checkListMembers = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListMembersChoices, 0 );
	bSizer3->Add( m_checkListMembers, 1, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( bSizer3, 1, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer2, 0, wxALIGN_RIGHT, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
}
