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

SymbolTree::SymbolTree()
{
	InitialiseSymbolMap();
}

SymbolTree::SymbolTree(wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
	InitialiseSymbolMap();
	Create(parent, id, pos, size, style);
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
	m_imagesMap[wxT("enumerator")] = 14;

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
	BuildTree( wxFileName() );
} 

void SymbolTree::BuildTree(const wxFileName &fileName)
{
	// Clear the tree
	DeleteAllItems();
	m_items.clear();
	m_globalsNode = wxTreeItemId();
	m_prototypesNode = wxTreeItemId();
	m_macrosNode = wxTreeItemId();
	m_sortItems.clear();
	
	m_fileName = fileName;
	// Get the current tree
	m_tree = TagsManagerST::Get()->Load(m_fileName);
	if( !m_tree )
	{
		wxLogMessage(wxT("Failed to load Symbols from database!"));
		return;
	}

	// Add invisible root node
	wxTreeItemId root = AddRoot(wxT("Workspace"), 0, 0); // Workspace icon;
	TreeWalker<wxString, TagEntry> walker(m_tree->GetRoot());
	
	// add three items here:
	// the globals node, the mcros and the prototype node
	m_globalsNode    = AppendItem(root, wxT("Global Functions and Variables"), 2, 2, new MyTreeItemData(wxT("Global Functions and Variables"), -1));
	m_prototypesNode = AppendItem(root, wxT("Functions Prototypes"), 2, 2, new MyTreeItemData(wxT("Functions Prototypes"), -1));
	m_macrosNode     = AppendItem(root, wxT("Macros"), 2, 2, new MyTreeItemData(wxT("Macros"), -1));

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
		if(nodeData.GetKind() == wxT("prototype"))
			parentHti = m_prototypesNode;
		else
			parentHti = m_globalsNode;	
	}
	else
		parentHti = node->GetParent()->GetData().GetTreeItemId();

	//---------------------------------------------------------------------------------
	// Macros are gathered under the 'Macros' node
	//---------------------------------------------------------------------------------
	if(nodeData.GetKind() == wxT("macro"))
	{
		parentHti = m_macrosNode;
	}

	//only if parent is valid, we add item to the tree
	wxTreeItemId hti;

	if(parentHti.IsOk() == false)
	{
		 parentHti = GetRootItem();
	}

	if(parentHti.IsOk()){
		hti = AppendItem(parentHti,				// parent
						displayName,			// display name
						iconIndex,				// item image index
						iconIndex,				// selected item image
						new MyTreeItemData(node->GetData().GetFile(), node->GetData().GetLine()));
		node->GetData().SetTreeItemId( hti );
		m_sortItems[parentHti.m_pItem] = true;
		m_items[nodeData.Key()] = hti;
	}
}

void SymbolTree::SortTree(std::map<void*, bool> & nodes)
{
	std::map<void*, bool>::iterator iter = nodes.begin();
	for(; iter != nodes.end(); iter++)
	{
		wxTreeItemId item = iter->first;
		if(item.IsOk())
		{
			// Does this node has children?
			if( GetChildrenCount( item ) == 0 )
				continue;
			SortChildren(item); 
		}
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

void SymbolTree::UpdateSymbols(SymbolTreeEvent& event)
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
	if(!m_tree)
		return;

	TagNode* node = m_tree->Find(key);
	if( node )
	{
		// Update the new data with the gui tree item id
		data.SetTreeItemId( node->GetData().GetTreeItemId() );
		node->SetData(data);

		// Update Icon if needed
		int iconIndex = GetItemIconIndex(data.GetKind(), data.GetAccess());
		int curIconIndex = -1;
		wxTreeItemId itemId = node->GetData().GetTreeItemId();
		if(itemId.IsOk()){
			curIconIndex = GetItemImage(itemId);
			if(curIconIndex != iconIndex )
			{
				// Need to update the image as well
				SetItemImage(node->GetData().GetTreeItemId(), iconIndex);
				SetItemImage(node->GetData().GetTreeItemId(), iconIndex, wxTreeItemIcon_Selected);

			} // if(curIconIndex != iconIndex )
			//update the linenumber and file
			MyTreeItemData *item_data = new MyTreeItemData(data.GetFile(), data.GetLine());
			wxTreeItemData *old_data = GetItemData(itemId);
			if(old_data)
				delete old_data;
			SetItemData(itemId, item_data);
		}
	}
}

void SymbolTree::DeleteSymbols(SymbolTreeEvent& event)
{
	if(!m_tree)
		return;

	std::vector<std::pair<wxString, TagEntry> > items = event.GetItems();
	Freeze();
	for(size_t i=0; i<items.size(); i++)
	{
		wxString key  = items.at(i).first;
		TagEntry data = items.at(i).second;

		std::map<wxString, wxTreeItemId>::iterator iter = m_items.find(key);
		if(iter != m_items.end() && iter->second.IsOk())
		{
			wxTreeItemId hti = iter->second;
			Delete(hti);
			m_items.erase(iter);
		}
	} // for(size_t i=0; i<items.size(); i++)
	Thaw();
}


void SymbolTree::AddSymbols(SymbolTreeEvent& event)
{
	if(!m_tree)
		return;

	m_sortItems.clear();
	std::vector<std::pair<wxString, TagEntry> > items = event.GetItems();
	Freeze();
	for(size_t i=0; i<items.size(); i++)
	{
		TagEntry data = items.at(i).second;
		if(m_tree)
		{
			TagNode *node = m_tree->AddEntry(data);
			if(node)
			{
				AddItem(node);
			}
		}
	} // for(size_t i=0; i<items.size(); i++)
	SortTree(m_sortItems);
	m_sortItems.clear();
	Thaw();
}
