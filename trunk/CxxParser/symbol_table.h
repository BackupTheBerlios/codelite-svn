#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "symbol_entry.h"
#include "map"
#include "list"
#include "smart_ptr.h"
#include "tree.h"
#include "tree_node.h"

typedef TreeNode<std::string, SymbolData> SymbolTreeNode;
typedef Tree<std::string, SymbolData> SymbolTree;
typedef SmartPtr<SymbolTree> SymbolTreePtr;
typedef SmartPtr<SymbolTreeNode> SymbolTreeNodePtr;

class SymbolTable
{
	SymbolTreePtr m_tree;
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


