#include "fileview.h"
#include "manager.h"
#include "tree.h"
#include <wx/xrc/xmlres.h>
#include "wx/imaglist.h"
#include <wx/textdlg.h>
#include <deque>

FileViewTree::FileViewTree()
{
}

void FileViewTree::ConnectEvents()
{
	Connect(XRCID("remove_project"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileViewTree::OnRemoveProject), NULL, this);
	Connect(XRCID("set_as_active"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileViewTree::OnSetActive), NULL, this);
	Connect(XRCID("new_item"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileViewTree::OnNewItem), NULL, this);
	Connect(XRCID("add_existing_item"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileViewTree::OnAddExistingItem), NULL, this);
	Connect(XRCID("new_virtual_folder"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileViewTree::OnNewVirtualFolder), NULL, this);
	Connect(XRCID("project_properties"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileViewTree::OnProjectProperties), NULL, this);
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

	Connect(GetId(), wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler(FileViewTree::OnMouseRightUp));
	Connect(GetId(), wxEVT_LEFT_DCLICK, wxMouseEventHandler(FileViewTree::OnMouseDblClick));
}

FileViewTree::~FileViewTree()
{
	delete m_folderMenu;
	delete m_projectMenu;
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

	// Load the popup menu
	m_folderMenu = wxXmlResource::Get()->LoadMenu(wxT("file_tree_folder"));
	m_projectMenu = wxXmlResource::Get()->LoadMenu(wxT("file_tree_project"));

	ConnectEvents();
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
			if(filename.GetExt().CmpNoCase(wxT("cpp")) == 0){
				icondIndex = 3;
			} else if(filename.GetExt().CmpNoCase(wxT("cxx")) == 0){
				icondIndex = 3;
			} else if(filename.GetExt().CmpNoCase(wxT("c++")) == 0){
				icondIndex = 3;
			} else if(filename.GetExt().CmpNoCase(wxT("cc")) == 0){
				icondIndex = 3;
			} else if(filename.GetExt().CmpNoCase(wxT("c")) == 0){
				icondIndex = 2;
			} else if(filename.GetExt().CmpNoCase(wxT("h")) == 0){
				icondIndex = 4;
			} else if(filename.GetExt().CmpNoCase(wxT("hpp")) == 0){
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
										new FilewViewTreeItemData(node->GetData()));
		
		// Set active project with bold
		if( parentHti == GetRootItem() && ManagerST::Get()->GetActiveProjectName() == node->GetData().GetDisplayName()){
			SetItemBold(hti);
		}

		items[node->GetKey()] = hti;
	}

//	SortTree(m_sortItems);
}

//-----------------------------------------------
// Event handlers
//-----------------------------------------------
void FileViewTree::OnMouseRightUp(wxTreeEvent &event)
{
	wxTreeItemId item = event.GetItem();
	if(item.IsOk()){
		SelectItem(item, true);

		FilewViewTreeItemData *data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
		switch( data->GetData().GetKind() )
		{
		case ProjectItem::TypeProject:
			PopupMenu( m_projectMenu );
			break;
		case ProjectItem::TypeVirtualDirectory:
			PopupMenu( m_folderMenu );
			break;
		default:
			break;
		}
	}
}

void FileViewTree::OnMouseDblClick(wxMouseEvent &event)
{
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
	FilewViewTreeItemData* itemData = static_cast<FilewViewTreeItemData*>(GetItemData(treeItem));
	if( !itemData )
	{
		event.Skip();
		return;
	}

	if( itemData->GetData().GetKind() == ProjectItem::TypeFile ){
		wxString filename = itemData->GetData().GetFile();
		wxString project  = itemData->GetData().Key().BeforeFirst(wxT('.'));
		ManagerST::Get()->OpenFile(filename, project, -1);
		event.Skip();
		return;
	}
	return;
}

void FileViewTree::OnRemoveProject(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxTreeItemId item = GetSelection();
	if(item.IsOk()){
		FilewViewTreeItemData *data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
		if( data->GetData().GetKind() == ProjectItem::TypeProject ){
			DoRemoveProject(data->GetData().GetDisplayName());
		}
	}
}

void FileViewTree::OnAddExistingItem(wxCommandEvent & WXUNUSED(event))
{
}

void FileViewTree::OnNewItem(wxCommandEvent & WXUNUSED(event))
{
}

void FileViewTree::OnSetActive(wxCommandEvent & WXUNUSED(event))
{
	wxTreeItemId item = GetSelection();
	DoSetProjectActive(item);
}

void FileViewTree::DoSetProjectActive(wxTreeItemId &item)
{
	if(item.IsOk()){
		FilewViewTreeItemData *data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
		if( data->GetData().GetKind() == ProjectItem::TypeProject ){

			wxString curActiveProj = ManagerST::Get()->GetActiveProjectName();
			
			// find previous active project and remove its bold style
			wxTreeItemIdValue cookie;
			wxTreeItemId child = GetFirstChild(GetRootItem(), cookie);
			while( child.IsOk() ){
				FilewViewTreeItemData *childData = static_cast<FilewViewTreeItemData*>(GetItemData(child));
				if( childData->GetData().GetDisplayName() == curActiveProj ){
					SetItemBold(child, false);
					break;
				}
				child = GetNextChild(GetRootItem(), cookie);
			}

			ManagerST::Get()->SetActiveProject(data->GetData().GetDisplayName());
			SetItemBold(item);
		}
	}

}

void FileViewTree::OnNewVirtualFolder(wxCommandEvent & WXUNUSED(event))
{
	wxTreeItemId item = GetSelection();
	DoAddVirtualFolder(item);
}

void FileViewTree::DoAddVirtualFolder(wxTreeItemId &parent)
{
	static int count = 0;
	wxString defaultName(wxT("NewDirectory"));
	defaultName << count++;

	wxTextEntryDialog *dlg = new wxTextEntryDialog(this, wxT("Virtual Directory Name:"), wxT("New Virtual Directory"), defaultName);
	if(dlg->ShowModal() == wxID_OK){
		wxString path = GetItemPath(parent);
		if(dlg->GetValue().Trim().IsEmpty())
			return;

		path += wxT(".");
		path += dlg->GetValue();
		ManagerST::Get()->AddVirtualDirectory(path);
	}
	dlg->Destroy();
}

wxString FileViewTree::GetItemPath(wxTreeItemId &item)
{
	std::deque<wxString> queue;
	wxString text = GetItemText(item);
	queue.push_front(text);

	wxTreeItemId p = GetItemParent(item);
	while( p.IsOk() && p != GetRootItem()){
		
		text = GetItemText(p);
		queue.push_front(text);
		p = GetItemParent(p);
	}

	if( queue.empty() ){
		return wxEmptyString;
	}

	wxString path;
	for(size_t i=0; i<queue.size(); i++){
		path += queue.front();
		path += wxT(".");
		queue.pop_front();
	}
	
	path += queue.front();
	return path;
}

void FileViewTree::OnProjectProperties(wxCommandEvent & WXUNUSED(event))
{
}

void FileViewTree::DoRemoveProject(const wxString &name)
{
	wxString message (wxT("You are about to remove project '"));
	message << name << wxT("' ");
	message << wxT(" from the workspace, click 'Yes' to proceed or 'No' to abort.");
	if( wxMessageBox (message, wxT("Confirm"), wxYES_NO) == wxYES ){
		ManagerST::Get()->RemoveProject(name);
	}
}

