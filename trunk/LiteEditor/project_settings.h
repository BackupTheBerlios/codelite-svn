#ifndef PROJECT_SETTINGS_H
#define PROJECT_SETTINGS_H

#include "build_config.h"
#include "wx/string.h"
#include <map>

class ProjectSettings : public ConfObject {
	wxString m_compilerName;
	std::map<wxString, BuildConfigPtr> m_configs;

public:
	ProjectSettings(wxXmlNode *node);
	virtual ~ProjectSettings();
	wxXmlNode *ToXml() const;

	BuildConfigPtr GetBuildConfiguration(const wxString &configName) const;
};

typedef SmartPtr<ProjectSettings> ProjectSettingsPtr;

#endif // PROJECT_SETTINGS_H
