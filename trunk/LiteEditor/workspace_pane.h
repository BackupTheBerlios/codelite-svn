#ifndef WORKSPACE_PANE_H
#define WORKSPACE_PANE_H

#include "wx/panel.h"
#include "tag_tree.h"
#include "wx/wxFlatNotebook/wxFlatNotebook.h"

class FileViewTree;
class SymbolTree;

class WorkspacePane : public wxPanel 
{
public:
	static const wxString SYMBOL_VIEW;
	static const wxString FILE_VIEW;

	wxFlatNotebook *m_book;
	wxString m_caption;
	FileViewTree *m_fileView;
	SymbolTree* m_tree;
	wxFlatNotebookImageList m_images;

private:
	void CreateGUIControls();
	int CaptionToIndex(const wxString &caption);

public:
	WorkspacePane(wxWindow *parent, const wxString &caption);
	virtual ~WorkspacePane();
	
	//-----------------------------------------------
	// Operations
	//-----------------------------------------------
	void BuildSymbolTree(TagTreePtr tree);
	void BuildFileTree();

	//-----------------------------------------------
	// Setters/Getters
	//-----------------------------------------------
	wxFlatNotebook *GetNotebook() { return m_book; }
	SymbolTree *GetSymbolTree() { return m_tree; }
	FileViewTree *GetFileViewTree() { return m_fileView; }
	const wxString &GetCaption() const { return m_caption; }
};

#endif // WORKSPACE_PANE_H

