#include "compile_request.h"

CompileReqeust::CompileReqeust(wxEvtHandler *owner, const wxString &projectName)
: CompilerAction(owner)
, m_project(projectName)
{
}

CompileReqeust::~CompileReqeust()
{
}

void CompileReqeust::Process()
{
}
