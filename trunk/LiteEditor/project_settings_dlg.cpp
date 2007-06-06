#include "project_settings_dlg.h"
#include "add_option_dialog.h"
#include "free_text_dialog.h"
#include "manager.h"
#include "configuration_manager_dlg.h"
#include "macros.h"
#include "editor_config.h"
#include "build_settings_config.h"

ProjectSettingsDlg::ProjectSettingsDlg( wxWindow* parent, const wxString &configName, const wxString &projectName, const wxString &title )
: ProjectSettingsBaseDlg( parent, wxID_ANY, title )
, m_projectName(projectName)
, m_configName(configName)
, m_oldConfigurationName(wxEmptyString)
{
	ConnectEvents();
	m_notebook3->SetSelection(0);
	//fill the dialog with values
	InitDialog(m_configName, wxEmptyString);
	m_oldConfigurationName = m_choiceConfigurationType->GetStringSelection();
}

void ProjectSettingsDlg::UpdateConfigurationTypeChoice(const wxString &itemToSelect)
{
	ProjectSettingsPtr projSettingsPtr = ManagerST::Get()->GetProjectSettings(m_projectName);
	ProjectSettingsCookie cookie;
	m_choiceConfigurationType->Clear();
	BuildConfigPtr conf = projSettingsPtr->GetFirstBuildConfiguration(cookie);
	while(conf){
		m_choiceConfigurationType->Append(conf->GetName());
		conf = projSettingsPtr->GetNextBuildConfiguration(cookie);
	}

	if(itemToSelect.IsEmpty() || m_choiceConfigurationType->FindString(itemToSelect) == wxNOT_FOUND){
		if(m_choiceConfigurationType->GetCount() > 0)
			m_choiceConfigurationType->SetSelection(0);
	}else{
		m_choiceConfigurationType->SetStringSelection(itemToSelect);
	}
}

void ProjectSettingsDlg::InitDialog(const wxString &configName, const wxString &oldConfig)
{
	wxUnusedVar(configName);
	ProjectSettingsPtr projSettingsPtr = ManagerST::Get()->GetProjectSettings(m_projectName);
	
	UpdateConfigurationTypeChoice(configName);
	if(oldConfig.IsEmpty() == false){
		// save old values before replacing them
		SaveValues(oldConfig);
	}
	CopyValues(m_choiceConfigurationType->GetStringSelection());
}

void ProjectSettingsDlg::ClearValues()
{
	BuildCommandList preBuildCmds, postBuildCmds;

	m_outputFilePicker->SetPath(wxEmptyString);
	m_intermediateDirPicker->SetPath(wxEmptyString);
	m_textCommand->SetValue(wxEmptyString);
	m_textCommandArguments->SetValue(wxEmptyString);
	m_workingDirPicker->SetPath(wxEmptyString);
	m_checkCompilerNeeded->SetValue(false);
	m_textCompilerOptions->SetValue(wxEmptyString);
	DisableCompilerPage(false);
	m_textAdditionalSearchPath->SetValue(wxEmptyString);
	m_checkLinkerNeeded->SetValue(false);
	DisableLinkerPage(m_checkLinkerNeeded->IsChecked());
	m_textLinkerOptions->SetValue(wxEmptyString);
	m_textLibraries->SetValue(wxEmptyString);
	m_textLibraryPath->SetValue(wxEmptyString);
	m_checkListPreBuildCommands->Clear();
	m_checkListPostBuildCommands->Clear();
}

