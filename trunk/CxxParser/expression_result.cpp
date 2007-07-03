#include "expression_result.h"

#define BOOL_TO_STR(b) b ? "true" : "false"

ExpressionResult::ExpressionResult()
{
	Reset();
}

ExpressionResult::~ExpressionResult()
{
}

void ExpressionResult::Print()
{
	printf("{m_name:%s, m_isFunc:%s, m_isThis:%s, m_isaType:%s, m_isPtr:%s, m_scope:%s}\n", 
				m_name.c_str(), 
				BOOL_TO_STR(m_isFunc), 
				BOOL_TO_STR(m_isThis),
				BOOL_TO_STR(m_isaType),
				BOOL_TO_STR(m_isPtr),
				m_scope.c_str());
}

void ExpressionResult::Reset()
{
	m_isFunc = false;
	m_name = "";
	m_isThis = false;
	m_isaType = false;
	m_isPtr = false;
	m_scope = "";
}
