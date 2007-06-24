#include "cxxparser.h"
#include "symbol_table.h"

extern std::string &getFileName();
extern std::string getCurrentScope();
extern int yylineno;

SymbolTableEntry *createClassSymbol(	const std::string &optTmpQualifier, 
											const std::string &keyWord, 
											const std::string &optClassQualifer, 
											const std::string &className, 
											bool isImpl)
{
	SymbolTableEntry *entry = new SymbolTableEntry();
	entry->file = getFileName();
	entry->kind = class_t;
	entry->name = className;
	entry->line = yylineno;
	entry->scope = getCurrentScope();
	return entry;
}
