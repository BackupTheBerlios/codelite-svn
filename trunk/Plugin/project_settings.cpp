#include "project_settings.h"
#include "xmlutils.h"

ProjectSettings::ProjectSettings(wxXmlNode *node)
{
	if(node){
		// load configurations
		wxXmlNode *child = node->GetChildren();
		while(child) {
			if(child->GetName() == wxT("Configuration")){
				wxString configName = XmlUtils::ReadString(child, wxT("Name"));
				m_configs.insert(std::pair<wxString, BuildConfigPtr>(configName, new BuildConfig(child)));
			}
			child = child->GetNext();
		}
	}else{
		//create new settings with default values
		m_compilerName = wxT("g++");
		m_configs.insert(std::pair<wxString, BuildConfigPtr>(wxT("Debug"), new BuildConfig(NULL)));
	}
}

ProjectSettings::~ProjectSettings()
{
}

wxXmlNode *ProjectSettings::ToXml() const
{
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Settings"));
	std::map<wxString, BuildConfigPtr>::const_iterator iter = m_configs.begin();
	for(; iter != m_configs.end(); iter++){
		node->AddChild(iter->second->ToXml());
	}

	return node;
}

BuildConfigPtr ProjectSettings::GetBuildConfiguration(const wxString &configName) const
{
	wxString confName = configName;
	if(confName.IsEmpty()){
		confName = wxT("Debug");
	}

	std::map<wxString, BuildConfigPtr>::const_iterator iter = m_configs.find(confName);
	if(iter == m_configs.end()){
		return NULL;
	}
	return iter->second;
}

BuildConfigPtr ProjectSettings::GetFirstBuildConfiguration(ProjectSettingsCookie &cookie) const
{
	cookie.iter = m_configs.begin();
	if(cookie.iter != m_configs.end()){
		BuildConfigPtr conf = cookie.iter->second;
		cookie.iter++;
		return conf;
	}
	return NULL;
}

BuildConfigPtr ProjectSettings::GetNextBuildConfiguration(ProjectSettingsCookie &cookie) const
{
	if(cookie.iter != m_configs.end()){
		BuildConfigPtr conf = cookie.iter->second;
		cookie.iter++;
		return conf;
	}
	return NULL;
}

void ProjectSettings::SetBuildConfiguration(const BuildConfigPtr bc)
{
	m_configs[bc->GetName()] = bc;
}
