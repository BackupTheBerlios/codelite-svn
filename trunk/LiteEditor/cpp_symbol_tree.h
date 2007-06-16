#ifndef LITEEDITOR_CPP_SYMBOL_TREE_H
#define LITEEDITOR_CPP_SYMBOL_TREE_H

#include "symbol_tree.h"

/// This class represents the GUI tree for the C++ symbols
class CppSymbolTree : public SymbolTree
{
public:

	CppSymbolTree();
	
	/// Nothing special here, just call our parent constructor
	CppSymbolTree(wxWindow *parent, const wxWindowID id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = 0);
		

	/// destructor
	virtual ~CppSymbolTree()
	{};

protected:
	virtual void OnMouseDblClick(wxMouseEvent& event);
	virtual void OnMouseRightUp(wxTreeEvent& event);
	
	DECLARE_DYNAMIC_CLASS(CppSymbolTree)
};

#endif // LITEEDITOR_CPP_SYMBOL_TREE_H