void ProjectSettingsDlg::CopyValues(const wxString &confName)
{
	BuildConfigPtr buildConf;
	ProjectSettingsPtr projSettingsPtr = ManagerST::Get()->GetProjectSettings(m_projectName);
	buildConf =	projSettingsPtr->GetBuildConfiguration(confName);
	if(!buildConf){
		ClearValues();
		return;
	}

	wxArrayString searchArr, libPath, libs;
	BuildCommandList preBuildCmds, postBuildCmds;

	m_outputFilePicker->SetPath(buildConf->GetOutputFileName());
	m_intermediateDirPicker->SetPath(buildConf->GetIntermediateDirectory());
	m_textCommand->SetValue(buildConf->GetCommand());
	m_textCommandArguments->SetValue(buildConf->GetCommandArguments());
	m_workingDirPicker->SetPath(buildConf->GetWorkingDirectory());
	m_checkCompilerNeeded->SetValue(!buildConf->IsCompilerRequired());
	m_textCompilerOptions->SetValue(buildConf->GetCompileOptions());
	DisableCompilerPage(m_checkCompilerNeeded->IsChecked());
	m_textAdditionalSearchPath->SetValue(buildConf->GetIncludePath());
	m_checkLinkerNeeded->SetValue(!buildConf->IsLinkerRequired());
	DisableLinkerPage(m_checkLinkerNeeded->IsChecked());
	m_textLinkerOptions->SetValue(buildConf->GetLinkOptions());
	m_textLibraries->SetValue(buildConf->GetLibraries());
	m_textLibraryPath->SetValue(buildConf->GetLibPath());
	m_textPreprocessor->SetValue(buildConf->GetPreprocessor());
	buildConf->GetPreBuildCommands(preBuildCmds);
	buildConf->GetPostBuildCommands(postBuildCmds);
	BuildCommandList::iterator iter = preBuildCmds.begin();

	m_checkListPreBuildCommands->Clear();
	for(; iter != preBuildCmds.end(); iter ++){
		int index = m_checkListPreBuildCommands->Append(iter->GetCommand());
		m_checkListPreBuildCommands->Check(index, iter->GetEnabled());
	}

	iter = postBuildCmds.begin();
	m_checkListPostBuildCommands->Clear();
	for(; iter != postBuildCmds.end(); iter ++){
		int index = m_checkListPostBuildCommands->Append(iter->GetCommand());
		m_checkListPostBuildCommands->Check(index, iter->GetEnabled());
	}

	//set the project type
	wxString projType = projSettingsPtr->GetProjectType();
	int sel = m_choiceProjectTypes->FindString(projType);
	if(sel == wxNOT_FOUND){
		sel = 0;
	}
	m_choiceProjectTypes->SetSelection(sel);

	m_choiceCompilerType->Clear();
	wxString cmpType = buildConf->GetCompilerType();
	BuildSettingsConfigCookie cookie;
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
	while(cmp){
		m_choiceCompilerType->Append(cmp->GetName());
		cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
	}

	int where = m_choiceCompilerType->FindString(cmpType);
	if(where == wxNOT_FOUND){
		if(m_choiceCompilerType->GetCount() > 0){
			m_choiceCompilerType->SetSelection(0);
		}
	}else{
		m_choiceCompilerType->SetSelection(where);
	}
}

void ProjectSettingsDlg::SaveValues(const wxString &confName)
{
	BuildConfigPtr buildConf;
	ProjectSettingsPtr projSettingsPtr = ManagerST::Get()->GetProjectSettings(m_projectName);
	buildConf =	projSettingsPtr->GetBuildConfiguration(confName);
	if(!buildConf){
		return;
	}
	wxArrayString searchArr, libPath, libs;
	BuildCommandList preBuildCmds, postBuildCmds;

	buildConf->SetOutputFileName(m_outputFilePicker->GetPath());
	buildConf->SetIntermediateDirectory(m_intermediateDirPicker->GetPath());
	buildConf->SetCommand(m_textCommand->GetValue());
	buildConf->SetCommandArguments(m_textCommandArguments->GetValue());
	buildConf->SetWorkingDirectory(m_workingDirPicker->GetPath());
	buildConf->SetCompilerRequired(!m_checkCompilerNeeded->IsChecked());
	buildConf->SetCompileOptions(m_textCompilerOptions->GetValue());
	buildConf->SetIncludePath(m_textAdditionalSearchPath->GetValue());
	buildConf->SetLibPath(m_textLibraryPath->GetValue());
	buildConf->SetLibraries(m_textLibraries->GetValue());
	buildConf->SetLinkerRequired(!m_checkLinkerNeeded->IsChecked());
	buildConf->SetLinkOptions(m_textLinkerOptions->GetValue());
	projSettingsPtr->SetProjectType(m_choiceProjectTypes->GetStringSelection());
	buildConf->SetCompilerType(m_choiceCompilerType->GetStringSelection());
	buildConf->SetPreprocessor(m_textPreprocessor->GetValue());

	BuildCommandList cmds;
	cmds.clear();
	for(size_t i=0; i<m_checkListPreBuildCommands->GetCount(); i++){
		wxString cmdLine = m_checkListPreBuildCommands->GetString((unsigned int)i);
		bool enabled = m_checkListPreBuildCommands->IsChecked((unsigned int)i);
		BuildCommand cmd(cmdLine, enabled);
		cmds.push_back(cmd);
	}
	buildConf->SetPreBuildCommands(cmds);

	cmds.clear();
	for(size_t i=0; i<m_checkListPostBuildCommands->GetCount(); i++){
		wxString cmdLine = m_checkListPostBuildCommands->GetString((unsigned int)i);
		bool enabled = m_checkListPostBuildCommands->IsChecked((unsigned int)i);
		BuildCommand cmd(cmdLine, enabled);
		cmds.push_back(cmd);
	}
	buildConf->SetPostBuildCommands(cmds);

	//save settings
	ManagerST::Get()->SetProjectSettings(m_projectName, projSettingsPtr);
}

