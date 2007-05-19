#include "builder_gnumake.h"

BuilderGnuMake::BuilderGnuMake()
: Builder(wxT("GNU make"))
{
}

BuilderGnuMake::~BuilderGnuMake()
{
}

bool BuilderGnuMake::BuildProject(const wxString &project, const wxString &target)
{
	return true;
}

bool BuilderGnuMake::BuildWorkspace(const wxString &target)
{
	return true;
}

bool BuilderGnuMake::Export(const wxString &project)
{
	return true;
}
