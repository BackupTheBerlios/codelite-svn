#ifndef EXPRESSION_RESULT_H
#define EXPRESSION_RESULT_H

#include "string"

class ExpressionResult
{
public:
	bool			m_isFunc;
	std::string m_name;
	bool 			m_isThis;
	
public:
	ExpressionResult();
	virtual ~ExpressionResult();
	void Reset();
	void Print();
};
#endif //EXPRESSION_RESULT_H
