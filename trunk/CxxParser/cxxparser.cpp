#include "cxxparser.h"
#include "symbol_table.h"

extern std::string getCurrentScope();
extern int cl_scope_lineno;

void createClassSymbol(	const std::string &optTmpQualifier, 
							const std::string &keyWord, 
							const std::string &optClassQualifer, 
							const std::string &className, 
							const bool isImpl,
							SymbolData &data)
{
	data.file = getFileName();
	data.kind = class_t;
	data.name = className;
	data.line = cl_scope_lineno;
	data.scope = getCurrentScope();
}
