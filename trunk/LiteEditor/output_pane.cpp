#include "output_pane.h"
#include <wx/xrc/xmlres.h>
#include "wx/wxFlatNotebook/wxFlatNotebook.h"
#include "manager.h"
#include "regex_processor.h"
#include "build_settings_config.h"
#include "dirsaver.h"

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

void OutputPane::OnPaint(wxPaintEvent &event){
	wxPaintDC dc(this);
	event.Skip();
}

OutputPane::~OutputPane()
{

}

void OutputPane::CreateGUIControls()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);

	wxToolBar *tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);

	int id = wxNewId();
	tb->AddTool(id, 
				wxT("Clear All"), 
				wxXmlResource::Get()->LoadBitmap(wxT("document_delete")), 
				wxT("Clear All"));
	Connect( id, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( OutputPane::OnClearAll ));

	id = wxNewId();
	tb->AddTool(id, 
				wxT("Word Wrap"), 
				wxXmlResource::Get()->LoadBitmap(wxT("word_wrap")), 
				wxT("Word Wrap"));
	Connect( id, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( OutputPane::OnWordWrap ));
	tb->Realize(); 
	// Connect handlers
	

	mainSizer->Add(tb, 0, wxEXPAND | wxALL, 1);

	long style = wxFNB_NO_X_BUTTON | wxFNB_NO_NAV_BUTTONS | wxFNB_DROPDOWN_TABS_LIST | wxFNB_FF2 | wxFNB_CUSTOM_DLG | wxFNB_BACKGROUND_GRADIENT | wxFNB_TABS_BORDER_SIMPLE; 
	m_book = new wxFlatNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	m_book->SetCustomizeOptions(wxFNB_CUSTOM_LOCAL_DRAG | wxFNB_CUSTOM_ORIENTATION | wxFNB_CUSTOM_TAB_LOOK);
	mainSizer->Add(m_book, 1, wxEXPAND | wxALL | wxGROW, 1);

	m_images.Add(wxXmlResource::Get()->LoadBitmap(wxT("find_results")));
	m_images.Add(wxXmlResource::Get()->LoadBitmap(wxT("build")));
	m_book->SetImageList( &m_images );

	// Create the 'Find In Files Window'
	wxScintilla *findInFilesWin = CreateScintillaPage();
	wxScintilla *buildWin = CreateScintillaPage();

	m_book->AddPage(findInFilesWin, FIND_IN_FILES_WIN, true, 0);
	m_book->AddPage(buildWin, BUILD_WIN, false, 1);

	mainSizer->Fit(this);
	mainSizer->Layout();
}

