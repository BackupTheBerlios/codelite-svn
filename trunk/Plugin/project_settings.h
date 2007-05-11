#ifndef PROJECT_SETTINGS_H
#define PROJECT_SETTINGS_H

#include "build_config.h"
#include "wx/string.h"
#include <map>

class ProjectSettingsCookie {
public:
	std::map<wxString, BuildConfigPtr>::const_iterator iter;
};

class ProjectSettings : public ConfObject {
	wxString m_compilerName;
	std::map<wxString, BuildConfigPtr> m_configs;

public:
	ProjectSettings(wxXmlNode *node);
	virtual ~ProjectSettings();
	wxXmlNode *ToXml() const;

	BuildConfigPtr GetBuildConfiguration(const wxString &configName) const;
	BuildConfigPtr GetFirstBuildConfiguration(ProjectSettingsCookie &cookie) const;
	BuildConfigPtr GetNextBuildConfiguration(ProjectSettingsCookie &cookie) const;
	void SetBuildConfiguration(const BuildConfigPtr bc) { m_configs[bc->GetName()] = bc; }

};

typedef SmartPtr<ProjectSettings> ProjectSettingsPtr;

#endif // PROJECT_SETTINGS_H
