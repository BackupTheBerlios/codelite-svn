#include "Target.h"

#include "wx/string.h"
#include "wx/arrstr.h"

Target::Target(wxString name, wxArrayString deps, wxArrayString actions) :
m_name(name),
m_deps(deps),
m_actions(actions)
{
	stripWhitespace();
}

void Target::stripWhitespace()
{
	for(int i = 0; i < (int)m_deps.size(); i++)
	{
		wxString dep = m_deps[i];
		
		dep.erase(0, dep.find_first_not_of(wxT("\n\r\t ")));
		dep.erase(dep.find_last_not_of(wxT("\\\t\n\r "))+1);
	}
}

wxString Target::getName()
{
	return m_name;
}

wxArrayString Target::getDeps()
{
	return m_deps;
}

wxArrayString Target::getActions()
{
	return m_actions;
}
