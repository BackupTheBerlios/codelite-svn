#include "editor.h"
#include <wx/xrc/xmlres.h>
#include <wx/ffile.h> 
#include <wx/tooltip.h>
#include <wx/settings.h>
#include "parse_thread.h"
#include "ctags_manager.h"
#include "manager.h"
#include "menumanager.h"
#include <wx/fdrepdlg.h>
#include "findreplacedlg.h"
#include <wx/wxFlatNotebook/renderer.h>
#include "context_manager.h"
#include "editor_config.h"
#include "filedroptarget.h"
#include "fileutils.h"

// fix bug in wxscintilla.h
#ifdef EVT_SCI_CALLTIP_CLICK
#undef EVT_SCI_CALLTIP_CLICK
#define EVT_SCI_CALLTIP_CLICK(id, fn)          DECLARE_EVENT_TABLE_ENTRY (wxEVT_SCI_CALLTIP_CLICK,          id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxScintillaEventFunction, & fn ), (wxObject *) NULL),
#endif

BEGIN_EVENT_TABLE(LEditor, wxScintilla)
EVT_SCI_CHARADDED(wxID_ANY, LEditor::OnCharAdded)
EVT_SCI_MARGINCLICK(wxID_ANY, LEditor::OnMarginClick)
EVT_SCI_DWELLSTART(wxID_ANY, LEditor::OnDwellStart)
EVT_SCI_CALLTIP_CLICK(wxID_ANY, LEditor::OnCallTipClick)
EVT_SCI_DWELLEND(wxID_ANY, LEditor::OnDwellEnd)
EVT_SCI_MODIFIED(wxID_ANY, LEditor::OnModified)
EVT_SCI_UPDATEUI(wxID_ANY, LEditor::OnSciUpdateUI)
EVT_CONTEXT_MENU(LEditor::OnContextMenu)

// Find and replace dialog
EVT_COMMAND(wxID_ANY, wxEVT_FRD_FIND_NEXT, LEditor::OnFindDialog)
EVT_COMMAND(wxID_ANY, wxEVT_FRD_REPLACE, LEditor::OnFindDialog)
EVT_COMMAND(wxID_ANY, wxEVT_FRD_REPLACEALL, LEditor::OnFindDialog)
EVT_COMMAND(wxID_ANY, wxEVT_FRD_BOOKMARKALL, LEditor::OnFindDialog)
EVT_COMMAND(wxID_ANY, wxEVT_FRD_CLOSE, LEditor::OnFindDialog)

END_EVENT_TABLE()

// Instantiate statics
std::stack<TagEntry> LEditor::m_history;
FindReplaceDialog* LEditor::m_findReplaceDlg = NULL;
FindReplaceData LEditor::m_findReplaceData;
std::map<wxString, int> LEditor::ms_bookmarkShapes;

LEditor::LEditor(wxWindow* parent, wxWindowID id, const wxSize& size, const wxString& fileName, const wxString& project, bool hidden)
: wxScintilla(parent, id, wxDefaultPosition, size)
, m_fileName(fileName)
, m_project(project)
, m_rightClickMenu(NULL)
, m_lastMatchPos(0)
{
	Show(!hidden);

	ms_bookmarkShapes[wxT("Small Rectangle")] = wxSCI_MARK_SMALLRECT;
	ms_bookmarkShapes[wxT("Rounded Rectangle")] = wxSCI_MARK_ROUNDRECT;
	ms_bookmarkShapes[wxT("Small Arrow")] = wxSCI_MARK_ARROW;
	ms_bookmarkShapes[wxT("Circle")] = wxSCI_MARK_CIRCLE;

	RestoreDefaults();
	m_fileName.MakeAbsolute();

	// If file name is provided, open it
	wxString tmpFilename(m_fileName.GetName());
	if(	!tmpFilename.IsEmpty() ){
		OpenFile(m_fileName.GetFullPath(), m_project);
	}
	SetDropTarget(new FileDropTarget());
}

void LEditor::RestoreDefaults()
{
	SetSyntaxHighlight();
	SetProperties();
}

void LEditor::SetSyntaxHighlight()
{
	m_context = ManagerST::Get()->NewContextByFileName(m_fileName, this);
}

LEditor::~LEditor()
{
	if(m_rightClickMenu)
		delete m_rightClickMenu;
}

