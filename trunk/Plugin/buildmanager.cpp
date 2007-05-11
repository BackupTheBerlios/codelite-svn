#include "buildmanager.h"
#include "builder.h"

BuildManager::BuildManager()
{
}

BuildManager::~BuildManager()
{
}

BuildManager *BuildManager::Get()
{
	static BuildManager theManager;
	return &theManager;
}

void BuildManager::AddBuilder(Builder *builder)
{
	if(!builder){
		return;
	}

	m_builders[builder->GetName()] = builder;
}

void BuildManager::RemoveBuilder(const wxString &name)
{
	std::map<wxString, Builder*>::iterator iter = m_builders.find(name);
	if(iter != m_builders.end()){
		m_builders.erase(iter);
	}
}


