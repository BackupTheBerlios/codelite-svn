#ifndef CONFIGURATION_MAPPING_H
#define CONFIGURATION_MAPPING_H

#include "wx/string.h"
#include "wx/xml/xml.h"
#include "list"
#include "smart_ptr.h"

class Configuration {
public:
	typedef std::list<std::pair<wxString, wxString> > ConfigMappingList;

private:
	wxString m_name;
	ConfigMappingList m_mappingList;

public:
	Configuration();
	Configuration(wxXmlNode *node);
	virtual ~Configuration();
	wxXmlNode *ToXml() const;

	const wxString &GetName() const{ return m_name; }
	const ConfigMappingList &GetMapping() const { return m_mappingList; }
	void SetConfigMappingList(const ConfigMappingList &mapList) { m_mappingList = mapList; }
	void SetName(const wxString &name){ m_name = name; }
};

typedef SmartPtr<Configuration> ConfigurationPtr;

class ConfigurationMapping {
	std::list<ConfigurationPtr> m_configurationList;

public:
	ConfigurationMapping(wxXmlNode *node);
	virtual ~ConfigurationMapping();
	wxXmlNode *ToXml() const;
	const std::list<ConfigurationPtr>& GetConfigurations() const { return m_configurationList; };
	void RemoveConfiguration(const wxString &configName);
	void SetConfiguration(ConfigurationPtr conf);

};

typedef SmartPtr<ConfigurationMapping> ConfigurationMappingPtr;

#endif // CONFIGURATION_MAPPING_H
