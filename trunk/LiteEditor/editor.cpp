#include "frame.h"
#include "editor.h"
#include <wx/ffile.h>
#include <wx/tooltip.h>
#include <wx/settings.h>
#include "parse_thread.h"
#include "ctags_manager.h"
#include "language.h"
#include "editor_config.h"

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
END_EVENT_TABLE()

std::stack<TagEntry> LEditor::m_history;

LEditor::LEditor(wxWindow* parent, wxWindowID id, const wxSize& size, const wxString& fileName, const wxString& project)
: wxScintilla(parent, id, wxDefaultPosition, size)
, m_fileName(fileName)
, m_project(project)
, m_tipKind(TipNone)
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
	if(false == m_fileName.IsEmpty())
		OpenFile(m_fileName, m_project);
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
	SetProperty(_("fold.compact"), _("1"));
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
	SetCaretLineBackground(wxColor(255, 255, 210)); // Light yellow

	SetFoldFlags(20);
	SetMarginWidth(2, 16);
	SetFoldMarginColour(true, wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	// Define the folding style to be square
	DefineMarker(wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_ARROWDOWN, wxColor(0xff, 0xff, 0xff), wxColor(0, 0, 0xFF));
	DefineMarker(wxSCI_MARKNUM_FOLDER, wxSCI_MARK_ARROW, wxColor(0xff, 0xff, 0xff), wxColor(0, 0, 0));
	DefineMarker(wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_EMPTY, wxColor(0xff, 0xff, 0xff), wxColor(0, 0, 0));
	DefineMarker(wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_EMPTY, wxColor(0xff, 0xff, 0xff), wxColor(0, 0, 0));
	DefineMarker(wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_EMPTY, wxColor(0xff, 0xff, 0xff), wxColor(0, 0, 0));
	DefineMarker(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_EMPTY, wxColor(0xff, 0xff, 0xff), wxColor(0, 0, 0));
	DefineMarker(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_EMPTY, wxColor(0xff, 0xff, 0xff), wxColor(0, 0, 0));


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
        wxString installPath = Frame::Get()->GetInstallPath();
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

		wxFont font(size, wxTELETYPE, wxNORMAL, bold ? wxBOLD : wxNORMAL, false, face);
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
}

void LEditor::OnCharAdded(wxScintillaEvent& event)
{
	switch( event.GetKey() )
	{
	case '.':
	case '>':
	case ':':
	case '(':
		CodeComplete();
		break;
	case ')':
		{
			CallTipCancel();
			m_tipKind = TipNone;
			break;
		}
	default:
		break;
	}
	event.Skip();
}

void LEditor::SetLineNumberWidth()
{
	int pixelWidth = 4 + 5 * TextWidth(wxSCI_STYLE_LINENUMBER, _T("9"));
	SetMarginWidth(0, pixelWidth);
	StyleSetBackground(wxSCI_STYLE_LINENUMBER, wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	StyleSetSize(wxSCI_STYLE_LINENUMBER, 10);
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

void LEditor::SaveFile()
{
	//-------------------------------------------------------------------
	// Using the CodeParser library, enforces us to notify the parsing
	// thread once the file is saved. ctags accepts file name, so we
	// notify him once the file on the disk is changed, there is no
	// point in notifying the parsing thread on, for example, OnCharAdded
	// event, since the actual file on the disk was not modified
	//-------------------------------------------------------------------
	wxFFile file(m_fileName.GetData(), _T("wb"));
	if(file.IsOpened() == false)
	{
		// Nothing to be done
		wxString msg = wxString::Format(_("Failed to open file %s"), m_fileName.GetData());
		wxMessageBox( msg );
		return;
	}

	file.Write(GetText());
	file.Close();

	// Put a request on the parsing thread to update the GUI tree for this file
	wxFileName fn = TagsManagerST::Get()->GetDatabase()->GetDatabaseFileName();
	ParseThreadST::Get()->Add(m_fileName, m_project, fn.GetFullPath());
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
	if( TagsManagerST::Get()->FunctionByLine(LineFromPosition(start), m_fileName, m_project, tag) )
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
												m_fileName, m_project, tag) )
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
		history.SetFile(m_fileName);
		history.SetProject(m_project);

		// Just open the file and set the cursor on the match we found
		Frame::Get()->OpenFile(tags[0]);
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
	Frame::Get()->OpenFile(tag);

	// remove it from the stack
	m_history.pop();
}

void LEditor::OnDwellStart(wxScintillaEvent & event)
{
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
	        								 m_fileName, m_project, tag) )
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
