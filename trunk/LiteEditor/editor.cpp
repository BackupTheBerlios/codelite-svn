#include "editor.h"
#include <wx/xrc/xmlres.h>
#include <wx/ffile.h> 
#include <wx/tooltip.h>
#include <wx/settings.h>
#include "parse_thread.h"
#include "ctags_manager.h"
#include "language.h"
#include "editor_config.h"
#include "manager.h"
#include "menumanager.h"
#include <wx/fdrepdlg.h>
#include "findreplacedlg.h"
#include <wx/wxFlatNotebook/renderer.h>

#ifdef USE_TRACE
#define DEBUG_START_TIMER(msg) { wxString logmsg; m_watch.Start(); wxLogMessage(logmsg << _T("Timer started ===> ") << msg); }
#define DEBUG_STOP_TIMER()   { wxString msg; msg << _T("Done, total time elapsed: ") << m_watch.Time() << _T(" milliseconds"); wxLogMessage(msg); }
#else
#define DEBUG_START_TIMER(msg)
#define DEBUG_STOP_TIMER()
#endif

#ifdef USE_TRACE
#define DEBUG_MSG(msg) { wxString tmpLogMsgString; tmpLogMsgString << msg; wxLogMessage(tmpLogMsgString); }
#else
#define DEBUG_MSG(msg) { wxString tmpLogMsgString; tmpLogMsgString << msg; }
#endif 


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

LEditor::LEditor(wxWindow* parent, wxWindowID id, const wxSize& size, const wxString& fileName, const wxString& project)
: wxScintilla(parent, id, wxDefaultPosition, size)
, m_fileName(fileName)
, m_project(project)
, m_tipKind(TipNone)
, m_lastMatchPos(0)
{
	// Initialise the map between a macro of proerpty and its value
	m_propertyInt[_T("wxSCI_C_DEFAULT")] = 0;
	m_propertyInt[_T("wxSCI_C_COMMENT")] = 1;
	m_propertyInt[_T("wxSCI_C_COMMENTLINE")] = 2;
	m_propertyInt[_T("wxSCI_C_COMMENTDOC")] = 3;
	m_propertyInt[_T("wxSCI_C_NUMBER")] = 4;
	m_propertyInt[_T("wxSCI_C_WORD")] = 5;
	m_propertyInt[_T("wxSCI_C_STRING")] = 6;
	m_propertyInt[_T("wxSCI_C_CHARACTER")] = 7;
	m_propertyInt[_T("wxSCI_C_UUID")] = 8;
	m_propertyInt[_T("wxSCI_C_PREPROCESSOR")] = 9;
	m_propertyInt[_T("wxSCI_C_OPERATOR")] = 10;
	m_propertyInt[_T("wxSCI_C_IDENTIFIER")] = 11;
	m_propertyInt[_T("wxSCI_C_STRINGEOL")] = 12;
	m_propertyInt[_T("wxSCI_C_VERBATIM")] = 13;
	m_propertyInt[_T("wxSCI_C_REGEX")] = 14;
	m_propertyInt[_T("wxSCI_C_COMMENTLINEDOC")] = 15;
	m_propertyInt[_T("wxSCI_C_WORD2")] = 16;
	m_propertyInt[_T("wxSCI_C_COMMENTDOCKEYWORD")] = 17;
	m_propertyInt[_T("wxSCI_C_COMMENTDOCKEYWORDERROR")] = 18;
	m_propertyInt[_T("wxSCI_C_GLOBALCLASS")] = 19;

	SetProperties();

	// If file name is provided, open it
	if(	false == m_fileName.GetFullPath().IsEmpty() &&		// valid file name was passed
		m_fileName.GetFullPath().Find(wxT("Untitled")) == -1)	// file name does not contain 'Untitiled'
	{
		OpenFile(m_fileName.GetFullPath(), m_project);
	}
}

LEditor::~LEditor()
{
}

