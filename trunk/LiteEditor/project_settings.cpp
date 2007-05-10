#include "project_settings.h"
#include "xmlutils.h"

ProjectSettings::ProjectSettings(wxXmlNode *node)
{
	if(node){
		wxXmlNode *comp = XmlUtils::FindFirstByTagName(node, wxT("Compiler"));
		if(comp){
			m_compilerName = XmlUtils::ReadString(comp, wxT("Name"));
		}

		// load configurations
		wxXmlNode *child = node->GetChildren();
		while(child) {
			if(child->GetName() == wxT("Configuration")){
				wxString configName = XmlUtils::ReadString(child, wxT("Name"));
				m_configs.insert(std::pair<wxString, BuildConfigPtr>(configName, new BuildConfig(child)));
			}
			child = child->GetNext();
		}
	}
}

ProjectSettings::~ProjectSettings()
{
}

wxXmlNode *ProjectSettings::ToXml() const
{
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Settings"));
	wxXmlNode *comp = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Compiler"));
	node->AddChild(comp);
	comp->AddProperty(wxT("Name"), m_compilerName);
	
	std::map<wxString, BuildConfigPtr>::const_iterator iter = m_configs.begin();
	for(; iter != m_configs.end(); iter++){
		node->AddChild(iter->second->ToXml());
	}

	return node;
}

BuildConfigPtr ProjectSettings::GetBuildConfiguration(const wxString &configName) const
{
	std::map<wxString, BuildConfigPtr>::const_iterator iter = m_configs.find(configName);
	if(iter == m_configs.end()){
		return NULL;
	}
	return iter->second;
}

