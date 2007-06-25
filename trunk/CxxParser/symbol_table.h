#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "symbol_entry.h"
#include "map"

class SymbolTable
{
	//a multimap that contains our symbols collected from the parsing
	//the key is a concatenation of the scope name + the symbol name
	//multiple entries with same key is allowed.
	std::multimap<std::string, SymbolData> m_symbols;
	typedef std::pair<std::string, SymbolData> MapEntry;
	
public:
	static SymbolTable& instance();
	
	/**
	 * test if a symbol with name and scope already exist in the symbol table
	 */ 
	bool IsSymbolExist(const std::string &name, const std::string &scope) const;
	
	/**
	 * add symbol to the table
	 */ 
	void AddSymbol(const SymbolData &entry);
	
private:
	SymbolTable();
	virtual ~SymbolTable();
};

#endif // SYMBOL_TABLE_H


