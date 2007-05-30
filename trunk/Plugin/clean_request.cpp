#include "clean_request.h"

CleanRequest::CleanRequest(const wxString &projectName, const wxString &configurationName)
: m_project(projectName)
, m_configuration(configurationName)
{
}

CleanRequest::~CleanRequest()
{
}

//do the actual cleanup
void CleanRequest::Process()
{
}
