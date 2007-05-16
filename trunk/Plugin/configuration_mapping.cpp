#include "configuration_mapping.h"
#include "xmlutils.h"

ConfigurationMapping::ConfigurationMapping(wxXmlNode *node){
	if(node){
		wxXmlNode *config = node->GetChildren();
		while(config){
			if(config->GetName() == wxT("Configuration")){
				m_configurationList.push_back(new Configuration(config));
			}
			config = config->GetNext();
		}
	}else{
		//construct default empty mapping
	}
}

ConfigurationMapping::~ConfigurationMapping(){
}

wxXmlNode *ConfigurationMapping::ToXml() const {
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("ConfigurationMapping"));
	std::list<ConfigurationPtr>::const_iterator iter = m_configurationList.begin();
	for(; iter != m_configurationList.end(); iter++){
		node->AddChild((*iter)->ToXml());
	}
	return node;
}

void ConfigurationMapping::RemoveConfiguration(const wxString &configName){
	std::list<ConfigurationPtr>::iterator iter = m_configurationList.begin();
	for(; iter != m_configurationList.end(); iter++){
		if((*iter)->GetName() == configName){
			m_configurationList.erase(iter);
			break;
		}
	}
}
void ConfigurationMapping::SetConfiguration(ConfigurationPtr conf){
	RemoveConfiguration(conf->GetName());
	m_configurationList.push_back(conf);
}

//------------------------------------------------
// Configuration object
//------------------------------------------------
Configuration::Configuration()
: m_name(wxEmptyString)
{
}

Configuration::Configuration(wxXmlNode *node){
	if(node){
		m_name = XmlUtils::ReadString(node, wxT("Name"));
		wxXmlNode *child = node->GetChildren();
		while(child){
			if(child->GetName() == wxT("Project")){
				wxString projName = XmlUtils::ReadString(child, wxT("Name"));
				wxString conf = XmlUtils::ReadString(child, wxT("ConfigName"));
				m_mappingList.push_back(std::pair<wxString, wxString>(projName, conf));
			}
			child = child->GetNext();
		}
	}
}

Configuration::~Configuration(){
}

wxXmlNode *Configuration::ToXml() const{
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Configuration"));
	node->AddProperty(wxT("Name"), m_name);
	Configuration::ConfigMappingList::const_iterator iter = m_mappingList.begin();
	for(; iter  != m_mappingList.end(); iter++){
		wxXmlNode *projNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Project"));
		projNode->AddProperty(wxT("Name"), iter->first);
		projNode->AddProperty(wxT("ConfigName"), iter->second);
		node->AddChild(projNode);
	}
	return node;
}

 