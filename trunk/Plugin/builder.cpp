#include "builder.h"
#include "macros.h"

Builder::Builder(const wxString &name)
: m_name(name)
{
}

Builder::~Builder()
{
}

wxString Builder::NormalizeConfigName(const wxString &confgName)
{
	wxString normalized(confgName);
	TrimString(normalized);
	normalized.Replace(wxT(" "), wxT("_"));
	return normalized;
}
