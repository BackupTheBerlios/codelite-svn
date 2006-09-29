#ifndef LITEEDITOR_CPP_SYMBOL_TREE_H
#define LITEEDITOR_CPP_SYMBOL_TREE_H

#include "symbol_tree.h"

/// This class represents the GUI tree for the C++ symbols
class CppSymbolTree : public codelite::SymbolTree
{
public:

	/// Nothing special here, just call our parent constructor
	CppSymbolTree(wxWindow *parent, const wxWindowID id)
		: SymbolTree(parent, id)
	{};

	/// destructor
	virtual ~CppSymbolTree()
	{};

	DECLARE_EVENT_TABLE()
	void OnMouseDblClick(wxMouseEvent& event);
};

#endif // LITEEDITOR_CPP_SYMBOL_TREE_H