/// Setup some scintilla properties
void LEditor::SetProperties()
{
	//load menu
	if(m_rightClickMenu)
		delete m_rightClickMenu;
	m_rightClickMenu = wxXmlResource::Get()->LoadMenu(wxT("editor_right_click"));
	
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();

	SetMouseDwellTime(200);
	SetProperty(wxT("fold"), wxT("1"));
	SetProperty(wxT("fold.html"), wxT("1"));
	SetProperty(wxT("styling.within.preprocessor"), wxT("1"));

	// Fold and comments as well
	SetProperty(wxT("fold.comment"), wxT("1"));

	//Set the selection color to grey/black as default
#ifdef __WXMSW__
	SetSelBackground(true, wxT("BLACK"));
	SetSelForeground(true, wxColor(0x00, 0x00, 0x00));
#endif
	SetModEventMask (wxSCI_MOD_DELETETEXT | wxSCI_MOD_INSERTTEXT  | wxSCI_PERFORMED_UNDO  | wxSCI_PERFORMED_REDO );

	int caretSlop = 1;
	int caretZone = 20;
	int caretStrict = 0;
	int caretEven = 0;
	int caretJumps = 0;

	SetXCaretPolicy(caretStrict | caretSlop | caretEven | caretJumps, caretZone);

	caretSlop = 1;
	caretZone = 1;
	caretStrict = 4;
	caretEven = 8;
	caretJumps = 0;
	SetYCaretPolicy(caretStrict | caretSlop | caretEven | caretJumps, caretZone);
	SetCaretWidth(1);
	SetMarginLeft(1);
	SetMarginRight(0);

#ifdef __WXMSW__
	SetCaretLineBackgroundAlpha(30);
	SetSelAlpha(70);
#endif
	// Mark current line
	SetCaretLineVisible(options->GetHighlightCaretLine());
#ifdef __WXMSW__
	SetCaretLineBackground(wxT("BLUE"));
#else
	SetCaretLineBackground(wxColour(255, 255, 220));
#endif // __WXMSW__

	SetFoldFlags(options->GetUnderlineFoldLine() ? 16 : 0);

	//------------------------------------------
	// Margin settings
	//------------------------------------------
	
	// symbol margin
	SetMarginType(0, wxSCI_MARGIN_SYMBOL);
	// Line numbes
	SetMarginType(1, wxSCI_MARGIN_NUMBER);
	// line number margin displays every thing but folding & bookmarks (256)
	SetMarginMask(1, ~(256 | wxSCI_MASK_FOLDERS));

	// Separator
	SetMarginType(2, wxSCI_MARGIN_FORE);

	// Fold margin - allow only folder symbols to display
	SetMarginMask(3, wxSCI_MASK_FOLDERS);

	// Set margins' width
	SetMarginWidth(0, options->GetDisplayBookmarkMargin() ? 16 : 0);	// Symbol margin
	// allow everything except for the folding symbols
	SetMarginMask(0, ~(wxSCI_MASK_FOLDERS));

	// Line number margin
	int pixelWidth = 4 + 6*TextWidth(wxSCI_STYLE_LINENUMBER, wxT("9"));
	SetMarginWidth(1, options->GetDisplayLineNumbers() ? pixelWidth : 0);

	SetMarginWidth(2, 1);	// Symbol margin which acts as separator

	int ww = options->GetDisplayFoldMargin() ? 16 : 0;
	SetMarginWidth(3, ww);	// Fold margin
	StyleSetForeground(wxSCI_STYLE_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW));
	
	// Mark fold margin as sensetive
	SetMarginSensitive(3, true);

	//---------------------------------------------------
	// Fold settings
	//---------------------------------------------------
	// Define the folding style to be square
	if( options->GetFoldStyle() == wxT("Flatten Tree Square Headers") ){

		DefineMarker(wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_BOXMINUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDER, wxSCI_MARK_BOXPLUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_VLINE, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_LCORNER, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_BOXPLUSCONNECTED, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_BOXMINUSCONNECTED, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_TCORNER, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));

	} else if( options->GetFoldStyle() == wxT("Flatten Tree Circular Headers") ){

		DefineMarker(wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_CIRCLEMINUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDER, wxSCI_MARK_CIRCLEPLUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_VLINE, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_LCORNERCURVE, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_CIRCLEPLUSCONNECTED, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_CIRCLEMINUSCONNECTED, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_TCORNER, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));

	} else if( options->GetFoldStyle() == wxT("Simple") ){

		DefineMarker(wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_MINUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDER, wxSCI_MARK_PLUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_PLUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_MINUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));

	} else if( options->GetFoldStyle() == wxT("Arrows") ){

		DefineMarker(wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_ARROWDOWN, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDER, wxSCI_MARK_ARROW, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_ARROW, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_ARROWDOWN, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));

	}

	// Bookmark
	int marker = wxSCI_MARK_ARROW;
	std::map<wxString, int>::iterator iter = ms_bookmarkShapes.find(options->GetBookmarkShape());
	if( iter != ms_bookmarkShapes.end() ){
		marker = iter->second;
	}

	MarkerDefine(0x8, marker);
	MarkerSetBackground(0x8, options->GetBookmarkBgColour());
	MarkerSetForeground(0x8, options->GetBookmarkFgColour());

	// Margin colours
	SetFoldMarginColour(true, wxT("WHITE"));
	SetFoldMarginHiColour(true, wxT("WHITE"));
	
	// calltip settings
    CallTipSetBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
	CallTipSetForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
	
	//---------------------------------------------------
	// Set autocompletion parameters
	//---------------------------------------------------

	AutoCompSetSeparator(static_cast<int>('@'));	// set the separator to be non valid language wxChar
	AutoCompSetChooseSingle(true);					// If only one match, insert it automatically
	AutoCompSetIgnoreCase(true);
	AutoCompSetMaxWidth(0);
	//AutoCompSetFillUps(wxT(":.(- "));

	SetTabWidth(8);
	SetIndent(8);

	StyleSetBold(wxSCI_STYLE_BRACELIGHT, true);
	
	StyleSetBackground(wxSCI_STYLE_LINENUMBER, wxT("WHITE"));
	StyleSetForeground(wxSCI_STYLE_LINENUMBER, wxColour(0, 63, 125));

	// Indentation guidelines
	SetIndentationGuides(options->GetShowIndentationGuidelines());
}

