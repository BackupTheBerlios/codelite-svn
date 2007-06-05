#include "context_cpp.h"
#include "editor.h"
#include "ctags_manager.h"
#include "manager.h"
#include "symbols_dialog.h"
#include "editor_config.h"

ContextCpp::ContextCpp(LEditor *container)
: ContextBase(container)
, m_tipKind(TipNone)
{
	//-----------------------------------------------
	// Load laguage settings from configuration file
	//-----------------------------------------------
	SetName(wxT("C++"));

	// Set the key words and the lexer
	wxString keyWords;
	std::list<StyleProperty> styles;
	LexerConfPtr lexPtr;
	// Read the configuration file
	if(EditorConfigST::Get()->IsOk()){
		lexPtr = EditorConfigST::Get()->GetLexer(wxT("C++"));
	}

	// Update the control
	LEditor &rCtrl = GetCtrl();
	rCtrl.SetLexer(lexPtr->GetLexerId());
	rCtrl.SetKeyWords(0, lexPtr->GetKeyWords());
	rCtrl.StyleClearAll();
	
	styles = lexPtr->GetProperties();
	std::list<StyleProperty>::iterator iter = styles.begin();
	for(; iter != styles.end(); iter++)
	{
		StyleProperty st = (*iter);
		int size = st.GetFontSize();
		wxString face = st.GetFaceName();
		bool bold = st.IsBold();
		
		wxFont font(size, wxFONTFAMILY_DEFAULT, wxNORMAL, bold ? wxBOLD : wxNORMAL, false, face);
		font.SetFaceName(face);

		rCtrl.StyleSetFont(st.GetId(), font);
		rCtrl.StyleSetSize(st.GetId(), (*iter).GetFontSize());
		rCtrl.StyleSetForeground(st.GetId(), (*iter).GetFgColour());
	}
}

ContextCpp::~ContextCpp()
{
}

ContextBase *ContextCpp::NewInstance(LEditor *container){
	return new ContextCpp(container);
}

void ContextCpp::CodeComplete()
{
	LEditor &rCtrl = GetCtrl();
	long pos = rCtrl.GetCurrentPos();
	bool showFuncProto = false;

	wxChar ch;

	//	Make sure we are not on a comment section
	if(IsCommentOrString(pos))
		return;

	// Search for first non-whitespace wxChar
	int pos1, pos2, end;
	ch = rCtrl.PreviousChar(pos, pos1);

	switch(ch)
	{
	case '.':
		// Class / Struct completion
		rCtrl.PreviousChar(pos1, end);
		break;
	case '>':
		// Check previous character if is '-'
		// We open drop box as well
		if(rCtrl.PreviousChar(pos1, pos2) == '-')
		{
			rCtrl.PreviousChar(pos2, end);
			break;
		}
	case ':':
		// Check previous character if is ':'
		// We open drop box as well
		if(rCtrl.PreviousChar(pos1, pos2) == _T(':'))
		{
			rCtrl.PreviousChar(pos2, end);
			break;
		}
	case '(':
		showFuncProto = true;
		rCtrl.PreviousChar(pos1, end);
		break;
	default:
		return;
	}

	// Get a full expression and pass it, along with the local scope
	// to the Language parser
	// We define an expression, by reading from pos and up until we find
	// the first '{' or ';' or SOT
	int semiColPos = rCtrl.FindString(_T(";"), 0, false, rCtrl.GetCurrentPos());
	int lcurlyPos  = rCtrl.FindString(_T("{"), 0, false, rCtrl.GetCurrentPos());
	int start;
	TagEntry tag;
	int line = 1;

	semiColPos > lcurlyPos ? start = semiColPos : start = lcurlyPos;
	wxString expr = rCtrl.GetTextRange(start, pos);

	// Get the closest function to the current caret position, this will give us a
	// good idea of the scope we are in it
	if( TagsManagerST::Get()->FunctionByLine(rCtrl.LineFromPosition(start)+1, rCtrl.GetFileName().GetFullPath(), rCtrl.GetProjectName(), tag) )
	{
		line = tag.GetLine();
	}

	long scopeStartPos = rCtrl.PositionFromLine(line - 1/* wxScintilla counts line from zero */);
	wxString scope = rCtrl.GetTextRange(scopeStartPos, end);
	wxString scopeName = tag.GetScopeName();

	if( showFuncProto )
	{
		// display call tip with function prototype
		m_ct = TagsManagerST::Get()->GetFunctionTip(expr, scope, scopeName);
		if( m_ct->Count() > 0 )
		{
			// we have a match
			rCtrl.CallTipShow(rCtrl.GetCurrentPos(), m_ct->Next());
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
			GetCtrl().AutoCompShow(0, list);
		}
	}
}
void ContextCpp::AutoIndent(const wxChar &nChar)
{
	LEditor &rCtrl = GetCtrl();
	int indentSize = rCtrl.GetIndent();
	int pos = wxNOT_FOUND;
	long matchPos = wxNOT_FOUND;

	if(IsCommentOrString(rCtrl.GetCurrentPos())){
		ContextBase::AutoIndent(nChar);
		return;
	}

	// enter was pressed
	int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPos());
	if(nChar == wxT('\n')){
		wxChar ch = rCtrl.PreviousChar(rCtrl.GetCurrentPos(), pos);
		if(pos != wxNOT_FOUND && ch == wxT('{')){
			if(IsCommentOrString(pos)){
				return;
			}

			//open brace?
			//increase indent size
			int prevLine = rCtrl.LineFromPosition(pos);
			
			int prevLineIndet = rCtrl.GetLineIndentation(prevLine);
			rCtrl.SetLineIndentation(line, indentSize + prevLineIndet);
		}else{
			//just copy the previous line indentation
			ContextBase::AutoIndent(nChar);
			return;
		}

		int dummy = rCtrl.GetLineIndentation(line);
		if(rCtrl.GetTabIndents()){
			dummy = dummy / indentSize;
		}
		rCtrl.SetCaretAt(rCtrl.GetCurrentPos() + dummy);
	/*} else if(nChar == wxT(':')){
		
		int curlineIndent = rCtrl.GetLineIndentation(line);
		int newIndent = 0;
		/*int p;
		if(rCtrl.PreviousChar(rCtrl.PositionBefore(rCtrl.GetCurrentPos()), p) == wxT(':')){
			//increase indentation
			newIndent = curlineIndent + indentSize;
			rCtrl.SetLineIndentation(line, newIndent);
			int dummy = rCtrl.GetLineIndentation(line);
			if(rCtrl.GetTabIndents()){
				dummy = dummy / indentSize;
			}
			rCtrl.SetCaretAt(rCtrl.GetCurrentPos() + dummy);
		}else{
			//reduce indentation
			if(curlineIndent - indentSize > 0){
				newIndent = curlineIndent - indentSize;
			rCtrl.SetLineIndentation(line, newIndent);
			//}
		}*/
	} else if(nChar == wxT('}') && rCtrl.MatchBraceBack(wxT('}'), rCtrl.GetCurrentPos()-1, matchPos)) {
		int secondLine = rCtrl.LineFromPosition(matchPos);
		if(secondLine == line){
			return;
		}
		int secondLineIndent = rCtrl.GetLineIndentation(secondLine);
		rCtrl.SetLineIndentation(line, secondLineIndent);
	}
}

