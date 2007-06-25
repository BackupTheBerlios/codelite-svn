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
	std::string key = entry.scope + "::" + entry.name;
	m_symbols.insert(MapEntry(key, entry)); 
}

bool SymbolTable::IsSymbolExist(const std::string &name, const std::string &scope) const
{
	std::string key = scope + "::" + name;
	return m_symbols.find(key) != m_symbols.end();
}
