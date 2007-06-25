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

void SymbolTable::AddSymbol(const SymbolData &entry)
{
	m_symbols.insert(MapEntry(entry.name, entry)); 
	printf("Symbol: %s was added the symbol table\n", entry.name.c_str());
}

bool SymbolTable::IsSymbolExist(const std::string &name, const std::string &scope) const
{
	return m_symbols.find(name) != m_symbols.end();
}

void SymbolTable::DiscardAll()
{
	m_symbols.clear();
}