/// Setup some scintilla properties
void LEditor::SetProperties()
{
	SetMouseDwellTime(200);

	// Setup folding
	SetMarginType(1, wxSCI_MARGIN_SYMBOL);
	SetMarginMask(2, wxSCI_MASK_FOLDERS);

	SetProperty(_("fold"), _("1"));
	SetProperty(_("fold.compact"), _("0"));
	SetProperty(_("styling.within.preprocessor"), _("1"));

	// Fold and comments as well
	SetProperty(_("fold.comment"), _("1"));

	//Set the selection color to grey/black as default
	SetSelBackground(true, wxColor(192, 192, 192));
	SetSelForeground(true, wxColor(0x00, 0x00, 0x00));
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
	SetMarginLeft(0);
	SetMarginRight(0);

	// Mark current line
	SetCaretLineVisible(true);
	SetCaretLineBackground(wxColor(213, 234, 255));

	SetFoldFlags(0);
	SetMarginWidth(2, 12);
	

	// Define the folding style to be square
	DefineMarker(wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_BOXMINUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
	DefineMarker(wxSCI_MARKNUM_FOLDER, wxSCI_MARK_BOXPLUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
	DefineMarker(wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_VLINE, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
	DefineMarker(wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_LCORNER, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
	DefineMarker(wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_BOXPLUSCONNECTED, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
	DefineMarker(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_BOXMINUSCONNECTED, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
	DefineMarker(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_TCORNER, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
	// Bookmark
	MarkerDefine(0x8, wxSCI_MARK_ARROW);
	MarkerSetBackground(0x8, wxColour(12, 133, 222));
	MarkerSetForeground(0x8, wxColour(66, 169, 244));

	// Margin colours
	SetFoldMarginColour(true, wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	//SetFoldMarginHiColour(true, wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	
	// calltip settings
    CallTipSetBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
	CallTipSetForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
	
	//-----------------------------------------------
	// Load laguage settings from configuration file
	//-----------------------------------------------

	// Set the key words and the lexer
	wxString keyWords;
	std::vector<AttributeStyle> styles;

	// Read the configuration file
	wxFileName ConfigFileName(_T("liteeditor.xml"));

	// check first presence of a local configfile
	bool ConfigFileFound(ConfigFileName.FileExists());

	// no local configfile found, check if there is one in the installPath
    if(!ConfigFileFound) {
		wxString installPath = ManagerST::Get()->GetInstallPath();
        ConfigFileName = wxFileName(installPath, ConfigFileName.GetFullName());
        ConfigFileFound = ConfigFileName.FileExists();
    } 

	if (ConfigFileFound)
		EditorConfigST::Get()->LoadWords(ConfigFileName,  _T("LEX_CPP"), keyWords);

	// Update the control
	SetLexer(wxSCI_LEX_CPP);
	SetKeyWords(0, keyWords);
	StyleClearAll();

	if (ConfigFileFound)
		EditorConfigST::Get()->LoadStyle(ConfigFileName, _T("LEX_CPP"), styles);

	for(size_t i=0; i<styles.size(); i++)
	{
		AttributeStyle st = styles[i];
		std::map<wxString, int>::iterator iter = m_propertyInt.find(st.GetName());
		if(iter == m_propertyInt.end())
			continue;

		int size = styles[i].GetFontSize();
		wxString face = styles[i].GetFaceName();
		bool bold = styles[i].IsBold();

		wxFont font(size, wxFONTFAMILY_DEFAULT, wxNORMAL, bold ? wxBOLD : wxNORMAL, false, face);
		font.SetFaceName(face);

		StyleSetFont(iter->second, font);
		StyleSetSize(iter->second, styles[i].GetFontSize());
		StyleSetForeground(iter->second, styles[i].GetFgColour());
	}
	SetMarginSensitive(2, true);

	// Set up line numbers margin
	SetLineNumberWidth();

	//---------------------------------------------------
	// Set autocompletion parameters
	//---------------------------------------------------

	AutoCompSetSeparator(static_cast<int>('@'));	// set the separator to be non valid language wxChar
	AutoCompSetChooseSingle(true);					// If only one match, insert it automatically
	AutoCompSetIgnoreCase(true);
	SetIndent(8);

	StyleSetBold(wxSCI_STYLE_BRACELIGHT, true);
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
		AutoIndent(event.GetKey());

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
				CallTipCancel();
				m_tipKind = TipNone;
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

void LEditor::SetLineNumberWidth()
{
	int pixelWidth = 4 + 5 * TextWidth(wxSCI_STYLE_LINENUMBER, _T("9"));
	SetMarginWidth(0, pixelWidth);
	StyleSetSize(wxSCI_STYLE_LINENUMBER, 10);
	StyleSetBackground(wxSCI_STYLE_LINENUMBER, wxFNBRenderer::LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), 50));
	StyleSetForeground(wxSCI_STYLE_LINENUMBER, wxColour(0, 63, 125));
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
	if(GetFileName().GetFullName().Find(wxT("Untitled")) != -1)
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
	ParseThreadST::Get()->Add(m_fileName.GetFullPath(), m_project, fn.GetFullPath());
	return true;
}

bool LEditor::SaveFileAs()
{
	// Prompt the user for a new file name
	const wxString ALL(_T("All Files (*.*)|*.*"));
	wxFileDialog *dlg = new wxFileDialog(this, _("Save As"), m_fileName.GetPath(), m_fileName.GetFullName(), ALL, 
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
	}
	
	delete dlg;
	return true;
}

// an internal function that does the actual file writing to disk
bool LEditor::SaveToFile(const wxFileName &fileName)
{
	wxFFile file(fileName.GetFullPath().GetData(), _T("wb"));
	if(file.IsOpened() == false)
	{
		// Nothing to be done
		wxString msg = wxString::Format(_("Failed to open file %s"), fileName.GetFullPath().GetData());
		wxMessageBox( msg );
		return false;
	}

	file.Write(GetText());
	file.Close();
	SetSavePoint();

	// update the file name (remove the star from the file name)
	ManagerST::Get()->SetPageTitle(this, fileName.GetFullName());
	return true;
}

void LEditor::OpenFile(const wxString &fileName, const wxString &project)
{
	if(fileName.IsEmpty() == true)
		return;

	wxFFile file(fileName.GetData(), _T("r"));
	if(file.IsOpened() == false)
	{
		// Nothing to be done
		wxString msg = wxString::Format(_("Failed to open file %s"), fileName.GetData());
		wxMessageBox( msg );
		return;
	}

	// Read the entire file content
	wxString text;
	file.ReadAll(&text);
	file.Close();
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

void LEditor::GetWordAndScope(wxString& word, wxString &scope, wxString& scopeName)
{
	// Get the partial word that we have
	long pos = GetCurrentPos();
	long start = WordStartPosition(pos, true);
	long end   = WordEndPosition(pos, true);

	word = GetTextRange(start, end);
	if(word.IsEmpty())
		return;

	// Get the visible scope: to reduce the overhead of scanning the entire file
	// we will work on a smaller scope which will be extracted using the following
	// logic: scope will be from 'start' exprStart, up to the first function that starts from
	// here and up or start of file of no function was found
	TagEntry tag;

	int line = 1;
	if( TagsManagerST::Get()->FunctionByLine(LineFromPosition(start), m_fileName.GetFullName(), m_project, tag) )
		line = tag.GetLine();

	long scopeStartPos = PositionFromLine(line - 1/* wxScintilla counts line from zero */);
	scope = GetTextRange(scopeStartPos, start);
	scopeName = tag.GetScopeName();
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
	std::vector<TagEntry> tags;
	wxString scope;
	wxString scopeName;
	wxString word;

	//	Make sure we are not on a comment section
	if(IsCommentOrString(GetCurrentPos()))
		return;

	DEBUG_START_TIMER(_T("Getting local scope"))
	// Get the local scope and the word under the cursor
	GetWordAndScope(word, scope ,scopeName);
	DEBUG_STOP_TIMER()

	if(word.IsEmpty())
		return;

	DEBUG_START_TIMER(_T("Scanning for tags ..."))
	TagsManagerST::Get()->GetTags(word, scopeName, tags, PartialMatch, scope);
	DEBUG_STOP_TIMER()

	/// Convert the vector to a string delimited
	wxString list;
	size_t i=0;
	if( tags.empty() == false )
	{
		for(; i<tags.size()-1; i++)
			list.Append(tags[i].GetName() + _T("@"));
		list.Append(tags[i].GetName());
		AutoCompShow(static_cast<int>(word.Length()), list);
	}
}

//------------------------------------------------------------------
// AutoCompletion, by far the nicest feature of a modern IDE
// This function attempts to resolve the string to the left of
// the '.', '->' operator and to display a popup menu with
// list of possible matches
//------------------------------------------------------------------
void LEditor::CodeComplete()
{
	long pos = GetCurrentPos();
	bool showFuncProto = false;

	wxChar ch;

	//	Make sure we are not on a comment section
	if(IsCommentOrString(pos))
		return;

	// Search for first non-whitespace wxChar
	int pos1, pos2, end;
	ch = PreviousChar(pos, pos1);

	switch(ch)
	{
	case '.':
		// Class / Struct completion
		PreviousChar(pos1, end);
		break;
	case '>':
		// Check previous character if is '-'
		// We open drop box as well
		if(PreviousChar(pos1, pos2) == '-')
		{
			PreviousChar(pos2, end);
			break;
		}
	case ':':
		// Check previous character if is ':'
		// We open drop box as well
		if(PreviousChar(pos1, pos2) == _T(':'))
		{
			PreviousChar(pos2, end);
			break;
		}
	case '(':
		showFuncProto = true;
		PreviousChar(pos1, end);
		break;
	default:
		return;
	}

	// Get a full expression and pass it, along with the local scope
	// to the Language parser
	// We define an expression, by reading from pos and up until we find
	// the first '{' or ';' or SOT
	int semiColPos = FindString(_T(";"), 0, false, GetCurrentPos());
	int lcurlyPos  = FindString(_T("{"), 0, false, GetCurrentPos());
	int start;
	TagEntry tag;
	int line = 1;

	semiColPos > lcurlyPos ? start = semiColPos : start = lcurlyPos;
	wxString expr = GetTextRange(start, pos);

	// Get the closest function to the current caret position, this will give us a
	// good idea of the scope we are in it
	if( TagsManagerST::Get()->FunctionByLine(LineFromPosition(start)+1,	// scintilla are counting from zero,
																		// while ctags are counting from one
												m_fileName.GetFullName(), m_project, tag) )
	{
		line = tag.GetLine();
	}

	long scopeStartPos = PositionFromLine(line - 1/* wxScintilla counts line from zero */);
	wxString scope = GetTextRange(scopeStartPos, end);
	wxString scopeName = tag.GetScopeName();

	if( showFuncProto )
	{
		// display call tip with function prototype
		m_ct = TagsManagerST::Get()->GetFunctionTip(expr, scope, scopeName);
		if( m_ct->Count() > 0 )
		{
			// we have a match
			CallTipShow(GetCurrentPos(), m_ct->Next());
			m_tipKind = TipFuncProto;
		}
	}
	else
	{
		//--------------------------------------------------------------
		// Get list of candidates if any and popup the Autucomplete box
		//--------------------------------------------------------------

		std::vector<TagEntry> candidates;
		if( TagsManagerST::Get()->AutoCompleteCandidates(expr, scope, scopeName, candidates) )
		{
			/// Convert the vector to a string delimited
			wxString list;
			size_t i=0;
			for(; i<candidates.size()-1; i++)
				list.Append(candidates[i].GetName() + _T("@"));
			list.Append(candidates[i].GetName());
			AutoCompShow(0, list);
		}
	}
}

//----------------------------------------------------------------
// Demonstrate how to achieve symbol browsing using the CodeLite
// library, in addition we implements here a memory for allowing
// user to go back and forward
//----------------------------------------------------------------
void LEditor::GotoDefinition()
{
	std::vector<TagEntry> tags;

	//	Make sure we are not on a comment section
	if(IsCommentOrString(GetCurrentPos()))
		return;

	// Get the word under the cursor OR the selected word
	wxString word = GetSelectedText();
	if(word.IsEmpty())
	{
		// No selection, try to find the word under the cursor
		long pos = GetCurrentPos();
		long end = WordEndPosition(pos, true);
		long start = WordStartPosition(pos, true);

		// Get the word
		word = GetTextRange(start, end);
		if(word.IsEmpty())
			return;
	}

	wxString logmsg;
	logmsg << _T("Searching for symbol: ") << word;
	DEBUG_START_TIMER(logmsg);

	// get all tags that matches the name (we use exact match)
	TagsManagerST::Get()->FindSymbol(word, tags);
	DEBUG_STOP_TIMER();
	if(tags.empty())
		return;

	// Did we get a single match?
	if(tags.size() == 1)
	{
		// Remember this position before skipping to the next one
		TagEntry history;
		history.SetLine(LineFromPosition(GetCurrentPos())+1 /** scintilla counts from zero, while tagentry from 1**/);
		history.SetFile(m_fileName.GetFullPath());
		history.SetProject(m_project);

		// Just open the file and set the cursor on the match we found
		ManagerST::Get()->OpenFile(tags[0]);
		m_history.push(history);
	}
	else
	{
		// Write the symbols found to the log, in normal editors,
		// I suggest poping up a dialog with ListBox suggesting user
		// to resolve the ambiguity
#ifdef USE_TRACE
		wxString logmsg;
		for(size_t i=0; i<tags.size(); i++)
			logmsg << _T("\n") << tags[i].GetName() << _T(" : ") << tags[i].GetFile() << _T(" : ") << tags[i].GetLine();
		wxLogMessage(logmsg);
#endif
	}
}

void LEditor::GotoPreviousDefintion()
{
	if(m_history.empty())
		return;

	// Get the last tag visited
	TagEntry tag(m_history.top());
	ManagerST::Get()->OpenFile(tag);

	// remove it from the stack
	m_history.pop();
}

void LEditor::OnDwellStart(wxScintillaEvent & event)
{
	// Handle dewell only if a project is opened
	if( GetProjectName().IsEmpty())
		return;

	int start;
	int line = 1;
	long pos = event.GetPosition();
	int  end = WordEndPosition(pos, true);
	int word_start = WordStartPosition(pos, true);

	// try to guess whether we are "standing" over a function
	int foundPos = wxNOT_FOUND;
	wxChar nextchar = NextChar( end, foundPos );
	bool isFunc = nextchar == _T('(') ? true : false;

	// get the expression we are standing on it
	if( IsCommentOrString( pos ) )
		return;
	
	// get the token
	wxString token = GetTextRange(word_start, end);

	// Get a full expression
	// We define an expression, by reading from pos and up until we find
	// the first '{' , ';' or SOT
	int semiColPos = FindString(_T(";"), 0, false, end);
	int lcurlyPos  = FindString(_T("{"), 0, false, end);
	semiColPos > lcurlyPos ? start = semiColPos : start = lcurlyPos;

	//---------------------------------
	// Get the scope name
	//---------------------------------
	
	// Get the closest function to the current caret position, this will give us a
	// good idea of the scope we are in it
	TagEntry tag;
	if( TagsManagerST::Get()->FunctionByLine(LineFromPosition(start)+1,	// scintilla are counting from zero, while ctags are counting from one
	        								 m_fileName.GetFullName(), m_project, tag) )
	{
		line = tag.GetLine();
	}

	long scopeStartPos = PositionFromLine(line - 1/* wxScintilla counts line from zero */);
	wxString scope = GetTextRange(scopeStartPos, end);
	wxString scopeName = tag.GetScopeName();

	//-------------------------------------------------------------
	// now we are ready to process the scope and build our tips
	//-------------------------------------------------------------
	std::vector<wxString> tips;
	TagsManagerST::Get()->GetHoverTip(token, scope, scopeName, isFunc, tips);

	//-------------------------------------------------------------
	// display a tooltip
	//-------------------------------------------------------------
	wxString tooltip;
	if( tips.size() > 0 )
	{
		tooltip << tips[0];
		for( size_t i=1; i<tips.size(); i++ )
			tooltip << _T("\n\n") << tips[i];

		// cancel any old calltip and display the new one
		CallTipCancel();
		CallTipShow( event.GetPosition(), tooltip );
		m_tipKind = TipHover;
	}
}

void LEditor::OnDwellEnd(wxScintillaEvent & WXUNUSED(event))
{
	// cancel hover tip
	if( m_tipKind == TipHover )
	{
		if( CallTipActive() )
			CallTipCancel();
		m_tipKind = TipNone;
	}
}

void LEditor::OnCallTipClick(wxScintillaEvent& event)
{
	switch( event.GetPosition() )
	{
	case ArrowUp:
		CallTipCancel();
		CallTipShow(GetCurrentPos(), m_ct->Prev());
		break;
	case ArrowDown:
		CallTipCancel();
		CallTipShow(GetCurrentPos(), m_ct->Next());
		break;
	case Elsewhere:
		break;
	}
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
			curpos = PositionBefore( curpos );
			if(curpos == 0)
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

bool LEditor::IsCommentOrString(long pos)
{
	int style;
	style = GetStyleAt(pos);
	return (style == wxSCI_C_COMMENT				||
			style == wxSCI_C_COMMENTLINE			||
			style == wxSCI_C_COMMENTDOC				||
			style == wxSCI_C_COMMENTLINEDOC			||
			style == wxSCI_C_COMMENTDOCKEYWORD		||
			style == wxSCI_C_COMMENTDOCKEYWORDERROR ||
			style == wxSCI_C_STRING					||
			style == wxSCI_C_STRINGEOL				||
			style == wxSCI_C_CHARACTER);
}

void LEditor::AutoIndent(const wxChar& nChar)
{
	int nPos = PositionBefore(GetCurrentPos());
	int nLine = LineFromPosition(nPos);
	int lineIndent = GetLineIndentation(nLine);	// Default line indentation
	int savedLineIndent = lineIndent;
	int indentSize = GetIndent();
	int newPos;

	// Find the previous types char, if it is '{' 
	// then increase the indentation level
	// if not, the indentation level is the same as 
	// previous line
	int prevPos = nPos;
	long nMatchPos;
	bool bFirstCharOfDoc = true;
	wxChar ch;
	switch(nChar)
	{
	case '\n':
		if(prevPos == 0) 
			// Probably we are at the firs line of the document
			bFirstCharOfDoc = true;

		while(PositionBefore(prevPos) || bFirstCharOfDoc)
		{
			prevPos = PositionBefore(prevPos);
			bFirstCharOfDoc = false;
			if((GetCharAt(prevPos) != '\r') &&
				(GetCharAt(prevPos) != '\t') &&
				(GetCharAt(prevPos) != '\v') &&
				(GetCharAt(prevPos) != ' ') )
			{
				ch = GetCharAt(prevPos);
				if(ch == '{')
				{
					// Make sure that this char is not in a comment
					// Check the style at the char position
					if(IsCommentOrString(prevPos))
						continue;
					lineIndent = GetLineIndentation(LineFromPosition(prevPos)) + indentSize;
					break;
				}
				else if(ch == ':')
				{
					if(IsCommentOrString(prevPos))
						continue;
					lineIndent = GetLineIndentation(LineFromPosition(prevPos)) - indentSize;

					// Keep indentation positive
					lineIndent = (lineIndent > 0) ? lineIndent : 0;

					// Reduce the line indentation for this line
					SetLineIndentation(LineFromPosition(prevPos), lineIndent);

					// Restore the line number
					if(lineIndent == 0)
						lineIndent = indentSize;
					else
						lineIndent = savedLineIndent;
					break;
				}
				else
					break;
			}
		}

		// Set the new indentation and postion the cursor
		SetLineIndentation(nLine + 1, lineIndent);
		newPos = GetLineEndPosition(nLine + 1);
		SetCurrentPos(newPos);
		SetSelectionStart(newPos);
		SetSelectionEnd(newPos);
		break;
	case '}':
		if(IsCommentOrString(prevPos))
			break;

		// Find the matching OPENING brace
		if(!MatchBraceBack('}', prevPos, nMatchPos))
			return;

		lineIndent = GetLineIndentation(LineFromPosition(nMatchPos));
		//lineIndent -= indentSize;
		if(lineIndent<0)
			lineIndent=0;

		// Set the new indentation and postion the cursor
		SetLineIndentation(nLine, lineIndent);
		newPos = GetLineEndPosition(nLine);
		SetCurrentPos(newPos);
		SetSelectionStart(newPos);
		SetSelectionEnd(newPos);
		break;
	default:
		// Do nothing
		break;
	}
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
		if(IsCommentOrString(nPrevPos))
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

	if(GetCharAt(pos) == '{' && !IsCommentOrString(pos))
	{
		BraceMatch(selRegion);
		return;
	}

	if(GetCharAt(PositionBefore(pos)) == '{' && !IsCommentOrString(PositionBefore(pos)))
	{
		SetCurrentPos(PositionBefore(pos));
		BraceMatch(selRegion);
		return;
	}

	if(GetCharAt(pos) == '}' && !IsCommentOrString(pos))
	{
		BraceMatch(selRegion);
		return;
	}

	if(GetCharAt(PositionBefore(pos)) == '}' && !IsCommentOrString(PositionBefore(pos)))
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
				if( wxMessageBox(wxT("CodeLite reached the end of the document, Search again from the start?"), wxT("Confirm"), wxYES_NO, m_findReplaceDlg) == wxYES){
					Replace();
				} 
			} else {
				if( wxMessageBox(wxT("CodeLite reached the start of the document, Search again from the end?"), wxT("Confirm"), wxYES_NO, m_findReplaceDlg) == wxYES){
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
	if( !FindAndSelect(data) ) {
		wxWindow *parent = m_findReplaceDlg->IsShown() ? m_findReplaceDlg : NULL;
		if(dirDown){
			if( wxMessageBox(wxT("CodeLite reached the end of the document, Search again from the start?"), wxT("Confirm"), wxYES_NO, parent) == wxYES){
				FindAndSelect(data);
			} 
		} else {
			if( wxMessageBox(wxT("CodeLite reached the start of the document, Search again from the end?"), wxT("Confirm"), wxYES_NO, parent) == wxYES){
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