void ProjectSettingsDlg::ConnectEvents()
{
	ConnectChoice(m_choiceConfigurationType, ProjectSettingsDlg::OnConfigurationTypeSelected);
	ConnectCheckBox(m_checkCompilerNeeded, ProjectSettingsDlg::OnCheckCompilerNeeded);
	ConnectCheckBox(m_checkLinkerNeeded, ProjectSettingsDlg::OnCheckLinkerNeeded);
	ConnectButton(m_buttonAddSearchPath, ProjectSettingsDlg::OnAddSearchPath);
	ConnectButton(m_buttonLibraries, ProjectSettingsDlg::OnAddLibrary);
	ConnectButton(m_buttonLibraryPath, ProjectSettingsDlg::OnAddLibraryPath);
	ConnectButton(m_buttonNewPreBuildCmd, ProjectSettingsDlg::OnNewPreBuildCommand);
	ConnectButton(m_buttonEditPreBuildCmd, ProjectSettingsDlg::OnEditPreBuildCommand);
	ConnectButton(m_buttonUpPreBuildCmd, ProjectSettingsDlg::OnUpPreBuildCommand);
	ConnectButton(m_buttonDownPreBuildCmd, ProjectSettingsDlg::OnDownPreBuildCommand);
	ConnectButton(m_buttonDeletePreBuildCmd, ProjectSettingsDlg::OnDeletePreBuildCommand);
	ConnectButton(m_buttonNewPostBuildCmd, ProjectSettingsDlg::OnNewPostBuildCommand);
	ConnectButton(m_buttonEditPostBuildCmd, ProjectSettingsDlg::OnEditPostBuildCommand);
	ConnectButton(m_buttonUpPostBuildCmd, ProjectSettingsDlg::OnUpPostBuildCommand);
	ConnectButton(m_buttonDownPostBuildCmd, ProjectSettingsDlg::OnDownPostBuildCommand);
	ConnectButton(m_buttonDeletePostBuildCmd, ProjectSettingsDlg::OnDeletePostBuildCommand);
	ConnectButton(m_buttonOK, ProjectSettingsDlg::OnButtonOK);
	ConnectButton(m_buttonApply, ProjectSettingsDlg::OnButtonApply);
	ConnectButton(m_buttonConfigManager, ProjectSettingsDlg::OnButtonConfigurationManager);
	ConnectButton(m_buttonAddPreprocessor, ProjectSettingsDlg::OnButtonAddPreprocessor);
}

void ProjectSettingsDlg::OnButtonAddPreprocessor(wxCommandEvent &event)
{
	wxUnusedVar(event);
	PopupAddOptionDlg(m_textPreprocessor);
}

void ProjectSettingsDlg::OnButtonOK(wxCommandEvent &event)
{
	wxUnusedVar(event);
	SaveValues(m_choiceConfigurationType->GetStringSelection());
	EndModal(wxID_OK);
}

void ProjectSettingsDlg::OnButtonApply(wxCommandEvent &event)
{
	wxUnusedVar(event);
	SaveValues(m_choiceConfigurationType->GetStringSelection());
}

void ProjectSettingsDlg::OnButtonConfigurationManager(wxCommandEvent &event)
{
	wxUnusedVar(event);
	ConfigurationManagerDlg *dlg = new ConfigurationManagerDlg(this);
	dlg->ShowModal();
	dlg->Destroy();

	//update the configuration type choice control
	UpdateConfigurationTypeChoice(m_choiceConfigurationType->GetStringSelection());
	//Load values according to the new selection
	CopyValues(m_choiceConfigurationType->GetStringSelection());
}

void ProjectSettingsDlg::OnConfigurationTypeSelected(wxCommandEvent &event)
{
	wxString selection = event.GetString();
	InitDialog(selection, m_oldConfigurationName);
	m_oldConfigurationName = selection;
}