bool ContextCpp::IsCommentOrString(long pos)
{
	int style;
	style = GetCtrl().GetStyleAt(pos);
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

void ContextCpp::OnDwellEnd(wxScintillaEvent &WXUNUSED(event))
{
	// cancel hover tip
	LEditor &rCtrl = GetCtrl();
	if( m_tipKind == TipHover )
	{
		if( rCtrl.CallTipActive() )
			rCtrl.CallTipCancel();
		m_tipKind = TipNone;
	}
}

void ContextCpp::OnCallTipClick(wxScintillaEvent &event)
{
	LEditor &rCtrl = GetCtrl();
	switch( event.GetPosition() )
	{
	case ArrowUp:
		rCtrl.CallTipCancel();
		rCtrl.CallTipShow(rCtrl.GetCurrentPos(), m_ct->Prev());
		break;
	case ArrowDown:
		rCtrl.CallTipCancel();
		rCtrl.CallTipShow(rCtrl.GetCurrentPos(), m_ct->Next());
		break;
	case Elsewhere:
		break;
	}
}

void ContextCpp::OnDwellStart(wxScintillaEvent &event)
{
	LEditor &rCtrl = GetCtrl();
	// Handle dewell only if a project is opened
	if( rCtrl.GetProjectName().IsEmpty())
		return;

	int start;
	int line = 1;
	long pos = event.GetPosition();
	int  end = rCtrl.WordEndPosition(pos, true);
	int word_start = rCtrl.WordStartPosition(pos, true);

	// try to guess whether we are "standing" over a function
	int foundPos = wxNOT_FOUND;
	wxChar nextchar = rCtrl.NextChar( end, foundPos );
	bool isFunc = nextchar == _T('(') ? true : false;

	// get the expression we are standing on it
	if( IsCommentOrString( pos ) )
		return;
	
	// get the token
	wxString token = rCtrl.GetTextRange(word_start, end);

	// Get a full expression
	// We define an expression, by reading from pos and up until we find
	// the first '{' , ';' or SOT
	int semiColPos = rCtrl.FindString(_T(";"), 0, false, end);
	int lcurlyPos  = rCtrl.FindString(_T("{"), 0, false, end);
	semiColPos > lcurlyPos ? start = semiColPos : start = lcurlyPos;

	//---------------------------------
	// Get the scope name
	//---------------------------------
	
	// Get the closest function to the current caret position, this will give us a
	// good idea of the scope we are in it
	TagEntry tag;
	if( TagsManagerST::Get()->FunctionByLine(rCtrl.LineFromPosition(start)+1,	// scintilla are counting from zero, while ctags are counting from one
		rCtrl.GetFileName().GetFullPath(), rCtrl.GetProjectName(), tag) )
	{
		line = tag.GetLine();
	}

	long scopeStartPos = rCtrl.PositionFromLine(line - 1/* wxScintilla counts line from zero */);
	wxString scope = rCtrl.GetTextRange(scopeStartPos, end);
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
		rCtrl.CallTipCancel();
		rCtrl.CallTipShow( event.GetPosition(), tooltip );
		m_tipKind = TipHover;
	}
}

