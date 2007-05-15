#include "configuration_manager_dlg.h"
#include "manager.h"
#include "new_configuration_dlg.h"


#define ConnectChoice(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(fn), NULL, this);

#define ConnectButton(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(fn), NULL, this);

//----------------------------------------------------------------------------
// Configuration Manager
//----------------------------------------------------------------------------


ConfigurationManagerDlg::ConfigurationManagerDlg( wxWindow* parent )
:
ConfigManagerBaseDlg( parent )
{
	PopulateConfigurations();
	InitDialog();
}


void ConfigurationManagerDlg::AddEntry(const wxString &projectName, const wxString &selectedConf)
{
	wxFlexGridSizer *mainSizer = dynamic_cast<wxFlexGridSizer*>(m_scrolledWindow->GetSizer());
	if(!mainSizer) return;

	wxArrayString choices;
	wxChoice *choiceConfig = new wxChoice( m_scrolledWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices, 0 );

	// Get all configuration of the project
	ProjectSettingsPtr settings = ManagerST::Get()->GetProjectSettings(projectName);
	if(settings){
		ProjectSettingsCookie cookie;
		BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
		while(bldConf){
			choiceConfig->Append(bldConf->GetName());
			bldConf = settings->GetNextBuildConfiguration(cookie);
		}
	}
	choiceConfig->Append(wxT("<New...>"));
	choiceConfig->Append(wxT("<Edit...>"));
	ConnectChoice(choiceConfig, ConfigurationManagerDlg::OnConfigSelected);
	wxStaticText *text = new wxStaticText( m_scrolledWindow, wxID_ANY, projectName, wxDefaultPosition, wxDefaultSize, 0 );

	int where = choiceConfig->FindString(selectedConf);
	if(where == wxNOT_FOUND){
		where = 0;
	}
	choiceConfig->SetSelection(where);
	mainSizer->Add(text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	mainSizer->Add(choiceConfig, 1, wxEXPAND | wxALL | wxALIGN_CENTER_VERTICAL, 5);
	
	ConfigEntry entry;
	entry.project = projectName;
	entry.projectSettings = settings;
	m_projSettingsMap[choiceConfig->GetId()] = entry;
}

void ConfigurationManagerDlg::PopulateConfigurations()
{
	wxArrayString projects;
	ManagerST::Get()->GetProjectList(projects);

	for(size_t i=0; i<projects.GetCount(); i++){
		AddEntry(projects.Item(i), wxT("Debug"));
	}

	wxFlexGridSizer *mainSizer = dynamic_cast<wxFlexGridSizer*>(m_scrolledWindow->GetSizer());
	if(!mainSizer) return;
	mainSizer->Fit(m_scrolledWindow);
}

void ConfigurationManagerDlg::InitDialog()
{
	// Connect events
	ConnectButton(m_buttonOK, ConfigurationManagerDlg::OnButtonOK);
	ConnectButton(m_buttonNewConfiguration, ConfigurationManagerDlg::OnButtonNew);
}

void ConfigurationManagerDlg::OnConfigSelected(wxCommandEvent &event)
{
	std::map<int, ConfigEntry>::iterator iter = m_projSettingsMap.find(event.GetId());
	if(iter == m_projSettingsMap.end())
		return;

	wxString selection = event.GetString();
	if(selection == wxT("<New...>")){
		// popup the 'New Configuration' dialog
		NewConfigurationDlg *dlg = new NewConfigurationDlg(this, iter->second.project);
		dlg->ShowModal();
		dlg->Destroy();
	}else if(selection == wxT("<Edit...>")){
		// popup the 'New Configuration' dialog
	}else{
		// do nothing
	}
}

void ConfigurationManagerDlg::OnButtonNew(wxCommandEvent &event)
{
	wxUnusedVar(event);
}

void ConfigurationManagerDlg::OnButtonOK(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EndModal(wxID_OK);
}

