#include "project_settings_dlg.h"

// help macros
#define ConnectCheckBox(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(fn), NULL, this);

#define ConnectChoice(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(fn), NULL, this);

#define ConnectButton(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(fn), NULL, this);


ProjectSettingsDlg::ProjectSettingsDlg( wxWindow* parent, const wxString &configName, const wxString &projectName )
: ProjectSettingsBaseDlg( parent )
, m_projectName(projectName)
, m_configName(configName)
{
	ConnectEvents();
	m_notebook3->SetSelection(0);
	//fill the dialog with values
	InitDialog(m_configName);
}

void ProjectSettingsDlg::InitDialog(const wxString &configName)
{
	wxUnusedVar(configName);
}

void ProjectSettingsDlg::ConnectEvents()
{
	ConnectChoice(m_choiceConfigurationType, ProjectSettingsDlg::OnConfigurationTypeSelected);
	ConnectCheckBox(m_checkCompilerNeeded, ProjectSettingsDlg::OnCheckCompilerNeeded);
	ConnectCheckBox(m_checkLinkerNeeded, ProjectSettingsDlg::OnCheckLinkerNeeded);
}

void ProjectSettingsDlg::OnConfigurationTypeSelected(wxCommandEvent &event)
{
	wxString selection = event.GetString();
	InitDialog(selection);
}

void ProjectSettingsDlg::OnCheckCompilerNeeded(wxCommandEvent &event)
{
	if(event.IsChecked()){
		m_compilerNameChoice->Disable();
		m_textAdditionalSearchPath->Disable();
		m_buttonAddSearchPath->Disable();
		m_textCompilerOptions->Disable();
		m_buttonCompilerOptions->Disable();
	}else{
		m_compilerNameChoice->Enable();
		m_textAdditionalSearchPath->Enable();
		m_buttonAddSearchPath->Enable();
		m_textCompilerOptions->Enable();
		m_buttonCompilerOptions->Enable();
	}
}

void ProjectSettingsDlg::OnCheckLinkerNeeded(wxCommandEvent &event)
{
	if(event.IsChecked()){
		m_textLibraryPath->Disable();
		m_textLibraries->Disable();
		m_textLinkerOptions->Disable();
		m_buttonLibraries->Disable();
		m_buttonLibraryPath->Disable();
		m_buttonLinkerOptions->Disable();
	}else{
		m_textLibraryPath->Enable();
		m_textLibraries->Enable();
		m_textLinkerOptions->Enable();
		m_buttonLibraries->Enable();
		m_buttonLibraryPath->Enable();
		m_buttonLinkerOptions->Enable();
	}
}
