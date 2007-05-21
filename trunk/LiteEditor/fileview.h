#ifndef FILE_VIEW_TREE_H
#define FILE_VIEW_TREE_H

#include "wx/treectrl.h" 
#include "project.h"

class wxMenu;

/**
 * Class FilewViewTreeItemData, a user defined class which stores a node private information
 *
 * \date 12-04-2007
 * \author Eran
 *
 */
class FilewViewTreeItemData : public wxTreeItemData
{
	ProjectItem m_item;
public:
	FilewViewTreeItemData(const ProjectItem &item) : m_item(item) { }
	const ProjectItem &GetData() const { return m_item; }
};

class FileViewTree : public wxTreeCtrl
{
	wxMenu *m_folderMenu;
	wxMenu *m_projectMenu;
	wxMenu *m_fileMenu;

	std::list<wxTreeItemId> m_itemsToSort;
public:
	/**
	 * Default cosntructor.
	 */
	FileViewTree();

	/**
	 * Parameterized constructor.
	 * \param parent Tree parent window
	 * \param id Window id
	 * \param pos Window position
	 * \param size Window size
	 * \param style Window style
	 */
	FileViewTree(wxWindow *parent, const wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
	
	/**
	 * Destructor .
	 */
	virtual ~FileViewTree(void);	

	/**
	 * Create tree, usually called after constructing FileViewTree with default constructor.
	 * \param parent Tree parent window
	 * \param id Window id
	 * \param pos Window position
	 * \param size Window size
	 * \param style Window style
	 */
	virtual void Create(wxWindow *parent, const wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);

	// Build the actual tree from the workspace
	void BuildTree();

protected:
	virtual void OnPopupMenu(wxTreeEvent &event);
	virtual void OnMouseDblClick(wxMouseEvent &event);
	virtual void OnRemoveProject(wxCommandEvent &event);
	virtual void OnSetActive(wxCommandEvent &event);
	virtual void OnNewItem(wxCommandEvent &event);
	virtual void OnAddExistingItem(wxCommandEvent &event);
	virtual void OnNewVirtualFolder(wxCommandEvent &event);
	virtual void OnProjectProperties(wxCommandEvent &event);
	virtual void OnSortItem(wxCommandEvent &event);
	virtual void OnRemoveVirtualFolder(wxCommandEvent &event);
	virtual void OnRemoveItem(wxCommandEvent &event);
	virtual void OnExportMakefile(wxCommandEvent &event);

	// Tree sorting
	virtual int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);

private:
	// Build project node
	void BuildProjectNode(const wxString &projectName);
	int GetIconIndex(const ProjectItem &item);
	void ConnectEvents();
	wxString GetItemPath(wxTreeItemId &item);

	void DoRemoveProject(const wxString &name);
	void DoSetProjectActive(wxTreeItemId &item);
	void DoAddVirtualFolder(wxTreeItemId &parent);
	void DoRemoveVirtualFolder(wxTreeItemId &parent);
	void DoRemoveItem(wxTreeItemId &item);
};

#endif // FILE_VIEW_TREE_H

