#include "fileview.h"
#include "manager.h"
#include "tree.h"
#include <wx/xrc/xmlres.h>

FileViewTree::FileViewTree()
{
}

FileViewTree::FileViewTree(wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
	Create(parent, id, pos, size, style);

	// Initialise images map
	wxImageList *images = new wxImageList(16, 16, true);
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("project")));				//0
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("folder")));				//1
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("page_white_c")));			//2
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("page_white_cplusplus")));	//3
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("page_white_h")));			//4
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("page_white_text")));		//5
	AssignImageList( images );
}

FileViewTree::~FileViewTree()
{
}

void FileViewTree::Create(wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
#ifndef __WXGTK__
	style |= (wxTR_HIDE_ROOT | wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT);
#else
	style |= (wxTR_HIDE_ROOT | wxTR_HAS_BUTTONS );
#endif
	wxTreeCtrl::Create(parent, id, pos, size, style);

	BuildTree();
} 

void FileViewTree::BuildTree()
{
	if( ManagerST::Get()->IsWorkspaceOpen() ){
		DeleteAllItems();

		// Add an invisible tree root
		wxTreeItemId root = AddRoot(wxT("Workspace"));
		wxArrayString list;
		ManagerST::Get()->GetProjectList(list);

		Freeze();
		for(size_t n=0; n<list.GetCount(); n++){
			BuildProjectNode(list.Item(n));		
		}
		Thaw();
	}
}

int FileViewTree::GetIconIndex(const ProjectItem &item)
{
	int icondIndex(5);
	switch( item.GetKind() )
	{
	case ProjectItem::TypeProject:
		icondIndex = 0;
		break;
	case ProjectItem::TypeVirtualDirectory:
		icondIndex = 1;	// Folder
		break;
	case ProjectItem::TypeFile:
		{
			wxFileName filename(item.GetFile());
			if(filename.GetExt().CmpNoCase("cpp") == 0){
				icondIndex = 3;
			} else if(filename.GetExt().CmpNoCase("cxx") == 0){
				icondIndex = 3;
			} else if(filename.GetExt().CmpNoCase("c++") == 0){
				icondIndex = 3;
			} else if(filename.GetExt().CmpNoCase("cc") == 0){
				icondIndex = 3;
			} else if(filename.GetExt().CmpNoCase("c") == 0){
				icondIndex = 2;
			} else if(filename.GetExt().CmpNoCase("h") == 0){
				icondIndex = 4;
			} else if(filename.GetExt().CmpNoCase("hpp") == 0){
				icondIndex = 4;
			} else {
				icondIndex = 5;
			}
			break;
		}
	default:
		icondIndex = 5;	// Text file
		break;
	}
	return icondIndex;
}

void FileViewTree::BuildProjectNode(const wxString &projectName)
{
	ProjectTreePtr tree = ManagerST::Get()->GetProjectFileViewTree(projectName);
	TreeWalker<wxString, ProjectItem> walker(tree->GetRoot());

	std::map<wxString, wxTreeItemId> items;
	for(; !walker.End(); walker++)
	{
		// Add the item to the tree
		ProjectTreeNode* node = walker.GetNode();
		wxTreeItemId parentHti;
		if(node->IsRoot()){
			parentHti = GetRootItem();
		} else {
			wxString parentKey = node->GetParent()->GetKey();
			if( items.find(parentKey) == items.end() )
				continue;
			parentHti = items.find(parentKey)->second;
		}

		wxTreeItemId hti = AppendItem(	parentHti,							// parent
										node->GetData().GetDisplayName(),	// display name
										GetIconIndex(node->GetData()),		// item image index
										GetIconIndex(node->GetData()),		// selected item image
										new FilewViewTreeItemData());
		items[node->GetKey()] = hti;
	}

//	SortTree(m_sortItems);
}