void LEditor::SetDirty(bool dirty)
{
	if( dirty ){
		if( !ManagerST::Get()->GetPageTitle(this).StartsWith(wxT("*")) ){
			ManagerST::Get()->SetPageTitle(this, wxT("*") + ManagerST::Get()->GetPageTitle(this));
		}
	} else {
		ManagerST::Get()->SetPageTitle(this, GetFileName().GetFullName());
	}
}

void LEditor::OnCharAdded(wxScintillaEvent& event)
{
	// set the page title as dirty
	SetDirty(true);
	
	// make sure line is visible
	int curLine = LineFromPosition(GetCurrentPos());
	if( !GetFoldExpanded(curLine) ) {
		ToggleFold(curLine);
	}

	// Always do auto-indentation
	if(event.GetKey() == ':' || event.GetKey() == '}' || event.GetKey() == '\n')
		m_context->AutoIndent(event.GetKey());

	if(false == GetProjectName().IsEmpty())
	{
		switch( event.GetKey() )
		{
		case ':': 
		case '.':
		case '>':
		case '(':
			CodeComplete();
			break;
		case ')':
			{
				m_context->CallTipCancel();
				break;
			}
		case '\n':
			{
				// incase we are typing in a folded line, make sure it is visible
				int  nLineNumber = LineFromPosition(GetCurrentPos());
				EnsureVisible(nLineNumber+1);
				break;
			}
		default:
			break;
		}
	}
	event.Skip();
}

void LEditor::OnSciUpdateUI(wxScintillaEvent &event)
{
	wxUnusedVar(event);
}

void LEditor::OnMarginClick(wxScintillaEvent& event)
{
	int nLine = LineFromPosition(event.GetPosition());
	ToggleFold(nLine);
}

void LEditor::DefineMarker(int marker, int markerType, wxColor fore, wxColor back)
{
	MarkerDefine(marker, markerType);
	MarkerSetForeground(marker, fore);
	MarkerSetBackground(marker, back);
}

