#include "precompiled_header.h"

#include "cpp_symbol_tree.h"
#include "frame.h"
#include <wx/xrc/xmlres.h>

using namespace codelite;

//----------------------------------------------------------------
// accessory function
//----------------------------------------------------------------
wxImageList* CreateSymbolTreeImages()
{
	wxImageList *images = new wxImageList(16, 16, true);

	images->Add(wxXmlResource::Get()->LoadBitmap(wxT("project")));		   // 0
	images->Add(wxXmlResource::Get()->LoadBitmap(wxT("namespace")));	   // 1
	images->Add(wxXmlResource::Get()->LoadBitmap(wxT("globals")));		   // 2
	images->Add(wxXmlResource::Get()->LoadBitmap(wxT("class")));		   // 3
    images->Add(wxXmlResource::Get()->LoadBitmap(wxT("struct")));		   // 4
	images->Add(wxXmlResource::Get()->LoadBitmap(wxT("func_public")));	   // 5
	images->Add(wxXmlResource::Get()->LoadBitmap(wxT("func_protected")));  // 6
	images->Add(wxXmlResource::Get()->LoadBitmap(wxT("func_private")));	   // 7
	images->Add(wxXmlResource::Get()->LoadBitmap(wxT("member_public")));   // 8
	images->Add(wxXmlResource::Get()->LoadBitmap(wxT("member_protected")));// 9
	images->Add(wxXmlResource::Get()->LoadBitmap(wxT("member_private")));  // 10
	
	wxBitmap bmp;

	// typedef
	bmp = wxXmlResource::Get()->LoadBitmap(wxT("typedef"));				   // 11
	bmp.SetMask(new wxMask(bmp, wxColor(0, 128, 128)));
	images->Add(bmp);

	// macro (same icon as typedef)
	bmp = wxXmlResource::Get()->LoadBitmap(wxT("typedef"));				   // 12
	bmp.SetMask(new wxMask(bmp, wxColor(0, 128, 128)));
	images->Add(bmp);

	bmp = wxXmlResource::Get()->LoadBitmap(wxT("enum"));				   // 13
	bmp.SetMask(new wxMask(bmp, wxColor(0, 128, 128)));
	images->Add(bmp);

	return images;
}

BEGIN_EVENT_TABLE(CppSymbolTree, SymbolTree)
	EVT_LEFT_DCLICK(CppSymbolTree::OnMouseDblClick)
END_EVENT_TABLE()

void CppSymbolTree::OnMouseDblClick(wxMouseEvent& event)
{
	//-----------------------------------------------------
	// We override the doubleclick on item event
	// to demonstrate how to access the tag information 
	// stored in the symbol tree
	//-----------------------------------------------------

	wxTreeItemId treeItem = GetSelection();
	if(!treeItem)
	{
		event.Skip();
		return;
	}

	// Make sure the double click was done on an actual item
	int flags = wxTREE_HITTEST_ONITEMLABEL;
	if(HitTest(event.GetPosition(), flags) != treeItem)
	{
		event.Skip();
		return;
	}

	//-----------------------------------------------------
	// Each tree items, keeps a private user data that 
	// holds the key for searching the its corresponding
	// node in the m_tree data structure
	//-----------------------------------------------------
	MyTreeItemData* itemData = static_cast<MyTreeItemData*>(GetItemData(treeItem));
	if( !itemData )
	{
		event.Skip();
		return;
	}

	// Find the node in the tree
	TagNode* node = m_tree->Find(itemData->GetKey());
	if( node )
	{
		if(node->GetData().GetKind() == wxT("project"))
		{
			event.Skip();
			return;
		}
		// Open the file and set the cursor to line number
		Frame::Get()->OpenFile(node->GetData());
		return;
	}
	event.Skip();
}