wxScintilla *OutputPane::CreateScintillaPage()
{
	wxScintilla *win = new wxScintilla(m_book, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	// Hide margins
	win->SetMarginWidth(0, 0);
	win->SetMarginWidth(1, 0);
	win->SetMarginWidth(2, 0);
	win->SetReadOnly(true);
	win->Connect(wxID_ANY, wxEVT_SET_FOCUS, wxFocusEventHandler(OutputPane::OnSetFocus), NULL, this);
	win->Connect(wxID_ANY, wxEVT_SCI_DOUBLECLICK, wxScintillaEventHandler(OutputPane::OnMouseDClick), NULL, this);
	wxFont font(8, wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
	win->StyleSetFont(wxSCI_STYLE_DEFAULT, font);
	return win;
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
	event.Skip();
}

void OutputPane::OnClearAll(wxCommandEvent &event)
{
	wxUnusedVar(event);
	Clear();
}

void OutputPane::OnWordWrap(wxCommandEvent &event)
{
	wxUnusedVar(event);
	int index = m_book->GetSelection();
	if( index == wxNOT_FOUND )
		return;

	wxScintilla *win = dynamic_cast<wxScintilla*>(m_book->GetPage((size_t)index));
	if(win){
		if(win->GetWrapMode() == wxSCI_WRAP_WORD){
			win->SetWrapMode(wxSCI_WRAP_NONE);
		}else{
			win->SetWrapMode(wxSCI_WRAP_WORD);
		}
	}
}


void OutputPane::AppendText(const wxString &winName, const wxString &text)
{
	int index = CaptionToIndex(winName);
	if( index == wxNOT_FOUND )
		return;
	
	if(index != m_book->GetSelection()){
		m_book->SetSelection((size_t)index);
	}

	wxScintilla *win = dynamic_cast<wxScintilla*>(m_book->GetPage((size_t)index));
	if( win ){
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
	}
}

void OutputPane::Clear()
{
	int index = m_book->GetSelection();
	if( index == wxNOT_FOUND )
		return;

	wxScintilla *win = dynamic_cast<wxScintilla*>(m_book->GetPage((size_t)index));
	if(win){
		win->SetReadOnly(false);
		win->ClearAll();
		win->SetReadOnly(true);
	}
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
	int fifWinIndex = CaptionToIndex(OutputPane::FIND_IN_FILES_WIN);
	int buildWinIndex = CaptionToIndex(OutputPane::BUILD_WIN);

	wxScintilla *win = dynamic_cast<wxScintilla*>(m_book->GetPage(m_book->GetSelection()));
	if( !win ){
		return;
	}

	int line = win->LineFromPosition(pos);
	wxString lineText = win->GetLine(line);

	//remove selection
	win->SetSelectionStart(pos);
	win->SetSelectionEnd(pos);
	if( fifWinIndex == m_book->GetSelection() ){
		//Find in files
		OnFindInFilesDClick(lineText);
	} else if(buildWinIndex == m_book->GetSelection()){
		//build window
		OnBuildWindowDClick(lineText);
	}
}

void OutputPane::OnFindInFilesDClick(const wxString &line)
{
	// each line has the format of 
	// file(line, col): text
	wxString fileName = line.BeforeFirst(wxT('('));
	wxString strLineNumber = line.AfterFirst(wxT('('));
	strLineNumber = strLineNumber.BeforeFirst(wxT(','));
	strLineNumber = strLineNumber.Trim();
	long lineNumber = -1;
	strLineNumber.ToLong(&lineNumber);

	// open the file in the editor
	ManagerST::Get()->OpenFile(fileName, wxEmptyString, lineNumber - 1 );
}

void OutputPane::OnBuildWindowDClick(const wxString &line)
{
	wxString fileName, strLineNumber;
	bool match = false;

	//get the selected compiler
	BuildConfigPtr bldConf = ManagerST::Get()->GetActiveProjectBuildConf();
	wxString cmpType = bldConf->GetCompilerType();
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);

	long idx;
	//try to match an error pattern to the line
	RegexProcessor re(cmp->GetErrPattern());
	cmp->GetErrFileNameIndex().ToLong(&idx);
	if(re.GetGroup(line, idx, fileName)){
		//we found the file name, get the line number
		cmp->GetErrLineNumberIndex().ToLong(&idx);
		re.GetGroup(line, idx, strLineNumber);
		match = true;
	}
	//try to match warning pattern
	if(!match){
		RegexProcessor re(cmp->GetWarnPattern());
		cmp->GetWarnFileNameIndex().ToLong(&idx);
		if(re.GetGroup(line, idx, fileName)){
			//we found the file name, get the line number
			cmp->GetWarnLineNumberIndex().ToLong(&idx);
			re.GetGroup(line, idx, strLineNumber);
			match = true;
		}
	}

	if(match){
		long lineNumber = -1;
		strLineNumber.ToLong(&lineNumber);

		// open the file in the editor
		DirSaver ds;
		::wxSetWorkingDirectory(ManagerST::Get()->GetProject(ManagerST::Get()->GetActiveProjectName())->GetFileName().GetPath());

		ManagerST::Get()->OpenFile(fileName, wxEmptyString, lineNumber - 1 );
	}
}