bool LEditor::SaveFile()
{
	if(GetFileName().GetFullName().Find(wxT("Untitled")) != -1 || GetFileName().GetFullName().IsEmpty())
	{
		return SaveFileAs();
	}

	// first save the file content
	if( !SaveToFile(m_fileName) )
		return false;

	if( GetProjectName().IsEmpty() )
		return true;

	//-------------------------------------------------------------------
	// Using the CodeParser library, enforces us to notify the parsing
	// thread once the file is saved. ctags accepts file name, so we
	// notify him once the file on the disk is changed, there is no
	// point in notifying the parsing thread on, for example, OnCharAdded
	// event, since the actual file on the disk was not modified
	//-------------------------------------------------------------------

	// Put a request on the parsing thread to update the GUI tree for this file
	wxFileName fn = TagsManagerST::Get()->GetDatabase()->GetDatabaseFileName();

	ParseRequest *req = new ParseRequest();
	req->dbfile = fn.GetFullPath();

	// Construct an absolute file name for ctags
	wxFileName absFile( m_fileName);
	absFile.MakeAbsolute();
	req->file = absFile.GetFullPath();
	req->project = m_project;

	ParseThreadST::Get()->Add(req);
	return true;
}

bool LEditor::SaveFileAs()
{
	// Prompt the user for a new file name
	const wxString ALL(_T("All Files (*.*)|*.*"));
	wxFileDialog *dlg = new wxFileDialog(this, wxT("Save As"), m_fileName.GetPath(), m_fileName.GetFullName(), ALL, 
											wxFD_SAVE | wxFD_OVERWRITE_PROMPT , 
											wxDefaultPosition);

	if (dlg->ShowModal() == wxID_OK)
	{
		// get the path
		wxFileName name(dlg->GetPath());
		if( !SaveToFile(name) )
		{
			wxMessageBox(wxT("Failed to save file"), wxT("Error"), wxOK | wxICON_ERROR);
			return false;
		}
		m_fileName = name;
		delete dlg;
		return true;
	}
	
	delete dlg;
	return false;
}

// an internal function that does the actual file writing to disk
bool LEditor::SaveToFile(const wxFileName &fileName)
{
	wxFFile file(fileName.GetFullPath().GetData(), _T("wb"));
	if(file.IsOpened() == false)
	{
		// Nothing to be done
		wxString msg = wxString::Format(wxT("Failed to open file %s"), fileName.GetFullPath().GetData());
		wxMessageBox( msg );
		return false;
	}

	file.Write(GetText());
	file.Close();
	SetSavePoint();

	// update the file name (remove the star from the file name)
	ManagerST::Get()->SetPageTitle(this, fileName.GetFullName());

	// Update context if needed
	if(fileName.GetExt() != m_fileName.GetExt()){
		// new context is required
		ClearDocumentStyle();
		m_context = ManagerST::Get()->NewContextByFileName(fileName, this);
		Colourise(0, wxSCI_INVALID_POSITION);
		SetProperties();
	}
	return true;
}

void LEditor::SetSyntaxHighlight(const wxString &lexerName) 
{
	ClearDocumentStyle();
	m_context = ContextManager::Get()->NewContext(this, lexerName);
	Colourise(0, wxSCI_INVALID_POSITION);
	SetProperties();
}

void LEditor::OpenFile(const wxString &fileName, const wxString &project)
{
	if(fileName.IsEmpty() == true)
		return;

	wxString text;
	FileUtils::ReadFileUTF8(fileName, text);
	SetText( text );

	// make sure user can not undo this operation
	EmptyUndoBuffer();

	// Keep the file name and the project
	m_fileName = fileName;
	m_project = project;
}

wxString LEditor::GetWordUnderCursor() 
{
	// Get the partial word that we have
	long pos = GetCurrentPos();
	long start = WordStartPosition(pos, true);
	long end   = WordEndPosition(pos, true);
	return GetTextRange(start, end);
}

//---------------------------------------------------------------------------
// Most of the functionality for this functionality
// is done in the Language & TagsManager objects, however,
// as you can see below, much work still needs to be done in the application
// layer (outside of the library) to provide the input arguments for
// the CodeParser library
//---------------------------------------------------------------------------
void LEditor::CompleteWord()
{
	m_context->CompleteWord();
}

//------------------------------------------------------------------
// AutoCompletion, by far the nicest feature of a modern IDE
// This function attempts to resolve the string to the left of
// the '.', '->' operator and to display a popup menu with
// list of possible matches
//------------------------------------------------------------------
void LEditor::CodeComplete()
{
	m_context->CodeComplete();
}


