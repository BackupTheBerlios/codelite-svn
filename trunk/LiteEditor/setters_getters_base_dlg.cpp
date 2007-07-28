///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  6 2007)
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

#include "setters_getters_base_dlg.h"

///////////////////////////////////////////////////////////////////////////

SettersGettersBaseDlg::SettersGettersBaseDlg( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Class Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer3->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textClassName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	bSizer3->Add( m_textClassName, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Select members from the list below :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer3->Add( m_staticText3, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxArrayString m_checkListMembersChoices;
	m_checkListMembers = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListMembersChoices, 0 );
	bSizer3->Add( m_checkListMembers, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("Options:") ), wxVERTICAL );
	
	m_checkKeepUnderscope = new wxCheckBox( this, wxID_ANY, wxT("Preserve underscores in member name"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer1->Add( m_checkKeepUnderscope, 0, wxALL, 5 );
	
	m_checkStartWithUppercase = new wxCheckBox( this, wxID_ANY, wxT("Function name starts with upper case letter"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer1->Add( m_checkStartWithUppercase, 0, wxALL, 5 );
	
	m_checkRemoveMUnderscore = new wxCheckBox( this, wxID_ANY, wxT("Remove 'm_' member prefix from function name"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer1->Add( m_checkRemoveMUnderscore, 0, wxALL, 5 );
	
	bSizer3->Add( sbSizer1, 0, wxALL|wxEXPAND, 5 );
	
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
