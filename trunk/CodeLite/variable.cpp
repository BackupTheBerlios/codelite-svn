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
	m_pattern = "";
}

void Variable::Print()
{
	fprintf(	stdout, "{m_name=%s, m_type=%s, m_typeScope=%s, m_templateDecl=%s, m_isPtr=%s, m_isTemplate=%s}\n", 
				m_name.c_str(), m_type.c_str(), m_typeScope.c_str(), m_templateDecl.c_str(),
				m_isPtr ? "true" : "false", 
				m_isTemplate ? "true" : "false");
	fprintf( stdout, "Pattern: %s\n", m_pattern.c_str());
	fflush(stdout);
}