//----------------------------------------------------------------
// Demonstrate how to achieve symbol browsing using the CodeLite
// library, in addition we implements here a memory for allowing
// user to go back and forward
//----------------------------------------------------------------
void LEditor::GotoDefinition()
{
	m_context->GotoDefinition();
}

void LEditor::GotoPreviousDefintion()
{
	m_context->GotoPreviousDefintion();
}

void LEditor::OnDwellStart(wxScintillaEvent & event)
{
	m_context->OnDwellStart(event);
}

void LEditor::OnDwellEnd(wxScintillaEvent & event)
{
	m_context->OnDwellEnd(event);
}

void LEditor::OnCallTipClick(wxScintillaEvent& event)
{
	m_context->OnCallTipClick(event);
}

void LEditor::OnModified(wxScintillaEvent& event)
{
	if(event.GetModificationType() & wxSCI_MOD_INSERTTEXT) {
		SetDirty(true);
	}

	if(event.GetModificationType() & wxSCI_MOD_DELETETEXT) {
		SetDirty(true);
	}

	if(event.GetModificationType() & wxSCI_PERFORMED_UNDO) {
		if(GetModify() == false){
			SetDirty(false);
		}
	}

	if(event.GetModificationType() & wxSCI_PERFORMED_REDO) 
	{
		SetDirty(true);
	}
}

void LEditor::OnMenuCommand(wxCommandEvent &event)
{
	MenuEventHandlerPtr handler = MenuManager::Get()->GetHandler(event.GetId());
	
	if( !handler ) {
		// do nothing
	} else {
		handler->ProcessCommandEvent(this, event);
	}
}

void LEditor::OnUpdateUI(wxUpdateUIEvent &event)
{
	MenuEventHandlerPtr handler = MenuManager::Get()->GetHandler(event.GetId());

	if( !handler ) {
		// do nothing
	} else {
		handler->ProcessUpdateUIEvent(this, event);
	}
}

//-----------------------------------------------------------------------
// Misc functions
//-----------------------------------------------------------------------

wxChar LEditor::PreviousChar(const int& pos, int &foundPos)
{
	wxChar ch = 0;
	long curpos = PositionBefore( pos );
	if(curpos == 0)
	{
		foundPos = curpos;
		return ch;
	}

	while( true )
	{
		ch = GetCharAt( curpos );
		if(ch == _T('\t') || ch == _T(' ') || ch == _T('\r') || ch == _T('\v') || ch == _T('\n'))
		{
			long tmpPos = curpos;
			curpos = PositionBefore( curpos );
			if(curpos == 0 && tmpPos == curpos)
				break;
		}
		else
		{
			foundPos = curpos;
			return ch;
		}
	}
	foundPos = -1;
	return ch;
}

wxChar LEditor::NextChar( const int &pos, int &foundPos )
{
	wxChar ch = 0;
	long nextpos = pos;
	while( true )
	{
		if( nextpos == GetLength() )
			break; // eof
		ch = GetCharAt( nextpos );
		if(ch == _T('\t') || ch == _T(' ') || ch == _T('\r') || ch == _T('\v') || ch == _T('\n'))
		{
			nextpos = PositionAfter( nextpos );
			continue;
		}
		else
		{
			foundPos = nextpos;
			return ch;
		}
	}
	foundPos = -1;
	return ch;
}


int LEditor::FindString (const wxString &str, int flags, const bool down, long pos)
{
    // initialize direction
    if( down )
	{
        SetTargetStart (pos);
		SetTargetEnd(GetLength());
    }
	else
	{
		SetTargetStart (pos);
		SetTargetEnd(0);
    }
	SetSearchFlags(flags);

    // search string
    int _pos = SearchInTarget(str);
	if (_pos >= 0) return _pos;
	else return -1;
}

bool LEditor::MatchBraceBack(const wxChar& chCloseBrace, const long &pos, long &matchedPos)
{
	if(pos<=0)
		return false;

	wxChar chOpenBrace;

	switch(chCloseBrace)
	{
	case '}': chOpenBrace = '{'; break;
	case ')': chOpenBrace = '('; break;
	case ']': chOpenBrace = '['; break;
	case '>': chOpenBrace = '<'; break;
	default: return false;
	}

	long nPrevPos = pos;
	wxChar ch;
	int depth = 1;

	// We go backward
	while(true)
	{
		if(nPrevPos == 0)
			break;
		nPrevPos = PositionBefore(nPrevPos);

		// Make sure we are not in a comment
		if(m_context->IsCommentOrString(nPrevPos))
			continue;

		ch = GetCharAt(nPrevPos);
		if(ch == chOpenBrace)
		{
			// Dec the depth level 
			depth--;
			if(depth == 0)
			{
				matchedPos = nPrevPos;
				return true;
			}
		}
		else if(ch == chCloseBrace)
		{
			// Inc depth level
			depth++;
		}
	}
	return false;
}

