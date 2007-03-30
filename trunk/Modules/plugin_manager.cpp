#include "plugin_manager.h"
#include "pluggable.h"

PluginManager::PluginManager()
{
}

PluginManager::~PluginManager()
{
	std::map<wxString, Pluggable*>::iterator iter = m_classes.begin();
	for(; iter != m_classes.end(); iter++)
	{
		delete iter->second;
	}
	m_classes.clear();
}

void PluginManager::AddClass(Pluggable* pcls)
{
	m_classes[pcls->GetClassName()] = pcls;
}

Pluggable *PluginManager::FindClass(const wxString &clasName)
{
	std::map<wxString, Pluggable*>::iterator iter = m_classes.find(clasName);
	if(iter == m_classes.end())
	{
		// no match was found
		return NULL;
	}
	// Create a new instance of the class and return it to caller
	return iter->second->Clone();
}
