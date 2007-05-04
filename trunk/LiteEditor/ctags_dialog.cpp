///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  1 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "ctags_dialog.h"
#include "ctags_manager.h"
#include "manager.h"

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( CtagsOptionsDlg, wxDialog )
	EVT_BUTTON( wxID_OK, CtagsOptionsDlg::OnButtonOK )
	EVT_BUTTON( wxID_CANCEL, CtagsOptionsDlg::OnButtonClose )
	EVT_CLOSE(CtagsOptionsDlg::OnClose)
END_EVENT_TABLE()

CtagsOptionsDlg::CtagsOptionsDlg( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxDialog( parent, id, title, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	mainSizer->Add(CreateGeneralPage(), 1, wxEXPAND | wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* btnSizer;
	btnSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_okBtn = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_okBtn, 0, wxALL, 5 );
	
	m_closeBtn = new wxButton( this, wxID_CANCEL, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_closeBtn, 0, wxALL, 5 );
	
	mainSizer->Add( btnSizer, 0, wxALIGN_RIGHT, 5 );
	
	SetSizer( mainSizer );

	GetSizer()->Fit(this);
	GetSizer()->SetMinSize(500, 270);
	GetSizer()->SetSizeHints(this);
}

wxPanel *CtagsOptionsDlg::CreateGeneralPage()
{
	CtagsOptions options = TagsManagerST::Get()->GetCtagsOptions();

	wxPanel *page = new wxPanel(this);
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( page, -1, wxT("Special Preprocessors:") ), wxVERTICAL );
	
	m_macros = new wxTextCtrl( page, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_macros->SetToolTip( wxT("List of Comma separated macros\nto be handled by CTAGS") );
	m_macros->SetValue(options.GetIgnoreMacros());
	sbSizer2->Add( m_macros, 1, wxEXPAND | wxALL, 5 );
	
	bSizer5->Add( sbSizer2, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( page, -1, wxT("General") ), wxVERTICAL );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText3 = new wxStaticText( page, wxID_ANY, wxT("File Types:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_staticText3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_fileTypes = new wxTextCtrl( page, wxID_ANY, options.GetFileSpec(), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_fileTypes, 1, wxALL|wxEXPAND, 5 );

	sbSizer4->Add( bSizer7, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText4 = new wxStaticText( page, wxID_ANY, wxT("Language:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_staticText4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_languagesChoices[] = { wxT("C++"), wxT("Java") };
	int m_languagesNChoices = sizeof( m_languagesChoices ) / sizeof( wxString );
	m_languages = new wxChoice( page, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_languagesNChoices, m_languagesChoices, 0 );
	bSizer8->Add( m_languages, 1, wxALL|wxEXPAND, 5 );
	m_languages->SetSelection(0);
	m_languages->SetStringSelection(options.GetLanguage());

	sbSizer4->Add( bSizer8, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	m_parseComments = new wxCheckBox( page, wxID_ANY, wxT("Parse Comments"), wxDefaultPosition, wxDefaultSize, 0 );
	m_parseComments->SetValue(true);
	
	bSizer9->Add( m_parseComments, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer4->Add( bSizer9, 1, wxEXPAND, 5 );
	
	bSizer5->Add( sbSizer4, 0, wxALL|wxEXPAND, 5 );
	
	page->SetSizer( bSizer5 );
	page->Layout();
	return page;
}

void CtagsOptionsDlg::OnButtonClose(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EndModal(wxID_CANCEL);
}

void CtagsOptionsDlg::OnButtonOK(wxCommandEvent &event)
{
	CtagsOptions options;
	options.SetFileSpec(m_fileTypes->GetValue());
	options.SetIgnoreMacros(m_macros->GetValue());
	options.SetLanguage(m_languages->GetStringSelection());
	TagsManagerST::Get()->SetCtagsOptions( options );

	// save new settings to the workspace
	ManagerST::Get()->SetWorkspaceCtagsOptions( options );
	EndModal(wxID_OK);
	wxUnusedVar(event);
}

void CtagsOptionsDlg::OnClose(wxCloseEvent &event)
{
	wxUnusedVar(event);
	EndModal(wxID_CANCEL);
}

