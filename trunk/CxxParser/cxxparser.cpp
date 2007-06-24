#include "cxxparser.h"
#include "symbol_table.h"

extern std::string &getFileName();
extern std::string getCurrentScope();
extern int yylineno;

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
	data.line = yylineno;
	data.scope = getCurrentScope();
}