void ProjectSettingsDlg::DisableCompilerPage(bool disable)
{
	m_textAdditionalSearchPath->Enable(!disable);
	m_buttonAddSearchPath->Enable(!disable);
	m_textCompilerOptions->Enable(!disable);
}

void ProjectSettingsDlg::DisableLinkerPage(bool disable)
{
	m_textLibraryPath->Enable(!disable);
	m_textLibraries->Enable(!disable);
	m_textLinkerOptions->Enable(!disable);
	m_buttonLibraries->Enable(!disable);
	m_buttonLibraryPath->Enable(!disable);
}

void ProjectSettingsDlg::OnCheckCompilerNeeded(wxCommandEvent &event)
{
	DisableCompilerPage(event.IsChecked());
}

void ProjectSettingsDlg::OnCheckLinkerNeeded(wxCommandEvent &event)
{
	DisableLinkerPage(event.IsChecked());
}

void ProjectSettingsDlg::PopupAddOptionDlg(wxTextCtrl *ctrl)
{
	AddOptionDlg *dlg = new AddOptionDlg(NULL, ctrl->GetValue());
	if(dlg->ShowModal() == wxID_OK){
		wxString updatedValue = dlg->GetValue();
		ctrl->SetValue(updatedValue);
	}
	dlg->Destroy();
}

void ProjectSettingsDlg::OnAddSearchPath(wxCommandEvent &event)
{
	wxUnusedVar(event);
	PopupAddOptionDlg(m_textAdditionalSearchPath);
}

void ProjectSettingsDlg::OnAddLibrary(wxCommandEvent &event)
{
	wxUnusedVar(event);
	PopupAddOptionDlg(m_textLibraries);
}

void ProjectSettingsDlg::OnAddLibraryPath(wxCommandEvent &event)
{
	wxUnusedVar(event);
	PopupAddOptionDlg(m_textLibraryPath);
}

void ProjectSettingsDlg::OnNewCommand(wxCheckListBox *list)
{
	FreeTextDialog *dlg = new FreeTextDialog(this);
	if(dlg->ShowModal() == wxID_OK){
		wxString value = dlg->GetValue();
		TrimString(value);
		if(value.IsEmpty() == false){
			list->Append(value);
			list->Check(list->GetCount()-1);
		}
	}
	dlg->Destroy();
}

void ProjectSettingsDlg::OnEditCommand(wxCheckListBox *list)
{
	wxString selectedString  = list->GetStringSelection();
	int sel = list->GetSelection();
	if(sel == wxNOT_FOUND){
		return;
	}

	FreeTextDialog *dlg = new FreeTextDialog(this, selectedString);
	if(dlg->ShowModal() == wxID_OK){
		wxString value = dlg->GetValue();
		TrimString(value);
		if(value.IsEmpty() == false){
			list->SetString(sel, value);
		}
	}
	dlg->Destroy();
}

void ProjectSettingsDlg::OnUpCommand(wxCheckListBox *list)
{
	wxString selectedString  = list->GetStringSelection();
	
	int sel = list->GetSelection();
	if(sel == wxNOT_FOUND){
		return;
	}

	bool isSelected = list->IsChecked(sel);
	sel --;
	if(sel < 0){
		return;
	}

	// sel contains the new position we want to place the selection string
	list->Delete(sel + 1);
	list->Insert(selectedString, sel);
	list->Select(sel);
	list->Check(sel, isSelected);
}

void ProjectSettingsDlg::OnDownCommand(wxCheckListBox *list)
{
	int sel = list->GetSelection();
	if(sel == wxNOT_FOUND){
		return;
	}

	sel ++;
	if(sel >= (int)list->GetCount()){
		return;
	}

	// sel contains the new position we want to place the selection string
	wxString oldStr = list->GetString(sel);
	bool oldStringIsSelected = list->IsChecked(sel);

	list->Delete(sel);
	list->Insert(oldStr, sel - 1);
	list->Select(sel);
	list->Check(sel - 1, oldStringIsSelected);
}

void ProjectSettingsDlg::OnDeleteCommand(wxCheckListBox *list)
{
	int sel = list->GetSelection();
	if(sel == wxNOT_FOUND){
		return;
	}
	list->Delete(sel);
	if(sel < (int)list->GetCount()){
		list->Select(sel);
	} else if(sel - 1 < (int)list->GetCount()){
		list->Select(sel -1);
	}
}