void ContextCpp::GotoPreviousDefintion()
{
	if(LEditor::GetHistory().empty())
		return;

	// Get the last tag visited
	TagEntry tag(LEditor::GetHistory().top());
	ManagerST::Get()->OpenFile(tag);

	// remove it from the stack
	LEditor::GetHistory().pop();
}

void ContextCpp::GotoDefinition()
{
	LEditor &rCtrl = GetCtrl();
	std::vector<TagEntry> tags;

	//	Make sure we are not on a comment section
	if(IsCommentOrString(rCtrl.GetCurrentPos()))
		return;

	// Get the word under the cursor OR the selected word
	wxString word = rCtrl.GetSelectedText();
	if(word.IsEmpty())
	{
		// No selection, try to find the word under the cursor
		long pos = rCtrl.GetCurrentPos();
		long end = rCtrl.WordEndPosition(pos, true);
		long start = rCtrl.WordStartPosition(pos, true);

		// Get the word
		word = rCtrl.GetTextRange(start, end);
		if(word.IsEmpty())
			return;
	}
	
	// get all tags that matches the name (we use exact match)
	TagsManagerST::Get()->FindSymbol(word, tags);
	if(tags.empty())
		return;

	// Remember this position before skipping to the next one
	TagEntry tag;
	tag.SetLine(rCtrl.LineFromPosition(rCtrl.GetCurrentPos())+1 /** scintilla counts from zero, while tagentry from 1**/);
	tag.SetFile(rCtrl.GetFileName().GetFullPath());
	tag.SetProject(rCtrl.GetProject());
	tag.SetPosition(rCtrl.GetCurrentPos());

	// Keep the current position as well
	LEditor::GetHistory().push(tag);

	// Did we get a single match?
	if(tags.size() == 1)
	{
		// Just open the file and set the cursor on the match we found
		ManagerST::Get()->OpenFile(tags[0]);
	}
	else
	{
		// popup a dialog offering the results to the user
		SymbolsDialog *dlg = new SymbolsDialog(&GetCtrl());
		dlg->AddSymbols( tags, 0 );
		if(dlg->ShowModal() == wxID_OK){
			ManagerST::Get()->OpenFile(dlg->GetFile(), rCtrl.GetProject(), dlg->GetLine()-1);
		}
		dlg->Destroy();
	}
}

void ContextCpp::CallTipCancel()
{
	LEditor &rCtrl = GetCtrl();
	rCtrl.CallTipCancel();
	m_tipKind = TipNone;
}

void ContextCpp::GetWordAndScope(wxString& word, wxString &scope, wxString& scopeName)
{
	LEditor &rCtrl = GetCtrl();
	// Get the partial word that we have
	long pos = rCtrl.GetCurrentPos();
	long start = rCtrl.WordStartPosition(pos, true);
	long end   = rCtrl.WordEndPosition(pos, true);

	word = rCtrl.GetTextRange(start, end);
	if(word.IsEmpty())
		return;

	// Get the visible scope: to reduce the overhead of scanning the entire file
	// we will work on a smaller scope which will be extracted using the following
	// logic: scope will be from 'start' exprStart, up to the first function that starts from
	// here and up or start of file of no function was found
	TagEntry tag;

	int line = 1;
	if( TagsManagerST::Get()->FunctionByLine(rCtrl.LineFromPosition(start), rCtrl.GetFileName().GetFullPath(), rCtrl.GetProject(), tag) )
		line = tag.GetLine();

	long scopeStartPos = rCtrl.PositionFromLine(line - 1/* wxScintilla counts line from zero */);
	scope = rCtrl.GetTextRange(scopeStartPos, start);
	scopeName = tag.GetScopeName();
}

void ContextCpp::CompleteWord()
{
	LEditor &rCtrl = GetCtrl();
	std::vector<TagEntry> tags;
	wxString scope;
	wxString scopeName;
	wxString word;

	//	Make sure we are not on a comment section
	if(IsCommentOrString(rCtrl.GetCurrentPos()))
		return;

	// Get the local scope and the word under the cursor
	GetWordAndScope(word, scope ,scopeName);

	if(word.IsEmpty())
		return;

	TagsManagerST::Get()->GetTags(word, scopeName, tags, PartialMatch, scope);

	/// Convert the vector to a string delimited
	wxString list;
	size_t i=0;
	if( tags.empty() == false )
	{
		for(; i<tags.size()-1; i++)
			list.Append(tags[i].GetName() + _T("@"));
		list.Append(tags[i].GetName());
		rCtrl.AutoCompShow(static_cast<int>(word.Length()), list);
	}
}