//--------------------------------------------------------
// Brace match
//--------------------------------------------------------
void LEditor::MatchBraceAndSelect(bool selRegion)
{
	// Get current position
	long pos = GetCurrentPos();

	if(GetCharAt(pos) == '{' && !m_context->IsCommentOrString(pos))
	{
		BraceMatch(selRegion);
		return;
	}

	if(GetCharAt(PositionBefore(pos)) == '{' && !m_context->IsCommentOrString(PositionBefore(pos)))
	{
		SetCurrentPos(PositionBefore(pos));
		BraceMatch(selRegion);
		return;
	}

	if(GetCharAt(pos) == '}' && !m_context->IsCommentOrString(pos))
	{
		BraceMatch(selRegion);
		return;
	}

	if(GetCharAt(PositionBefore(pos)) == '}' && !m_context->IsCommentOrString(PositionBefore(pos)))
	{
		SetCurrentPos(PositionBefore(pos));
		BraceMatch(selRegion);
		return;
	}
}

void LEditor::BraceMatch(const bool& bSelRegion)
{
	// Check if we have a match
	long endPos = wxScintilla::BraceMatch(GetCurrentPos());
	if(endPos != wxSCI_INVALID_POSITION)
	{
		// Highlight indent guide if exist
		long startPos = GetCurrentPos();
		if(bSelRegion)
		{
			// Select the range
			if(endPos > startPos) {
				SetSelectionEnd(PositionAfter(endPos));
				SetSelectionStart(startPos);
			} else {
				SetSelectionEnd(PositionAfter(startPos));
				SetSelectionStart(endPos);
			}
		}
		else 
		{
			SetSelectionEnd(endPos);
			SetSelectionStart(endPos);
			SetCurrentPos(endPos);
		}
		EnsureCaretVisible();
	}
}

void LEditor::SetActive()
{
	// if the find and replace dialog is opened, set ourself 
	// as the event owners
	if( m_findReplaceDlg ) {
		m_findReplaceDlg->SetEventOwner(GetEventHandler());
	}

	SetFocus();
	SetSCIFocus(true);
}

// Popup a Find/Replace dialog
void LEditor::DoFindAndReplace()
{
	if( m_findReplaceDlg == NULL ) 
	{
		// Create the dialog
		m_findReplaceDlg = new FindReplaceDialog(ManagerST::Get()->GetMainFrame(), m_findReplaceData);
		m_findReplaceDlg->SetEventOwner(this->GetEventHandler());
	}
	
	if( m_findReplaceDlg->IsShown() )
	{
		// make sure that dialog has focus and that this instace
		// of LEditor is the owner for the events
		m_findReplaceDlg->SetEventOwner(this->GetEventHandler());
		m_findReplaceDlg->SetFocus();
		return;
	}

	// the search always starts from the current line
	m_lastMatchPos = GetCurrentPos();
	//if there is a selection, set it
	if(GetSelectedText().IsEmpty() == false)
	{
		m_findReplaceData.SetFindString(GetSelectedText());
		m_findReplaceDlg->SetFindReplaceData(m_findReplaceData);
	}
	m_findReplaceDlg->Show();
}

void LEditor::OnFindDialog(wxCommandEvent& event)
{
	wxEventType type = event.GetEventType();
	bool dirDown = ! (m_findReplaceDlg->GetData().GetFlags() & wxFRD_SEARCHUP ? true : false);

	if( type == wxEVT_FRD_FIND_NEXT )
	{
		FindNext(m_findReplaceDlg->GetData());
	}
	else if( type == wxEVT_FRD_REPLACE )
	{
		if( !Replace() ) {
			if(dirDown){
				if( wxMessageBox(wxT("Lite Editor reached the end of the document, Search again from the start?"), wxT("Lite Editor"), wxYES_NO, m_findReplaceDlg) == wxYES){
					Replace();
				} 
			} else {
				if( wxMessageBox(wxT("Lite Editor reached the start of the document, Search again from the end?"), wxT("Lite Editor"), wxYES_NO, m_findReplaceDlg) == wxYES){
					Replace();
				}
			}
		}
	}
	else if(type == wxEVT_FRD_REPLACEALL)
	{
		ReplaceAll();
	}
	else if(type == wxEVT_FRD_BOOKMARKALL)
	{
		MarkAll();
	}
}

