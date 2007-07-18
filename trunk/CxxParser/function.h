#ifndef FUNCTION_H
#define FUNCTION_H

#include "string"
#include "list"
#include "variable.h"

class Function
{
public:
	std::string 	m_name;
	std::string		m_scope;					//functions' scope
	std::string 	m_signature;
	Variable 		m_returnValue;
	
public:
	Function();
	virtual ~Function();

	//clear the class content
	void Reset();
	
	//print the variable to stdout
	void Print();
};

typedef std::list<Function> FunctionList;
#endif //FUNCTION_H
