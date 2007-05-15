#ifndef __configuration_manager_dlg__
#define __configuration_manager_dlg__

/**
@file
Subclass of ConfigManagerBaseDlg, which is generated by wxFormBuilder.
@todo Add your event handlers directly to this file.
*/

#include "configuration_manager_base_dlg.h"
#include "smart_ptr.h"
#include <map>
#include "project_settings.h"

class ConfigEntry {
public:
	ProjectSettingsPtr projectSettings;
	wxString project;
};

/** Implementing ConfigManagerBaseDlg */
class ConfigurationManagerDlg : public ConfigManagerBaseDlg
{
	std::map<int, ConfigEntry> m_projSettingsMap;

protected:
	void InitDialog();
	void OnButtonNew(wxCommandEvent &event);
	void OnButtonOK(wxCommandEvent &event);
	void OnConfigSelected(wxCommandEvent &event);

	void PopulateConfigurations();
	void AddEntry(const wxString &projectName, const wxString &selectedConf);
	
public:
	/** Constructor */
	ConfigurationManagerDlg( wxWindow* parent );
};


#endif // __configuration_manager_dlg__
