#include "project_settings_dlg.h"
#include "add_option_dialog.h"
#include "free_text_dialog.h"
#include "manager.h"

// help macros
#define ConnectCheckBox(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(fn), NULL, this);

#define ConnectChoice(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(fn), NULL, this);

#define ConnectButton(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(fn), NULL, this);

#define Trim(str)\
	{\
		str = str.Trim();\
		str = str.Trim(false);\
	}

// Utils function
static wxString ArrayToSmiColonString(wxArrayString &array){
	wxString result;
	for(size_t i=0; i<array.GetCount(); i++){
		result += array.Item(i);
		result += wxT(";");
	}
	return result;
}

ProjectSettingsDlg::ProjectSettingsDlg( wxWindow* parent, const wxString &configName, const wxString &projectName )
: ProjectSettingsBaseDlg( parent )
, m_projectName(projectName)
, m_configName(configName)
, m_projSettingsPtr(NULL)
{
	ConnectEvents();
	m_notebook3->SetSelection(0);
	//fill the dialog with values
	InitDialog(m_configName);
}

void ProjectSettingsDlg::InitDialog(const wxString &configName)
{
	wxUnusedVar(configName);
	m_projSettingsPtr = ManagerST::Get()->GetProjectSettings(m_projectName);
	ManagerST::Get()->SetProjectSettings(m_projectName, m_projSettingsPtr);

	ProjectSettingsCookie cookie;
	BuildConfigPtr conf = m_projSettingsPtr->GetFirstBuildConfiguration(cookie);
	while(conf){
		m_choiceConfigurationType->Append(conf->GetName());
		conf = m_projSettingsPtr->GetNextBuildConfiguration(cookie);
	}

	if(configName.IsEmpty() || m_choiceConfigurationType->FindString(configName) == wxNOT_FOUND){
		m_choiceConfigurationType->SetSelection(0);
	}else{
		m_choiceConfigurationType->SetStringSelection(configName);
	}
	
	if(configName.IsEmpty() == false){
		// save old values before replacing them
		SaveValues(m_choiceConfigurationType->GetStringSelection());
	}
	CopyValues(m_choiceConfigurationType->GetStringSelection());
}

void ProjectSettingsDlg::CopyValues(const wxString &confName)
{
	BuildConfigPtr buildConf;
	buildConf =	m_projSettingsPtr->GetBuildConfiguration(confName);
	if(!buildConf){
		return;
	}

	wxArrayString searchArr, libPath, libs;
	BuildCommandList preBuildCmds, postBuildCmds;

	m_outputFilePicker->SetPath(buildConf->GetOutputFileName());
	m_intermediateDirPicker->SetPath(buildConf->GetIntermediateDirectory());
	m_textCommand->SetValue(buildConf->GetCommand());
	m_textCommandArguments->SetValue(buildConf->GetCommandArguments());
	m_textCommandArguments->SetValue(buildConf->GetCommandArguments());
	m_workingDirPicker->SetPath(buildConf->GetWorkingDirectory());
	m_checkCompilerNeeded->SetValue(!buildConf->IsCompilerRequired());
	m_textCompilerName->SetValue(buildConf->GetCompilerName());
	m_textCompilerOptions->SetValue(buildConf->GetCompileOptions());

	
	buildConf->GetIncludePath(searchArr);
	buildConf->GetLibPath(libPath);
	buildConf->GetLibraries(libs);
	buildConf->GetPreBuildCommands(preBuildCmds);
	buildConf->GetPostBuildCommands(postBuildCmds);

	DisableCompilerPage(m_checkCompilerNeeded->IsChecked());
	m_textAdditionalSearchPath->SetValue(ArrayToSmiColonString(searchArr));
	m_checkLinkerNeeded->SetValue(!buildConf->IsLinkerRequired());
	DisableLinkerPage(m_checkLinkerNeeded->IsChecked());
	m_textLinkerOptions->SetValue(buildConf->GetLinkOptions());
	m_textLibraries->SetValue(ArrayToSmiColonString(libs));
	m_textLibraryPath->SetValue(ArrayToSmiColonString(libPath));

	BuildCommandList::iterator iter = preBuildCmds.begin();
	for(; iter != preBuildCmds.end(); iter ++){
		int index = m_checkListPreBuildCommands->Append(iter->GetCommand());
		m_checkListPreBuildCommands->Check(index, iter->GetEnabled());
	}

	iter = postBuildCmds.begin();
	for(; iter != postBuildCmds.end(); iter ++){
		int index = m_checkListPostBuildCommands->Append(iter->GetCommand());
		m_checkListPostBuildCommands->Check(index, iter->GetEnabled());
	}
}

void ProjectSettingsDlg::SaveValues(const wxString &confName)
{
	BuildConfigPtr buildConf;
	buildConf =	m_projSettingsPtr->GetBuildConfiguration(confName);
	if(!buildConf){
		return;
	}
	wxArrayString searchArr, libPath, libs;
	BuildCommandList preBuildCmds, postBuildCmds;

	buildConf->SetOutputFileName(m_outputFilePicker->GetPath());
	buildConf->SetIntermediateDirectory(m_intermediateDirPicker->GetPath());

	//save settings
	ManagerST::Get()->SetProjectSettings(m_projectName, m_projSettingsPtr);
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
}

void ProjectSettingsDlg::OnButtonOK(wxCommandEvent &event)
{
	wxUnusedVar(event);
	SaveValues(m_choiceConfigurationType->GetStringSelection());
	EndModal(wxID_OK);
}

void ProjectSettingsDlg::OnConfigurationTypeSelected(wxCommandEvent &event)
{
	wxString selection = event.GetString();
	InitDialog(selection);
}

void ProjectSettingsDlg::DisableCompilerPage(bool disable)
{
	m_textCompilerName->Enable(!disable);
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
	AddOptionDlg *dlg = new AddOptionDlg(this, ctrl->GetValue());
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
		Trim(value);
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
		Trim(value);
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
