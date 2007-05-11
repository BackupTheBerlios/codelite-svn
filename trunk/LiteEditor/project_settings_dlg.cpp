#include "project_settings_dlg.h"
#include "add_option_dialog.h"
#include "free_text_dialog.h"

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
	}else{
		m_compilerNameChoice->Enable();
		m_textAdditionalSearchPath->Enable();
		m_buttonAddSearchPath->Enable();
		m_textCompilerOptions->Enable();
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
	}else{
		m_textLibraryPath->Enable();
		m_textLibraries->Enable();
		m_textLinkerOptions->Enable();
		m_buttonLibraries->Enable();
		m_buttonLibraryPath->Enable();
	}
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
