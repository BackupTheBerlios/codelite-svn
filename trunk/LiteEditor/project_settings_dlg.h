#ifndef __project_settings_dlg__
#define __project_settings_dlg__

/**
@file
Subclass of ProjectSettingsBaseDlg, which is generated by wxFormBuilder.
@todo Add your event handlers directly to this file.
*/

#include "project_settings_base_dlg.h"
#include "project_settings.h"

/** Implementing ProjectSettingsBaseDlg */
class ProjectSettingsDlg : public ProjectSettingsBaseDlg
{
	wxString m_projectName;
	wxString m_configName;
	wxString m_oldConfigurationName;
	
protected:
	void ConnectEvents();
	void InitDialog(const wxString &configName, const wxString &oldConfig);

	/**
	 * Copy values from the project settings configuration file to the 
	 * GUI controls
	 * \param confName configuration name to copy
	 */
	void CopyValues(const wxString &confName);

	/**
	 * Save values from the GUI controls back to 
	 * project settings configuration file
	 * \param confName configuration name to save
	 */
	void SaveValues(const wxString &confName);

	void DisableCompilerPage(bool disable);
	void DisableLinkerPage(bool disable);

	void PopupAddOptionDlg(wxTextCtrl *ctrl);

	void OnEditCommand(wxCheckListBox* list);
	void OnUpCommand(wxCheckListBox* list);
	void OnDownCommand(wxCheckListBox* list);
	void OnNewCommand(wxCheckListBox* list);
	void OnDeleteCommand(wxCheckListBox* list);


public:
	/** Constructor */
	ProjectSettingsDlg( wxWindow* parent, const wxString &configName, const wxString &projectName );

	virtual void OnCheckCompilerNeeded(wxCommandEvent &event);
	virtual void OnCheckLinkerNeeded(wxCommandEvent &event);
	virtual void OnConfigurationTypeSelected(wxCommandEvent &event);
	virtual void OnAddSearchPath(wxCommandEvent &event);
	virtual void OnAddLibraryPath(wxCommandEvent &event);
	virtual void OnAddLibrary(wxCommandEvent &event);

	virtual void OnNewPreBuildCommand(wxCommandEvent & WXUNUSED(event)){ OnNewCommand(m_checkListPreBuildCommands); }
	virtual void OnEditPreBuildCommand(wxCommandEvent &WXUNUSED(event)){ OnEditCommand(m_checkListPreBuildCommands); }
	virtual void OnUpPreBuildCommand(wxCommandEvent &WXUNUSED(event)){ OnUpCommand(m_checkListPreBuildCommands); }
	virtual void OnDownPreBuildCommand(wxCommandEvent &WXUNUSED(event)){ OnDownCommand(m_checkListPreBuildCommands); }
	virtual void OnDeletePreBuildCommand(wxCommandEvent &WXUNUSED(event)){ OnDeleteCommand(m_checkListPreBuildCommands); }

	virtual void OnNewPostBuildCommand(wxCommandEvent & WXUNUSED(event)){ OnNewCommand(m_checkListPostBuildCommands); }
	virtual void OnEditPostBuildCommand(wxCommandEvent &WXUNUSED(event)){ OnEditCommand(m_checkListPostBuildCommands); }
	virtual void OnUpPostBuildCommand(wxCommandEvent &WXUNUSED(event)){ OnUpCommand(m_checkListPostBuildCommands); }
	virtual void OnDownPostBuildCommand(wxCommandEvent &WXUNUSED(event)){ OnDownCommand(m_checkListPostBuildCommands); }
	virtual void OnDeletePostBuildCommand(wxCommandEvent &WXUNUSED(event)){ OnDeleteCommand(m_checkListPostBuildCommands); }
	virtual void OnButtonOK(wxCommandEvent &event);
	virtual void OnButtonApply(wxCommandEvent &event);
	virtual void OnButtonConfigurationManager(wxCommandEvent &event);

};

#endif // __project_settings_dlg__

