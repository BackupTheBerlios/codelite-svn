#include "output_pane.h"
#include "flat_menu_bar.h"
#include <wx/xrc/xmlres.h>
#include "wx/wxFlatNotebook/wxFlatNotebook.h"
#include "manager.h"

#ifndef wxScintillaEventHandler
#define wxScintillaEventHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxScintillaEventFunction, &func)
#endif

const wxString OutputPane::FIND_IN_FILES_WIN = wxT("Find Results");
const wxString OutputPane::BUILD_WIN = wxT("Build");

OutputPane::OutputPane(wxWindow *parent, const wxString &caption)
: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(400, 300))
, m_caption(caption)
, m_canFocus(true)
{
	CreateGUIControls();	
}


OutputPane::~OutputPane()
{

}

void OutputPane::CreateGUIControls()
{
	wxArtManagerST::Get()->SetMenuTheme( StyleXP );
	wxArtManagerST::Get()->SetRaiseToolbar( false );
	wxArtManagerST::Get()->DrawMenuBarBorder( false );
	wxArtManagerST::Get()->SetMenuBarColour( wxT("Default") );
	wxArtManagerST::Get()->SetMBVerticalGradient(true);

	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);

	wxFlatMenuBar *mb = new wxFlatMenuBar(this, wxID_ANY, true, SmallIcons);
	mb->SetSize(wxSize(-1, 28));
	
	wxBitmap bmp = wxXmlResource::Get()->LoadBitmap(_T("document_delete"));

	wxFlatToolbarItem *tool = new wxFlatToolbarItem(bmp, wxNewId(), wxT("Clear All"));
	mb->AppendToolbarItem(tool);

	// Connect handlers
	Connect( tool->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( OutputPane::OnClearAll ));

	mainSizer->Add(mb, 0, wxGROW | wxALL, 1);

	long style = wxFNB_NO_X_BUTTON | wxFNB_NO_NAV_BUTTONS | wxFNB_DROPDOWN_TABS_LIST | wxFNB_BOTTOM | wxFNB_FF2 | wxFNB_CUSTOM_DLG; 
	m_book = new wxFlatNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	mainSizer->Add(m_book, 1, wxEXPAND | wxALL | wxGROW, 1);

	// Create the 'Find In Files Window'
	wxScintilla *findInFilesWin = new wxScintilla(m_book, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	// Hide margins
	findInFilesWin->SetMarginWidth(0, 0);
	findInFilesWin->SetMarginWidth(1, 0);
	findInFilesWin->SetMarginWidth(2, 0);

	m_book->AddPage(findInFilesWin, FIND_IN_FILES_WIN, true);
	findInFilesWin->SetReadOnly(true);
		
	findInFilesWin->Connect(wxID_ANY, wxEVT_SET_FOCUS, wxFocusEventHandler(OutputPane::OnSetFocus), NULL, this);
	findInFilesWin->Connect(wxID_ANY, wxEVT_SCI_DOUBLECLICK, wxScintillaEventHandler(OutputPane::OnMouseDClick), NULL, this);

	wxFont font(8, wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
	findInFilesWin->StyleSetFont(wxSCI_STYLE_DEFAULT, font);
	mainSizer->Fit(this);
	mainSizer->Layout();
}

void OutputPane::OnSetFocus(wxFocusEvent &event)
{
	if( m_canFocus ){
		return;
	}

	wxWindow *prevFocusWin = event.GetWindow();
	if( prevFocusWin ){
		prevFocusWin->SetFocus();
	}
}

void OutputPane::OnClearAll(wxCommandEvent &event)
{
	wxUnusedVar(event);
	Clear();
}

void OutputPane::AppendText(const wxString &winName, const wxString &text)
{
	int index = CaptionToIndex(winName);
	if( index == wxNOT_FOUND )
		return;
	
	m_book->SetSelection((size_t)index);
	switch( index )
	{
	case 0:	// Find In Files
		{
			wxScintilla *win = static_cast<wxScintilla*>(m_book->GetPage((size_t)index));
			// enable writing
			win->SetReadOnly(false);					
			// add the text
			win->AddText( text );						
			// the next 4 lines make sure that the caret is at last line
			// and is visible
			win->SetSelectionEnd(win->GetLength());
			win->SetSelectionStart(win->GetLength());
			win->SetCurrentPos(win->GetLength());
			win->EnsureCaretVisible();
			// enable readonly mode 
			win->SetReadOnly(true);	
			
			break;
		}
	default:
		break;
	}
}

void OutputPane::Clear()
{
	int index = CaptionToIndex(OutputPane::FIND_IN_FILES_WIN);
	if( index == wxNOT_FOUND )
		return;

	wxScintilla *win = static_cast<wxScintilla*>(m_book->GetPage((size_t)index));

	win->SetReadOnly(false);
	win->ClearAll();
	win->SetReadOnly(true);
}

int OutputPane::CaptionToIndex(const wxString &caption)
{
	int i = 0;
	for(; i<m_book->GetPageCount(); i++)
	{
		if(m_book->GetPageText((size_t)i) == caption)
			return i;
	}
	return wxNOT_FOUND;
}

void OutputPane::OnMouseDClick(wxScintillaEvent &event)
{
	long pos = event.GetPosition();
	int index = CaptionToIndex(OutputPane::FIND_IN_FILES_WIN);
	if( index == wxNOT_FOUND )
		return;

	wxScintilla *win = dynamic_cast<wxScintilla*>(m_book->GetPage((size_t)index));
	if( !win ){
		return;
	}

	int line = win->LineFromPosition(pos);
	wxString lineText = win->GetLine(line);

	win->SetSelectionStart(pos);
	win->SetSelectionEnd(pos);

	// each line has the format of 
	// file(line, col): text
	wxString fileName = lineText.BeforeFirst(wxT('('));
	wxString strLineNumber = lineText.AfterFirst(wxT('('));
	strLineNumber = strLineNumber.BeforeFirst(wxT(','));
	strLineNumber = strLineNumber.Trim();
	long lineNumber = -1;
	strLineNumber.ToLong(&lineNumber);

	// open the file in the editor
	ManagerST::Get()->OpenFile(fileName, wxEmptyString, lineNumber - 1 );
}


