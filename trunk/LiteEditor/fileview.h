#ifndef FILE_VIEW_TREE_H
#define FILE_VIEW_TREE_H

#include "wx/treectrl.h"

/**
 * Class FilewViewTreeItemData, a user defined class which stores a node private information
 *
 * \date 12-04-2007
 * \author Eran
 *
 */
class FilewViewTreeItemData : public wxTreeItemData
{
public:
    FilewViewTreeItemData(){ }
};

class FileViewTree : public wxTreeCtrl
{
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

private:
	// Build the actual tree from the workspace
	void BuildTree();
};

#endif // FILE_VIEW_TREE_H
