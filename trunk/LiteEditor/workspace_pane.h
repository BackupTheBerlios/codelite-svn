#ifndef WORKSPACE_PANE_H
#define WORKSPACE_PANE_H

#include "wx/panel.h"
#include "tag_tree.h"
#include "wx/wxFlatNotebook/wxFlatNotebook.h"
#include "wx/filename.h"

class FileViewTree;
class SymbolTree;
class wxChoicebook;
class CppSymbolTree;

class WorkspacePane : public wxPanel 
{
public:
	static const wxString SYMBOL_VIEW;
	static const wxString FILE_VIEW;

	wxFlatNotebook *m_book;
	wxString m_caption;
	FileViewTree *m_fileView;
	wxFlatNotebookImageList m_images;
	wxChoicebook *m_treeBook;

private:
	void CreateGUIControls();
	CppSymbolTree *GetTreeByFilename(const wxFileName &filename);

public:
	WorkspacePane(wxWindow *parent, const wxString &caption);
	virtual ~WorkspacePane();
	
	//-----------------------------------------------
	// Operations
	//-----------------------------------------------
	void BuildSymbolTree(const wxFileName &filename);
	void BuildFileTree();
	void DisplaySymbolTree(const wxFileName &filename);
	void DeleteSymbolTree(const wxFileName &filename);
	void DeleteAllSymbolTrees();

	// Return the index of the given tab by name
	int CaptionToIndex(const wxString &caption);

	//-----------------------------------------------
	// Setters/Getters
	//-----------------------------------------------
	wxFlatNotebook *GetNotebook() { return m_book; }
	SymbolTree *GetSymbolTree();
	FileViewTree *GetFileViewTree() { return m_fileView; }
	const wxString &GetCaption() const { return m_caption; }
};

#endif // WORKSPACE_PANE_H

