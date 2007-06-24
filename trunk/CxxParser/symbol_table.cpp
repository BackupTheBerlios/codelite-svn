#include "symbol_table.h"
 
SymbolTable::SymbolTable()   
{
	SymbolData entry;
	entry.name = "<global>";
	
	//create a dummy tree root node representing the global scope
	m_tree = new SymbolTree(entry.name, entry);
}

SymbolTable::~SymbolTable()
{
	//m_tree is a smart pointer, so no need to delete it here
}

SymbolTable& SymbolTable::instance()
{
	static SymbolTable theTable;
	return theTable;
}

void SymbolTable::AddSymbol(const SymbolData &entry)
{
	std::string key = entry.MakeKey();
}

bool SymbolTable::IsSymbolExist(const std::string &name, const std::string &scope) const
{
	return true;
}
