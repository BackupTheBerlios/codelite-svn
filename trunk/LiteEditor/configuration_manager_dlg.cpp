#include "configuration_manager_dlg.h"
#include "manager.h"
#include "new_configuration_dlg.h"
#include "edit_configuration.h"
#include "macros.h"

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
	choiceConfig->Append(clCMD_NEW);
	choiceConfig->Append(clCMD_EDIT);
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
	entry.choiceControl = choiceConfig;

	m_projSettingsMap[choiceConfig->GetId()] = entry;
}

void ConfigurationManagerDlg::PopulateConfigurations()
{
	//popuplate the configurations
	BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
	if(!matrix){
		return;
	}

	wxFlexGridSizer *mainSizer = dynamic_cast<wxFlexGridSizer*>(m_scrolledWindow->GetSizer());
	if(!mainSizer) return;

	Freeze();
	// remove old entries from the configuration table
	wxSizerItemList list = mainSizer->GetChildren();
	for ( wxSizerItemList::Node *node = list.GetFirst(); node; node = node->GetNext() ){
		wxSizerItem *current = node->GetData();
		current->GetWindow()->Destroy();
	}
	
	std::list<ConfigurationPtr> configs = matrix->GetConfigurations();
	std::list<ConfigurationPtr>::iterator iter = configs.begin();
	
	m_choiceConfigurations->Clear();
	for(; iter != configs.end(); iter++){
		m_choiceConfigurations->Append((*iter)->GetName());
	}

	int sel = m_choiceConfigurations->FindString(matrix->GetSelectedConfigurationName());
	if(sel != wxNOT_FOUND){
		m_choiceConfigurations->SetSelection(sel);
	}else if(m_choiceConfigurations->GetCount() > 0){
		m_choiceConfigurations->SetSelection(0);
	}

	wxArrayString projects;
	ManagerST::Get()->GetProjectList(projects);
	
	for(size_t i=0; i<projects.GetCount(); i++){
		wxString selConf = matrix->GetProjectSelectedConf(matrix->GetSelectedConfigurationName(),  projects.Item(i));
		AddEntry(projects.Item(i), m_choiceConfigurations->GetStringSelection());
	}

	mainSizer->Fit(m_scrolledWindow);
	Layout();
	Thaw();
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
		
		// repopulate the configurations
		PopulateConfigurations();
	}else if(selection == wxT("<Edit...>")){
		EditConfigurationDialog *dlg = new EditConfigurationDialog(this, iter->second.project);
		dlg->ShowModal();
		dlg->Destroy();
		// repopulate the configurations
		PopulateConfigurations();
	}else{
		// do nothing
	}
}

void ConfigurationManagerDlg::OnButtonNew(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxTextEntryDialog *dlg = new wxTextEntryDialog(this, wxT("Enter New Configuration Name:"), wxT("New Configuration"));
	if(dlg->ShowModal() == wxID_OK){
		wxString value = dlg->GetValue();
		TrimString(value);
		if(value.IsEmpty() == false){
			BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
			if(!matrix){
				return;
			}

			ConfigurationPtr conf(new Configuration(NULL));
			conf->SetName(value);
			conf->SetConfigMappingList(GetCurrentSettings());
			matrix->SetConfiguration(conf);
			//save changes
			ManagerST::Get()->SetWorkspaceBuildMatrix(matrix);
		}
	}
	dlg->Destroy();
}

Configuration::ConfigMappingList ConfigurationManagerDlg::GetCurrentSettings()
{
	//return the current settings as described by the dialog
	Configuration::ConfigMappingList list;
	
	std::map<int, ConfigEntry>::iterator iter = m_projSettingsMap.begin();
	for(; iter != m_projSettingsMap.end(); iter++){

		wxString value = iter->second.choiceControl->GetStringSelection();
		if(value != clCMD_NEW && value != clCMD_EDIT){
			ConfigMappingEntry entry(iter->second.project, value);
			list.push_back(entry);
		}
	}
	return list;
}

void ConfigurationManagerDlg::OnButtonOK(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EndModal(wxID_OK);
}

