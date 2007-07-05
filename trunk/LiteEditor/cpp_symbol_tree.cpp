#include "precompiled_header.h"

#include "cpp_symbol_tree.h"
#include "manager.h"
#include <wx/xrc/xmlres.h>
#include <wx/imaglist.h>

IMPLEMENT_DYNAMIC_CLASS(CppSymbolTree, SymbolTree)

//----------------------------------------------------------------
// accessory function
//----------------------------------------------------------------
wxImageList* CreateSymbolTreeImages()
{
	wxImageList *images = new wxImageList(16, 16, true);

	images->Add(wxXmlResource::Get()->LoadBitmap(_T("project")));		   // 0
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("namespace")));	   // 1
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("globals")));		   // 2
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("class")));		   // 3
    images->Add(wxXmlResource::Get()->LoadBitmap(_T("struct")));		   // 4
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("func_public")));	   // 5
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("func_protected")));  // 6
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("func_private")));	   // 7
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("member_public")));   // 8
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("member_protected")));// 9
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("member_private")));  // 10
	
	wxBitmap bmp;

	// typedef
	bmp = wxXmlResource::Get()->LoadBitmap(_T("typedef"));				   // 11
	bmp.SetMask(new wxMask(bmp, wxColor(0, 128, 128)));
	images->Add(bmp);

	// macro (same icon as typedef)
	bmp = wxXmlResource::Get()->LoadBitmap(_T("typedef"));				   // 12
	bmp.SetMask(new wxMask(bmp, wxColor(0, 128, 128)));
	images->Add(bmp);

	bmp = wxXmlResource::Get()->LoadBitmap(_T("enum"));				   // 13
	bmp.SetMask(new wxMask(bmp, wxColor(0, 128, 128)));
	images->Add(bmp);

	return images;
}

CppSymbolTree::CppSymbolTree()
{
}

CppSymbolTree::CppSymbolTree(wxWindow *parent, const wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
: SymbolTree(parent, id, pos, size, style)
{
	Connect(GetId(), wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler(CppSymbolTree::OnMouseRightUp));
	Connect(GetId(), wxEVT_LEFT_DCLICK, wxMouseEventHandler(CppSymbolTree::OnMouseDblClick));
}

void CppSymbolTree::OnMouseRightUp(wxTreeEvent &event)
{
	wxTreeItemId item = event.GetItem();
	if(item.IsOk()){
		SelectItem(item, true);
	}
}

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
	} // if( !itemData )

	wxString filename = itemData->GetFileName();
	wxString project = ManagerST::Get()->GetProjectNameByFile(filename);
	int lineno = itemData->GetLineno()-1;
	
	// Open the file and set the cursor to line number
	ManagerST::Get()->OpenFile(filename, project, lineno);
}
