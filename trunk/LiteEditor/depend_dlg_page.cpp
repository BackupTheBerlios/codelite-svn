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

#include "depend_dlg_page.h"

///////////////////////////////////////////////////////////////////////////

DependenciesPage::DependenciesPage( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Projects:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_staticText1, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxArrayString m_checkListProjectListChoices;
	m_checkListProjectList = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListProjectListChoices, 0 );
	bSizer3->Add( m_checkListProjectList, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Build Order:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_staticText2, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_listBoxBuildOrder = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer3->Add( m_listBoxBuildOrder, 1, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( bSizer3, 1, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}
