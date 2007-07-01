#include "variable.h"

Variable::Variable()
{
	Reset();
}

Variable::~Variable()
{
}

void Variable::Reset()
{
	m_type = "";
	m_templateDecl = "";
	m_name = "";
	m_isTemplate = false;
	m_isPtr = false;
	m_typeScope = "";
}

void Variable::Print()
{
	fprintf(	stdout, "{m_name=%s, m_type=%s, m_typeScope=%s, m_templateDecl=%s, m_isPtr=%s}\n", 
				m_name.c_str(), m_type.c_str(), m_typeScope.c_str(), m_templateDecl.c_str(),
				m_isPtr ? "true" : "false");
	fflush(stdout);
}

