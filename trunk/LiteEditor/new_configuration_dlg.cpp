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

#include "new_configuration_dlg.h"
#include "manager.h"
#include "project_settings.h"

#define ConnectChoice(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(fn), NULL, this);

#define ConnectButton(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(fn), NULL, this);

///////////////////////////////////////////////////////////////////////////

NewConfigurationDlg::NewConfigurationDlg( wxWindow* parent, const wxString &projectName, int id, wxString title, wxPoint pos, wxSize size, int style ) 
: wxDialog( parent, id, title, pos, size, style )
, m_projectName(projectName)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( m_panel1, wxID_ANY, wxT("Configuration Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_staticText1, 0, wxALL, 5 );
	
	m_textConfigurationName = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_textConfigurationName, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText2 = new wxStaticText( m_panel1, wxID_ANY, wxT("Copy Settings from:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_staticText2, 0, wxALL, 5 );
	
	wxArrayString m_choiceCopyConfigurationsChoices;
	m_choiceCopyConfigurations = new wxChoice( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceCopyConfigurationsChoices, 0 );

	// Get all configuration of the project
	m_choiceCopyConfigurations->Append(wxT("-- None --"));
	ProjectSettingsPtr settings = ManagerST::Get()->GetProjectSettings(m_projectName);
	if(settings){
		ProjectSettingsCookie cookie;
		BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
		while(bldConf){
			m_choiceCopyConfigurations->Append(bldConf->GetName());
			bldConf = settings->GetNextBuildConfiguration(cookie);
		}
	}
	m_choiceCopyConfigurations->SetSelection(0);
	bSizer3->Add( m_choiceCopyConfigurations, 0, wxALL|wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer3 );
	m_panel1->Layout();
	bSizer3->Fit( m_panel1 );
	bSizer1->Add( m_panel1, 1, wxEXPAND | wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_ANY, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer2, 0, wxALIGN_RIGHT, 5 );
	
	ConnectButton(m_buttonOK, NewConfigurationDlg::OnButtonOK);

	this->SetSizer( bSizer1 );
	this->Layout();
}

void NewConfigurationDlg::OnButtonOK(wxCommandEvent &event)
{
	wxUnusedVar(event);

	wxString newConfName;
	newConfName = m_textConfigurationName->GetValue();
	newConfName = newConfName.Trim();
	newConfName = newConfName.Trim(false);
	if(newConfName.IsEmpty()){
		wxMessageBox (wxT("Configuration Name is empty"), wxT("Lite Editor"), wxOK | wxICON_INFORMATION);
		return;
	}

	ProjectSettingsPtr settings = ManagerST::Get()->GetProjectSettings(m_projectName);
	wxString copyFrom = m_choiceCopyConfigurations->GetStringSelection();
	BuildConfigPtr newBuildConf;

	if(copyFrom == wxT("-- None --")){
		newBuildConf = new BuildConfig(NULL);
	}else{
		BuildConfigPtr origBuildConf = settings->GetBuildConfiguration(copyFrom);
		newBuildConf = origBuildConf->Clone();
	}
	
	newBuildConf->SetName(newConfName);
	settings->SetBuildConfiguration(newBuildConf);
	//save the settings
	ManagerST::Get()->SetProjectSettings(m_projectName, settings);
	EndModal(wxID_OK);
}
