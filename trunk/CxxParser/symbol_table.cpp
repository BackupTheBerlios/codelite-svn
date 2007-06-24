#include "symbol_table.h"
 
SymbolTable::SymbolTable()   
{
}

SymbolTable::~SymbolTable()
{
}

SymbolTable& SymbolTable::instance()
{
	static SymbolTable theTable;
	return theTable;
}

SymbolTableEntryPtr SymbolTable::AddSymbol(SymbolTableEntry *entry)
{
	return NULL;
}

bool SymbolTable::IsSymbolExist(const std::string &name, const std::string &scope) const
{
	return true;
}
