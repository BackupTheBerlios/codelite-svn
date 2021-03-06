#include "function.h"

Function::Function()
{
	Reset();
}

Function::~Function()
{
}

void Function::Reset()
{
	m_name = "";
	m_scope = "";
	m_returnValue.Reset();
	m_name = "";
	m_signature = "";
}

void Function::Print()
{
	fprintf(	stdout, "{m_name=%s, m_scope=%s, m_signature=%s\nm_returnValue=", 
				m_name.c_str(), m_scope.c_str(), m_signature.c_str());
	m_returnValue.Print();
	fprintf(	stdout, "}\n");
	fflush(stdout);
}

