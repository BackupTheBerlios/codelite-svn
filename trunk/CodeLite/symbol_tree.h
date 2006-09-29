#ifndef CODELITE_SYMBOL_TREE_H
#define CODELITE_SYMBOL_TREE_H

#include "ctags_manager.h"
#include "parse_thread.h"

namespace codelite
{

/**
 * Class MyTreeItemData, a user defined class which keeps the full name of a tree item.
 * This will allow us to quickly search the TagTree for entries using the full name as the key.
 *
 * \date 08-19-2006
 * \author Eran
 *
 */
class MyTreeItemData : public wxTreeItemData
{
public:
    /**
     * Constructor.
     * \param key The full name of an item as returned by TagNode::GetKey()
     */
    MyTreeItemData(const wxString& key) : m_key(key) { }

	/**
     * \return the key of the item
     */
    const wxString& GetKey() const { return m_key; }

private:
    wxString m_key;
};

/**
 * Class SymbolTree, a tree that contains the language symbols from SQLite database.
 *
 * \date 08-18-2006
 * \author Eran
 *
 */
class SymbolTree : public wxTreeCtrl
{
protected:
	std::map<wxString, int> m_imagesMap;
	TagTreePtr m_tree;
	std::map<wxString, wxTreeItemId> m_globalsMap;
	std::map<wxString, wxTreeItemId> m_prototypesMap;
	std::map<void*, bool> m_sortItems;
	std::map<wxString, bool> m_globalsKind;

public:
	/**
	 * Default cosntructor.
	 */
	SymbolTree();

	/**
	 * Parameterized constructor.
	 * \param parent Tree parent window
	 * \param id Window id
	 * \param pos Window position
	 * \param size Window size
	 * \param style Window style
	 */
	SymbolTree(wxWindow *parent, const wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
	
	/**
	 * Destructor .
	 */
	virtual ~SymbolTree(void);	

	/**
	 * Create tree, usually called after contructing SymbolTree with default constructor.
	 * \param parent Tree parent window
	 * \param id Window id
	 * \param pos Window position
	 * \param size Window size
	 * \param style Window style
	 */
	virtual void Create(wxWindow *parent, const wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
	
	/**
	 * Construct the gui tree from Tag Tree.
	 * If tree is NULL, the latest tree from TagsManagerST::Get()->Load() will be used.
	 */
	void BuildTree(TagTreePtr& tree);

	/**
	 * User provided icons for the symbols tree. 
	 * The assignment is index based, in the following order:
	 *
	 * -# project
	 * -# namespace 
	 * -# globals 
	 * -# class 
	 * -# function 
	 * -# prototype_public 
	 * -# prototype_protected
	 * -# prototype_private 
	 * -# member_public 
	 * -# member_protected 
	 * -# member_private 
	 * -# typedef 
	 * -# macro
	 * -# enum
	 *
	 * \note Due to ctags limitation, only 'function' is mentioned here, there is no function_protected, function_private, etc
	 * since when coming to implementation (not prototypes!), all functions will receive 'public' icon.    
	 * \param images Image list (allocated on the heap), this class becomes the owner of this image list
	 */
	virtual void SetSymbolsImages(wxImageList *images)
	{
		AssignImageList(images);
	};

protected:

	/**
	 * Add an item to the gui tree.
	 * \param node Node to add 
	 */
	void AddItem(TagNode* node);

	/**
	 * Return the icon index according to item kind and access.
	 * \param kind Item kind (class, namespace etc)
	 * \param access One of $public$, $private$, $protected$, or $wxEmptyString$
	 * \return icon index
	 */
	int GetItemIconIndex(const wxString &kind, const wxString &access = wxEmptyString);

	/**
	 * Initialise the image: icon index map.
	 */
	void InitialiseSymbolMap();

	/**
	 * Sort the tree.
	 * \param nodes Vector of nodes to sort
	 */
	void SortTree(std::map<void*, bool> & nodes);

	/**
	 * Override this function in the derived class to change the sort order of the items in the tree control. 
	 * \param item1 Item one
	 * \param item2 Item two
	 * \return The function should return a negative, zero or positive value 
	 * if the first item is less than, equal to or greater than the second one.
	 */
	int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);

	/**
	 * Update gui item with new data
	 * \param data new data 
	 * \param key node key
	 */
	void UpdateGuiItem(TagEntry& data, const wxString& key);

    DECLARE_EVENT_TABLE()
	void OnAddSymbols(SymbolTreeEvent& event);
	void OnDeleteSymbols(SymbolTreeEvent& event);
	void OnUpdateSymbols(SymbolTreeEvent& event);
	void OnDeleteProject(SymbolTreeEvent& event);
	DECLARE_DYNAMIC_CLASS(SymbolTree)
};

} // codelite

#endif // CODELITE_SYMBOL_TREE_H
