#ifndef __project_settings_dlg__
#define __project_settings_dlg__

/**
@file
Subclass of ProjectSettingsBaseDlg, which is generated by wxFormBuilder.
@todo Add your event handlers directly to this file.
*/

#include "project_settings_base_dlg.h"

/** Implementing ProjectSettingsBaseDlg */
class ProjectSettingsDlg : public ProjectSettingsBaseDlg
{
	wxString m_projectName;
	wxString m_configName;

protected:
	void ConnectEvents();
	void InitDialog(const wxString &configName);

public:
	/** Constructor */
	ProjectSettingsDlg( wxWindow* parent, const wxString &configName, const wxString &projectName );

	virtual void OnCheckCompilerNeeded(wxCommandEvent &event);
	virtual void OnCheckLinkerNeeded(wxCommandEvent &event);
	virtual void OnConfigurationTypeSelected(wxCommandEvent &event);
	virtual void OnAddSearchPath(wxCommandEvent &event);
};

#endif // __project_settings_dlg__
