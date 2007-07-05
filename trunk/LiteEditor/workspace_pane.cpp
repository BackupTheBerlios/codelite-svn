#include "workspace_pane.h"
#include "fileview.h"
#include "cpp_symbol_tree.h"
#include <wx/xrc/xmlres.h>
#include "frame.h"
#include "wx/choicebk.h"

const wxString WorkspacePane::SYMBOL_VIEW = wxT("Outline");
const wxString WorkspacePane::FILE_VIEW   = wxT("Files");

extern wxImageList* CreateSymbolTreeImages();

WorkspacePane::WorkspacePane(wxWindow *parent, const wxString &caption)
: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(300, 400))
, m_caption(caption)
{
	CreateGUIControls();	
}


WorkspacePane::~WorkspacePane()
{

}

int WorkspacePane::CaptionToIndex(const wxString &caption)
{
	int i = 0;
	for(; i<m_book->GetPageCount(); i++)
	{
		if(m_book->GetPageText((size_t)i) == caption)
			return i;
	}
	return wxNOT_FOUND;
}

void WorkspacePane::CreateGUIControls()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);

	long style = wxFNB_NO_X_BUTTON | wxFNB_NO_NAV_BUTTONS | wxFNB_DROPDOWN_TABS_LIST | wxFNB_FF2 | wxFNB_BACKGROUND_GRADIENT | wxFNB_CUSTOM_DLG | wxFNB_TABS_BORDER_SIMPLE; 
	m_book = new wxFlatNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	m_book->SetCustomizeOptions(wxFNB_CUSTOM_LOCAL_DRAG | wxFNB_CUSTOM_ORIENTATION | wxFNB_CUSTOM_TAB_LOOK);
	mainSizer->Add(m_book, 1, wxEXPAND | wxALL, 1);
	
	m_images.Add(wxXmlResource::Get()->LoadBitmap(wxT("file_view")));
	m_images.Add(wxXmlResource::Get()->LoadBitmap(wxT("class_view")));
	m_book->SetImageList( &m_images );

	// Add the class view tree
	m_treeBook = new wxChoicebook(m_book, wxID_ANY);
	m_book->AddPage(m_treeBook, WorkspacePane::SYMBOL_VIEW, false, 1);

	m_fileView = new FileViewTree(m_book, wxID_ANY);
	m_book->AddPage(m_fileView, WorkspacePane::FILE_VIEW, true, 0);
}

CppSymbolTree *WorkspacePane::GetTreeByFilename(const wxFileName &filename)
{
	size_t count = m_treeBook->GetPageCount();
	//found the relevant page
	for(size_t i=0; i<count; i++)
	{
		CppSymbolTree *tree = dynamic_cast<CppSymbolTree *>(m_treeBook->GetPage(i));
		if(tree)
		{
			if(tree->GetFilename() == filename)
				return tree;
		} // if(tree)
	} // for(size_t i=0; i<count; i++)
	return NULL;
}

void WorkspacePane::BuildSymbolTree(const wxFileName &filename)
{
	CppSymbolTree *tree = GetTreeByFilename(filename);
	if(!tree)
	{
		tree = new CppSymbolTree(m_treeBook, wxID_ANY);
		tree->SetSymbolsImages(CreateSymbolTreeImages());
		m_treeBook->AddPage(tree, filename.GetFullName(), true);
	} // if(!tree)
	tree->BuildTree(filename);
}

void WorkspacePane::BuildFileTree()
{
	m_fileView->BuildTree();
}

SymbolTree *WorkspacePane::GetSymbolTree()
{
	int id = Frame::Get()->GetNotebook()->GetSelection();
	if(id != wxNOT_FOUND)
	{
		LEditor *editor = dynamic_cast<LEditor*>( Frame::Get()->GetNotebook()->GetPage((size_t)id));
		if(editor)
		{
			return GetTreeByFilename(editor->GetFileName());
		} // if(editor)
	} // if(id != wxNOT_FOUND)
	return NULL;
}

void WorkspacePane::DisplaySymbolTree(const wxFileName &filename)
{
	size_t count = m_treeBook->GetPageCount();
	//found the relevant page
	for(size_t i=0; i<count; i++)
	{
		CppSymbolTree *tree = dynamic_cast<CppSymbolTree *>(m_treeBook->GetPage(i));
		if(tree)
		{
			if(tree->GetFilename() == filename)
			{
				m_treeBook->SetSelection(i);
				return;
			} // if(tree->GetFilename() == filename)
		} // if(tree)
	} // for(size_t i=0; i<count; i++)
}

void WorkspacePane::DeleteSymbolTree(const wxFileName &filename)
{
	size_t count = m_treeBook->GetPageCount();
	//found the relevant page
	for(size_t i=0; i<count; i++)
	{
		CppSymbolTree *tree = dynamic_cast<CppSymbolTree *>(m_treeBook->GetPage(i));
		if(tree)
		{
			if(tree->GetFilename() == filename)
			{
				m_treeBook->DeletePage(i);
				return;
			} // if(tree->GetFilename() == filename)
		} // if(tree)
	} // for(size_t i=0; i<count; i++)
}