void LEditor::FindNext(const FindReplaceData &data)
{
	bool dirDown = ! (data.GetFlags() & wxFRD_SEARCHUP ? true : false);
	if( !FindAndSelect(data) ) 
	{
		wxWindow *parent = m_findReplaceDlg->IsShown() ? m_findReplaceDlg : NULL;
		if(dirDown)
		{
			if( wxMessageBox(wxT("Lite Editor reached the end of the document, Search again from the start?"), wxT("Lite Editor"), wxYES_NO, parent) == wxYES)
			{
				FindAndSelect(data);
			} 
		} 
		else 
		{
			if( wxMessageBox(wxT("Lite Editor reached the start of the document, Search again from the end?"), wxT("Lite Editor"), wxYES_NO, parent) == wxYES)
			{
				FindAndSelect(data);
			}
		}
	}
}

bool LEditor::Replace()
{
	return Replace(m_findReplaceDlg->GetData());
}

bool LEditor::FindAndSelect()
{
	return FindAndSelect(m_findReplaceDlg->GetData());
}

bool LEditor::FindAndSelect(const FindReplaceData &data)
{
	bool dirDown = ! (data.GetFlags() & wxFRD_SEARCHUP ? true : false);
	int flags = GetSciSearchFlag(data);
	int pos = FindString(data.GetFindString(), flags, dirDown, m_lastMatchPos);
	if (pos >= 0) 
	{
		EnsureCaretVisible();
		SetSelection (pos, pos + (int)data.GetFindString().Length());

		if( dirDown ) {
			m_lastMatchPos = PositionAfter(pos);
		} else {
			m_lastMatchPos = PositionBefore(pos);
		}
		return true;
	}
	else
	{
		// No match was found
		if( dirDown ) {
			m_lastMatchPos = 0;
		} else {
			m_lastMatchPos = GetLength();
		}
		return false;
	}
}

bool LEditor::Replace(const FindReplaceData &data)
{
	wxString replaceString = data.GetReplaceString();
	wxString findString    = data.GetFindString();
	wxString selection = GetSelectedText();

	SetSearchFlags( GetSciSearchFlag(data));
	TargetFromSelection();

	if(SearchInTarget( findString ) != -1) {
		// the selection contains the searched string
		// do the replace
		ReplaceTarget( replaceString );
	}

	//  and find another match in the document
	return FindAndSelect();
}

int LEditor::GetSciSearchFlag(const FindReplaceData &data)
{
	size_t flags = 0;
	size_t wxflags = data.GetFlags();
	wxflags & wxFRD_MATCHWHOLEWORD ? flags |= wxSCI_FIND_WHOLEWORD : flags = flags;
	wxflags & wxFRD_MATCHCASE ? flags |= wxSCI_FIND_MATCHCASE : flags = flags;
	wxflags & wxFRD_REGULAREXPRESSION ? flags |= wxSCI_FIND_REGEXP : flags = flags;
	return static_cast<int>(flags);
}

//----------------------------------------------
// Bookmarks
//----------------------------------------------
void LEditor::AddMarker()
{
	int nPos = GetCurrentPos();
	int nLine = LineFromPosition(nPos);
	MarkerAdd(nLine, 0x8);
}

void LEditor::DelMarker()
{
	int nPos = GetCurrentPos();
	int nLine = LineFromPosition(nPos);
	MarkerDelete(nLine, 0x8);
}

void LEditor::ToggleMarker()
{
	// Add/Remove marker
	// First we check if we already have a marker
	int nPos = GetCurrentPos();
	int nLine = LineFromPosition(nPos);
	int nBits = MarkerGet(nLine);
	bool bHasMraker = nBits & 0x100 ? true : false; // The 8th bit, is 2^8 = 256
	
	if( !bHasMraker )
		//Delete it
		AddMarker();
	else
		//Add one
		DelMarker();
}

