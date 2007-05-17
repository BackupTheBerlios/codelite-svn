#include "configuration_mapping.h"
#include "xmlutils.h"
#include "macros.h"

BuildMatrix::BuildMatrix(wxXmlNode *node){
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

BuildMatrix::~BuildMatrix(){
}

wxXmlNode *BuildMatrix::ToXml() const {
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("BuildMatrix"));
	std::list<ConfigurationPtr>::const_iterator iter = m_configurationList.begin();
	for(; iter != m_configurationList.end(); iter++){
		node->AddChild((*iter)->ToXml());
	}
	return node;
}

void BuildMatrix::RemoveConfiguration(const wxString &configName){
	std::list<ConfigurationPtr>::iterator iter = m_configurationList.begin();
	for(; iter != m_configurationList.end(); iter++){
		if((*iter)->GetName() == configName){
			m_configurationList.erase(iter);
			break;
		}
	}
}
void BuildMatrix::SetConfiguration(ConfigurationPtr conf){
	RemoveConfiguration(conf->GetName());
	m_configurationList.push_back(conf);
}

wxString BuildMatrix::GetProjectSelectedConf(const wxString &configName, const wxString &project) const
{
	std::list<ConfigurationPtr>::const_iterator iter = m_configurationList.begin();
	for(; iter != m_configurationList.end(); iter++){
		if((*iter)->GetName() == configName){
			Configuration::ConfigMappingList list = (*iter)->GetMapping();
			Configuration::ConfigMappingList::const_iterator it = list.begin();
			for(; it != list.end(); it++){
				if((*it).m_project == project){
					return (*it).m_name;
				}
			}
			break;
		}
	}
	return wxEmptyString;
}


wxString BuildMatrix::GetSelectedConfigurationName() const 
{
	std::list<ConfigurationPtr>::const_iterator iter = m_configurationList.begin();
	for(; iter != m_configurationList.end(); iter++){
		if((*iter)->IsSelected()){
			return (*iter)->GetName();
		}
	}
	return wxEmptyString;
}

//------------------------------------------------
// Configuration object
//------------------------------------------------
Configuration::Configuration()
: m_name(wxEmptyString)
, m_isSelected(false)
{
}

Configuration::Configuration(wxXmlNode *node){
	if(node){
		m_name = XmlUtils::ReadString(node, wxT("Name"));
		m_isSelected = XmlUtils::ReadBool(node, wxT("Selected"));
		wxXmlNode *child = node->GetChildren();
		while(child){
			if(child->GetName() == wxT("Project")){
				wxString projName = XmlUtils::ReadString(child, wxT("Name"));
				wxString conf = XmlUtils::ReadString(child, wxT("ConfigName"));
				m_mappingList.push_back(ConfigMappingEntry(projName, conf));
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
	node->AddProperty(wxT("Selected"), BoolToString(m_isSelected));

	Configuration::ConfigMappingList::const_iterator iter = m_mappingList.begin();
	for(; iter  != m_mappingList.end(); iter++){
		wxXmlNode *projNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Project"));
		projNode->AddProperty(wxT("Name"), iter->m_project);
		projNode->AddProperty(wxT("ConfigName"), iter->m_name);
		node->AddChild(projNode);
	}
	return node;
}
