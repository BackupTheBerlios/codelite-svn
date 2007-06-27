#ifndef TARGET_H_INCLUDED
#define TARGET_H_INCLUDED

#include "wx/string.h"
#include "wx/arrstr.h"

#include <vector>

class Target
{
	public:
		Target(wxString name, wxArrayString deps, wxArrayString actions);
		void stripWhitespace();
		void splitDepencencies();
		
		wxString getName();
		wxArrayString getDeps();
		wxArrayString getActions();
	
	private:
	
		wxString m_name;
		wxArrayString m_deps;
		wxArrayString m_actions;
};

typedef std::vector<Target> Targets;

#endif // TARGET_H_INCLUDED
