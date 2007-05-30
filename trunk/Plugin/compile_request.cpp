#include "compile_request.h"

CompileReqeust::CompileReqeust(const wxString &projectName, const wxString &configurationName)
: m_project(projectName)
, m_configuration(configurationName)
{
}

CompileReqeust::~CompileReqeust()
{
}

void CompileReqeust::Process()
{
}
