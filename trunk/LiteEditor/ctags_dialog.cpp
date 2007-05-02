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

#include "ctags_dialog.h"
#include "options_panel.h"

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
	
	
	long bookStyle = wxFNB_NO_NAV_BUTTONS | wxFNB_NO_X_BUTTON | wxFNB_NODRAG | wxFNB_FF2 | wxFNB_BACKGROUND_GRADIENT;

	m_book = new wxFlatNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, bookStyle );
	m_book->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	m_book->AddPage(new OptionsPanel(m_book), wxT("General"), true);
	
	mainSizer->Add( m_book, 1, wxEXPAND | wxALL, 5 );
	
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
	GetSizer()->SetMinSize(500, 300);
	GetSizer()->SetSizeHints(this);
}

void CtagsOptionsDlg::OnButtonClose(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EndModal(wxID_CANCEL);
}

void CtagsOptionsDlg::OnButtonOK(wxCommandEvent &event)
{
	wxUnusedVar(event);
}

void CtagsOptionsDlg::OnClose(wxCloseEvent &event)
{
	wxUnusedVar(event);
	EndModal(wxID_CANCEL);
}