void LEditor::DelAllMarkers()
{
	// Delete all markers from the view
	MarkerDeleteAll(0x8);
}

void LEditor::FindNextMarker()
{
	int nPos = GetCurrentPos();
	int nLine = LineFromPosition(nPos);
	int nFoundLine = MarkerNext(nLine + 1, 0x100);
	if (nFoundLine >= 0)
	{
		GotoLine(nFoundLine);
	}
	else
	{
		//We reached the last marker, try again from top
		int nLine = LineFromPosition(0);
		int nFoundLine = MarkerNext(nLine, 0x100);
		if (nFoundLine >= 0)
		{
			GotoLine(nFoundLine);
		}
	}
}

void LEditor::FindPrevMarker()
{
	int nPos = GetCurrentPos();
	int nLine = LineFromPosition(nPos);
	int nFoundLine = MarkerPrevious(nLine - 1, 0x100);
	if (nFoundLine >= 0)
	{
		GotoLine(nFoundLine);
	}
	else
	{
		//We reached first marker, try again from button
		int nFileSize = GetLength();
		int nLine = LineFromPosition(nFileSize);
		int nFoundLine = MarkerPrevious(nLine, 0x100);
		if (nFoundLine >= 0)
		{
			GotoLine(nFoundLine);
		}
	}
}

bool LEditor::ReplaceAll()
{
	int occur = 0;
	wxString findWhat = m_findReplaceDlg->GetData().GetFindString();
	wxString replaceWith = m_findReplaceDlg->GetData().GetReplaceString();
	int flags = (int)m_findReplaceDlg->GetData().GetFlags();

	if (findWhat.IsEmpty()) {
        return false;
    }

    BeginUndoAction();
	long pos = 0;
	
	// Save the caret position
	long savedPos = GetCurrentPos();

	SetCaretAt(0);
    while ((pos = FindString(findWhat, flags, true, pos)) >= 0) 
	{
		occur++;
        ReplaceTarget (replaceWith);
		pos += (int)replaceWith.Length();
		SetCaretAt(pos);
	}
	
	// Restore the caret
	SetCaretAt(savedPos);

    EndUndoAction();
	
	wxString message;
	message << wxT("Replacements: ") << occur;
	m_findReplaceDlg->SetReplacementsMessage(message);
	return occur > 0;
}

bool LEditor::MarkAll()
{
	long pos = wxSCI_INVALID_POSITION;
	long savedPos = GetCurrentPos();
	wxString findWhat = m_findReplaceDlg->GetData().GetFindString();
	int flags = (int)m_findReplaceDlg->GetData().GetFlags();

	if(findWhat.IsEmpty())
		return false;

	// Set the cursor to start
	SetCaretAt(0);

	while ((pos = FindString (findWhat, flags, true, pos)) >= 0) 
	{
		MarkerAdd(LineFromPosition(pos), 0x8);
		pos = PositionAfter(pos);
		SetCaretAt(pos);
	}

	// Resttore the caret
	SetCaretAt(savedPos);
	return true;
}

void LEditor::SetCaretAt(const long pos)
{
	SetCurrentPos(pos);
	SetSelectionStart(pos);
	SetSelectionEnd(pos);
}

void LEditor::ReloadFile()
{
	if(m_fileName.GetFullPath().IsEmpty() == true || m_fileName.GetFullPath().StartsWith(wxT("Untitled")))
		return;

	// Read the entire file content
	wxString text;
	FileUtils::ReadFileUTF8(m_fileName, text);
	SetText( text );
	SetDirty(false);
}

void LEditor::Create(const wxFileName &fileName, const wxString &project)
{
	// set the file name
	SetFileName(fileName);
	// set the project name
	SetProject(project);
	// let the editor choose the syntax highlight to use according to file extension
	// and set the editor properties to default
	RestoreDefaults();
	// reload the file from disk
	ReloadFile();
	// mark this editor as non-modified to avoid non-needed confirm dialogs
	SetSavePoint();
	EmptyUndoBuffer();
}

void LEditor::OnContextMenu(wxContextMenuEvent &event)
{
	wxUnusedVar(event);
	PopupMenu(m_rightClickMenu);
}

bool LEditor::IsSwapFilesEnabled() const
{
	return m_context->IsSwapFilesEnabled();
}

void LEditor::SwapFiles()
{
	m_context->SwapFiles(m_fileName);
}
