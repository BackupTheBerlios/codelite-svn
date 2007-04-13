#include "workspace_pane.h"

const wxString WorkspacePane::SYMBOL_VIEW = wxT("Symbols");
const wxString WorkspacePane::FILE_VIEW   = wxT("Files");

extern wxImageList* CreateSymbolTreeImages();

WorkspacePane::WorkspacePane(wxWindow *parent, const wxString &caption)
: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(300, -1))
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

	long style = wxFNB_NO_X_BUTTON | wxFNB_NO_NAV_BUTTONS | wxFNB_DROPDOWN_TABS_LIST | wxFNB_BOTTOM; 
	m_book = new wxFlatNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	mainSizer->Add(m_book, 1, wxEXPAND | wxALL, 1);

	// Add the class view tree
	m_tree = new CppSymbolTree(m_book, wxID_ANY);
	m_book->AddPage(m_tree, WorkspacePane::SYMBOL_VIEW, true);

	m_fileView = new FileViewTree(m_book, wxID_ANY);
	m_book->AddPage(m_fileView, WorkspacePane::FILE_VIEW);

	// Set the images for the symbols
	m_tree->SetSymbolsImages( CreateSymbolTreeImages() );
}

void WorkspacePane::BuildSymbolTree(TagTreePtr tree)
{
	m_tree->BuildTree( tree );
}

void WorkspacePane::BuildFileTree()
{
	m_fileView->BuildTree();
}
