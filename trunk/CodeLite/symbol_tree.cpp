#include "precompiled_header.h" 

#ifdef __VISUALC__
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif 

#include "symbol_tree.h"
#include "ctags_manager.h"
#include "tokenizer.h"


IMPLEMENT_DYNAMIC_CLASS(SymbolTree, wxTreeCtrl)

BEGIN_EVENT_TABLE(SymbolTree, wxTreeCtrl)
EVT_SYMBOLTREE_ADD_ITEM(wxID_ANY, SymbolTree::OnAddSymbols)
EVT_SYMBOLTREE_DELETE_ITEM(wxID_ANY, SymbolTree::OnDeleteSymbols)
EVT_SYMBOLTREE_UPDATE_ITEM(wxID_ANY, SymbolTree::OnUpdateSymbols)
EVT_SYMBOLTREE_DELETE_PROJECT(wxID_ANY, SymbolTree::OnDeleteProject)
END_EVENT_TABLE()

SymbolTree::SymbolTree()
: m_tree(NULL)
{
	InitialiseSymbolMap();
}

SymbolTree::SymbolTree(wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
: m_tree(NULL)
{
	InitialiseSymbolMap();
	Create(parent, id, pos, size, style);

	// Connect this tree to the parse thread
	ParseThreadST::Get()->SetNotifyWindow( this );
}

SymbolTree::~SymbolTree()
{
}

void SymbolTree::InitialiseSymbolMap()
{
	//--------------------------------------------------------
	// Initialise the images map (kind:icon_index)
	//--------------------------------------------------------

	m_imagesMap[wxT("project")] = 0;
	m_imagesMap[wxT("namespace")] = 1;
	m_imagesMap[wxT("globals")] = 2;
	m_imagesMap[wxT("class")] = 3;
	m_imagesMap[wxT("interface")] = 3;
	m_imagesMap[wxT("interface_private")] = 3;
	m_imagesMap[wxT("interface_protected")] = 3;
	m_imagesMap[wxT("class_private")] = 3;
	m_imagesMap[wxT("class_public")] = 3;
	m_imagesMap[wxT("class_protected")] = 3;

	m_imagesMap[wxT("struct")] = 4;
	m_imagesMap[wxT("struct_private")] = 4;
	m_imagesMap[wxT("struct_public")] = 4;
	m_imagesMap[wxT("struct_protected")] = 4;

	m_imagesMap[wxT("function")] = 5;
	m_imagesMap[wxT("prototype")] = 5;
	m_imagesMap[wxT("function_public")] = 5;
	m_imagesMap[wxT("prototype_public")] = 5;

	m_imagesMap[wxT("function_protected")] = 6;
	m_imagesMap[wxT("prototype_protected")] = 6;
	m_imagesMap[wxT("function_private")] = 7;
	m_imagesMap[wxT("prototype_private")] = 7;
	m_imagesMap[wxT("variable")] = 8;
	m_imagesMap[wxT("member_public")] = 8;
	m_imagesMap[wxT("member_protected")] = 9;
	m_imagesMap[wxT("member_private")] = 10;
	m_imagesMap[wxT("typedef")] = 11;
	m_imagesMap[wxT("typedef_public")] = 11;
	m_imagesMap[wxT("typedef_private")] = 11;
	m_imagesMap[wxT("typedef_protected")] = 11;
	m_imagesMap[wxT("macro")] = 12;
	m_imagesMap[wxT("macro_private")] = 12;
	m_imagesMap[wxT("macro_protected")] = 12;
	m_imagesMap[wxT("macro_public")] = 12;
	m_imagesMap[wxT("enum")] = 13;
	m_imagesMap[wxT("enum_private")] = 13;
	m_imagesMap[wxT("enum_public")] = 13;
	m_imagesMap[wxT("enum_protected")] = 13;

	m_imagesMap[wxT("method")] = 5;
	m_imagesMap[wxT("method_public")] = 5;
	m_imagesMap[wxT("method_protected")] = 6;
	m_imagesMap[wxT("method_private")] = 7;

	//-----------------------------------------------------------
	// Populate globals kind
	//-----------------------------------------------------------

	m_globalsKind[wxT("<global>")] = true;
	m_globalsKind[wxT("function")] = true;
	m_globalsKind[wxT("prototype")] = true;
	m_globalsKind[wxT("variable")] = true;
}

void SymbolTree::Create(wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
#ifndef __WXGTK__
	style |= (wxTR_HIDE_ROOT | wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT);
#else
	style |= (wxTR_HIDE_ROOT | wxTR_HAS_BUTTONS );
#endif
	wxTreeCtrl::Create(parent, id, pos, size, style);

	TagTreePtr dummy;
	BuildTree( dummy );
} 

void SymbolTree::BuildTree( TagTreePtr& tree )
{
	// Clear the tree
	DeleteAllItems();
	m_globalsMap.clear();
	m_prototypesMap.clear();
	m_sortItems.clear();
	m_macrosMap.clear();

	// Get the current tree
	if( ! tree )
	{
		m_tree = TagsManagerST::Get()->Load();
		if( !m_tree )
		{
			wxLogMessage(wxT("Failed to load Symbols from database!"));
			return;
		}
	}
	else
		m_tree = tree;

	// Add invisible root node
	wxTreeItemId root = AddRoot(wxT("Workspace"), 0, 0); // Workspace icon;
	m_tree->GetRoot()->GetData().SetTreeItemId( root );

	TreeWalker<wxString, TagEntry> walker(m_tree->GetRoot());

	// Iterate over the tree and add items
	m_sortItems.clear();

	Freeze();

	for(; !walker.End(); walker++)
	{
		// Add the item to the tree
		TagNode* node = walker.GetNode();

		// Skip root node
		if(node->IsRoot())
			continue;

		// Add the node
		AddItem(node);
	}

	SortTree(m_sortItems);
	Thaw();
}

void SymbolTree::AddItem(TagNode* node)
{
	// Get node icon index
	TagEntry nodeData = node->GetData();
	

	int iconIndex = GetItemIconIndex(nodeData.GetKind(), nodeData.GetAccess());
	wxString displayName(nodeData.GetDisplayName());
	wxTreeItemId parentHti;

	if(nodeData.GetName().IsEmpty())
		return;

	//-------------------------------------------------------------------------------
	// We gather globals together under special node
	//-------------------------------------------------------------------------------
	if( (nodeData.GetParent() == wxT("<global>")) &&					// parent is global scope
		m_globalsKind.find(nodeData.GetKind()) != m_globalsKind.end() ) //the node kind is one of function, prototype or variable
	{
		//search for the global node for this project
		std::map<wxString, wxTreeItemId>::iterator iter = m_globalsMap.find(node->GetData().GetProject());
		if(iter == m_globalsMap.end())
		{
			// First time, add a node under the project node
			parentHti = AppendItem(node->GetParent()->GetData().GetTreeItemId(),
						wxT("Global Functions and Variables"),
						2, 2, new MyTreeItemData(wxT("Global Functions and Variables")));
			m_globalsMap[nodeData.GetProject()] = parentHti;
		}
		else
			parentHti = iter->second;
	}
	else
		parentHti = node->GetParent()->GetData().GetTreeItemId();

	//---------------------------------------------------------------------------------
	// make prototypes function as child of a special node in the tree
	//---------------------------------------------------------------------------------
	if(nodeData.GetKind() == wxT("prototype"))
	{
		std::map<wxString, wxTreeItemId>::iterator iter = m_prototypesMap.find(node->GetParent()->GetData().GetPath());
		if(iter != m_prototypesMap.end())
		{
			parentHti = iter->second;
		}
		else
			return;
	}

	//---------------------------------------------------------------------------------
	// Macros are gathered under the 'Macros' node
	//---------------------------------------------------------------------------------
	if(nodeData.GetKind() == wxT("macro"))
	{
		std::map<wxString, wxTreeItemId>::iterator iter = m_macrosMap.find(node->GetData().GetProject());
		if(iter != m_macrosMap.end())
		{
			parentHti = iter->second;
		}
		else
			return;
	}

	//only if parent is valid, we add item to the tree
	wxTreeItemId hti;
	if(parentHti.IsOk()){
		hti = AppendItem(parentHti,				// parent
						displayName,			// display name
						iconIndex,				// item image index
						iconIndex,				// selected item image
						new MyTreeItemData(node->GetKey()));
	}

	node->GetData().SetTreeItemId( hti );

	// incase the added note was a container, add a prototype node under it as well
	if( TagsManagerST::Get()->GetCtagsOptions().GetLanguage() == wxT("C++")){
		if(nodeData.IsContainer())
		{
			// add the prototype node under it
			m_prototypesMap[nodeData.GetPath()] = AppendItem(node->GetData().GetTreeItemId(),
				wxT("Functions Prototypes"),
				2, 2, new MyTreeItemData(wxT("Functions Prototypes")));
		}

		// are we a project?
		if(nodeData.GetKind() == wxT("project"))
		{
			// First time, add a 'global' node under the root
			m_globalsMap[nodeData.GetName()] = AppendItem(hti, wxT("Global Functions and Variables"), 2, 2, new MyTreeItemData(wxT("Global Functions and Variables")));
			m_macrosMap[nodeData.GetName()] = AppendItem(hti, wxT("Macros"), 2, 2, new MyTreeItemData(wxT("Macros")));
		}
	}

	m_sortItems[parentHti.m_pItem] = true;
}

void SymbolTree::SortTree(std::map<void*, bool> & nodes)
{
	std::map<void*, bool>::iterator iter = nodes.begin();
	for(; iter != nodes.end(); iter++)
	{
		wxTreeItemId item = iter->first;

		// Does this node has children?
		if( GetChildrenCount( item ) == 0 )
			continue;
		SortChildren(item); 
	}
}

int SymbolTree::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
{
	// Get the items and compare their icons
	int img1, img2;
	img1 = GetItemImage(item1);
	img2 = GetItemImage(item2);
	if(img1 > img2)
		return 1;
	else if(img1 < img2)
		return -1;
	else
	{
		// Items  has the same icons, compare text
		return wxTreeCtrl::OnCompareItems(item1, item2);
	}
}

int SymbolTree::GetItemIconIndex(const wxString &kind, const wxString &access)
{
	wxString key(kind);
	int index(4);       // structs will be the default icon index

	if( !access.IsEmpty() )
		key += wxT("_") + access;

	std::map<wxString, int>::iterator iter = m_imagesMap.find(key);
	if(iter != m_imagesMap.end())
		index = iter->second;
	return index;
}

void SymbolTree::OnUpdateSymbols(SymbolTreeEvent& event)
{
	if( !m_tree ) 
		return;

	std::vector<std::pair<wxString, TagEntry> > items = event.GetItems();

	size_t i=0;
	Freeze();
	for(; i<items.size(); i++)
	{
		wxString key = items[i].first;
		TagEntry data= items[i].second;

		UpdateGuiItem(data, key);
	}
	Thaw();
}

void SymbolTree::UpdateGuiItem(TagEntry& data, const wxString& key)
{
	TagNode* node = m_tree->Find(key);
	if( node )
	{
		// If the update the was made on a prototype entry and we already have a 'function' entry in the 
		// tree, we dont update the GUI tree
		if( data.GetKind() == wxT("prototype") && node->GetData().GetKind() == wxT("function"))
			return;

		// Update the new data with the gui tree item id
		data.SetTreeItemId( node->GetData().GetTreeItemId() );
		node->SetData(data);

		// Update Icon if needed
		int iconIndex = GetItemIconIndex(data.GetKind(), data.GetAccess());
		int curIconIndex = -1;
		wxTreeItemId itemId = node->GetData().GetTreeItemId();
		if(itemId.IsOk()){
			curIconIndex = GetItemImage(itemId);
		}

		if(curIconIndex != iconIndex && itemId.IsOk())
		{
			// Need to update the image as well
			SetItemImage(node->GetData().GetTreeItemId(), iconIndex);
		}
	}
}

void SymbolTree::OnDeleteSymbols(SymbolTreeEvent& event)
{
	if( !m_tree ) 
		return;

	std::vector<std::pair<wxString, TagEntry> > items = event.GetItems();
	size_t i=0;

	Freeze();
	for(; i<items.size(); i++)
	{
		wxString key = items[i].first;
		
		TagNode* node = m_tree->Find(key);
		if( node )
		{
			// Project nodes are never removed from here
			if(node->GetData().GetKind() == wxT("project"))
				continue;

			// Remove the node from the GUI tree
			wxTreeItemId hti = node->GetData().GetTreeItemId();
			Delete(hti);

			// if this node is a container, remove its prototype as well
			if(node->GetData().IsContainer())
			{
				std::map<wxString, wxTreeItemId>::iterator it = m_prototypesMap.find(node->GetData().GetPath());
				if(it != m_prototypesMap.end())
					m_prototypesMap.erase(it);
			}

			// If this node is a project, remove its 'Global' node as well from the 
			// globals map
			if(node->GetData().GetKind() == wxT("project"))
			{
				std::map<wxString, wxTreeItemId>::iterator it = m_globalsMap.find(node->GetData().GetName());
				if(it != m_globalsMap.end())
					m_globalsMap.erase(it);
			}

			// Remove the node from the tag tree as well
			node = m_tree->Remove(key);
			delete node;
		}
	}
	Thaw();
}

void SymbolTree::OnAddSymbols(SymbolTreeEvent& event)
{
	if( !m_tree )
		return;

	std::vector<std::pair<wxString, TagEntry> > items = event.GetItems();
	size_t i=0;
	m_sortItems.clear();

	Freeze();
	for(; i<items.size(); i++)
	{
		wxString key = items[i].first;
		TagEntry nodeData = items[i].second;

		// Check to see if this node already exist in the tree
		if(m_tree->Find(nodeData.Key()))
		{
			UpdateGuiItem(nodeData, key);
		}
		else
		{
			TagNode* node = m_tree->AddEntry(nodeData);
			AddItem(node);
		}
	}
	SortTree(m_sortItems);
	Thaw();
}

void SymbolTree::OnDeleteProject(SymbolTreeEvent& event)
{
	wxString proj = event.GetProject();

	// locate the Tree item associated with this project and rmeove it
	TagNode* node = m_tree->Remove(proj);
	if( node ) 
	{
		// remove succeeded
		wxTreeItemId hti = node->GetData().GetTreeItemId();
		if( hti.IsOk() )
			Delete( hti );
		delete node;
	}
}

void SymbolTree::RemoveSymbols(TagTreePtr tree)
{
	std::vector<std::pair<wxString, TagEntry> >  deletedItems;
	tree->ToVector(deletedItems);

	//send event to this object to remove the symbols
	SymbolTreeEvent event(deletedItems, wxEVT_COMMAND_SYMBOL_TREE_DELETE_ITEM);
	wxPostEvent(this, event);
}

void SymbolTree::AddSymbols(TagTreePtr tree)
{
	std::vector<std::pair<wxString, TagEntry> >  newItems;
	tree->ToVector(newItems);

	//send event to this object to remove the symbols
	SymbolTreeEvent event(newItems, wxEVT_COMMAND_SYMBOL_TREE_ADD_ITEM);
	wxPostEvent(this, event);
}
