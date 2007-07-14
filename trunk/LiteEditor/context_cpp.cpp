#include "context_cpp.h"
#include "editor.h"
#include "ctags_manager.h"
#include "manager.h"
#include "symbols_dialog.h"
#include "editor_config.h"
#include "macros.h" 
#include "wx/xrc/xmlres.h"
#include "algorithm"
#include "language.h"
#include "browse_record.h"

#define VALIDATE_PROJECT()\
	if(rCtrl.GetProject().IsEmpty())\
	{\
		return;\
	}

//Images initialization
wxBitmap ContextCpp::m_classBmp = wxNullBitmap;
wxBitmap ContextCpp::m_structBmp = wxNullBitmap;
wxBitmap ContextCpp::m_namespaceBmp = wxNullBitmap;
wxBitmap ContextCpp::m_variableBmp = wxNullBitmap;
wxBitmap ContextCpp::m_tpyedefBmp = wxNullBitmap;
wxBitmap ContextCpp::m_memberPrivateBmp = wxNullBitmap;
wxBitmap ContextCpp::m_memberPublicBmp = wxNullBitmap;
wxBitmap ContextCpp::m_memberProtectedeBmp = wxNullBitmap;
wxBitmap ContextCpp::m_functionPrivateBmp = wxNullBitmap;
wxBitmap ContextCpp::m_functionPublicBmp = wxNullBitmap;
wxBitmap ContextCpp::m_functionProtectedeBmp = wxNullBitmap;
wxBitmap ContextCpp::m_macroBmp = wxNullBitmap;
wxBitmap ContextCpp::m_enumBmp = wxNullBitmap;

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
		
		wxFont font(size, wxFONTFAMILY_TELETYPE, wxNORMAL, bold ? wxBOLD : wxNORMAL);
		//font.SetFaceName(face);

		rCtrl.StyleSetFont(st.GetId(), font);
		rCtrl.StyleSetSize(st.GetId(), (*iter).GetFontSize());
		rCtrl.StyleSetForeground(st.GetId(), (*iter).GetFgColour());
	}

	//create all images used by the cpp context
	wxImage img;
	if(m_classBmp.IsOk() == false)
	{
		m_classBmp = wxXmlResource::Get()->LoadBitmap(wxT("class"));
		m_structBmp = wxXmlResource::Get()->LoadBitmap(wxT("struct"));
		m_namespaceBmp = wxXmlResource::Get()->LoadBitmap(wxT("namespace"));
		m_variableBmp = wxXmlResource::Get()->LoadBitmap(wxT("member_public"));
		m_tpyedefBmp = wxXmlResource::Get()->LoadBitmap(wxT("typedef"));
		m_tpyedefBmp.SetMask(new wxMask(m_tpyedefBmp, wxColor(0, 128, 128)));

		m_memberPrivateBmp = wxXmlResource::Get()->LoadBitmap(wxT("member_private"));
		m_memberPublicBmp = wxXmlResource::Get()->LoadBitmap(wxT("member_public"));
		m_memberProtectedeBmp = wxXmlResource::Get()->LoadBitmap(wxT("member_protected"));
		m_functionPrivateBmp = wxXmlResource::Get()->LoadBitmap(wxT("func_private"));
		m_functionPublicBmp = wxXmlResource::Get()->LoadBitmap(wxT("func_public"));
		m_functionProtectedeBmp = wxXmlResource::Get()->LoadBitmap(wxT("func_protected"));
		m_macroBmp = wxXmlResource::Get()->LoadBitmap(wxT("typedef"));
		m_macroBmp.SetMask(new wxMask(m_macroBmp, wxColor(0, 128, 128)));

		m_enumBmp = wxXmlResource::Get()->LoadBitmap(wxT("enum"));
		m_enumBmp.SetMask(new wxMask(m_enumBmp, wxColor(0, 128, 128)));
	}

	//register the images
	rCtrl.ClearRegisteredImages();
	rCtrl.RegisterImage(1, m_classBmp);
	rCtrl.RegisterImage(2, m_structBmp);
	rCtrl.RegisterImage(3, m_namespaceBmp);
	rCtrl.RegisterImage(4, m_variableBmp);
	rCtrl.RegisterImage(5, m_tpyedefBmp);
	rCtrl.RegisterImage(6, m_memberPrivateBmp);
	rCtrl.RegisterImage(7, m_memberPublicBmp);
	rCtrl.RegisterImage(8, m_memberProtectedeBmp);
	rCtrl.RegisterImage(9, m_functionPrivateBmp);
	rCtrl.RegisterImage(10, m_functionPublicBmp);
	rCtrl.RegisterImage(11, m_functionProtectedeBmp);
	rCtrl.RegisterImage(12, m_macroBmp);
	rCtrl.RegisterImage(13, m_enumBmp);
}

ContextCpp::~ContextCpp()
{
}

ContextBase *ContextCpp::NewInstance(LEditor *container){
	return new ContextCpp(container);
}

void ContextCpp::OnDwellEnd(wxScintillaEvent &event)
{
	LEditor &rCtrl = GetCtrl();
	rCtrl.CallTipCancel();
	m_tipKind = TipNone;
	event.Skip();
}

void ContextCpp::OnDwellStart(wxScintillaEvent &event) 
{
	LEditor &rCtrl = GetCtrl();

	VALIDATE_PROJECT();

	//before we start, make sure we are the visible window
	Manager *mgr = ManagerST::Get();
	if(mgr->GetActiveEditor() != &rCtrl)
	{
		wxLogMessage(wxT("OnDwellStart cancelled - we are no longer the active tab"));
		event.Skip();
		return;
	} // if(mgr->GetActiveEditor() != &rCtrl)

	// Handle dewell only if a project is opened
	if(rCtrl.GetProjectName().IsEmpty())
		return;

	int start;
	long pos = event.GetPosition();
	int  end = rCtrl.WordEndPosition(pos, true);
	int word_start = rCtrl.WordStartPosition(pos, true);

	// get the expression we are standing on it
	if( IsCommentOrString( pos ) )
		return;
	
	// get the token
	wxString word = rCtrl.GetTextRange(word_start, end);

	// The full expression is obtained by simply searching backwards for 
	// the first '{' or ';' or SOT
	int semiColPos = rCtrl.FindString(wxT(";"), 0, false, pos);
	int lcurlyPos  = rCtrl.FindString(wxT("{"), 0, false, pos);
	// get the full text of the current page
	wxString text = rCtrl.GetTextRange(0, pos);
	
	semiColPos > lcurlyPos ? start = semiColPos : start = lcurlyPos;
	if(start < 0){
		start = 0;
	}
	wxString expr = rCtrl.GetTextRange(start, end);
	//-------------------------------------------------------------
	// now we are ready to process the scope and build our tips
	//-------------------------------------------------------------
	std::vector<wxString> tips;
	TagsManagerST::Get()->GetHoverTip(expr, word, text, tips);

	//-------------------------------------------------------------
	// display a tooltip
	//-------------------------------------------------------------
	wxString tooltip;
	if( tips.size() > 0 )
	{
		tooltip << tips[0];
		for( size_t i=1; i<tips.size(); i++ )
			tooltip << wxT("\n") << tips[i];

		// cancel any old calltip and display the new one
		rCtrl.CallTipCancel();
		rCtrl.CallTipShow( event.GetPosition(), tooltip );
		m_tipKind = TipHover;
	}
}

wxString ContextCpp::GetImageString(const TagEntry &entry)
{
	if(entry.GetKind() == wxT("class"))
		return wxT("?1");

	if(entry.GetKind() == wxT("struct"))
		return wxT("?2");

	if(entry.GetKind() == wxT("namespace"))
		return wxT("?3");

	if(entry.GetKind() == wxT("variable"))
		return wxT("?4");

	if(entry.GetKind() == wxT("typedef"))
		return wxT("?5");

	if(entry.GetKind() == wxT("member") && entry.GetAccess() == wxT("private"))
		return wxT("?6");

	if(entry.GetKind() == wxT("member") && entry.GetAccess() == wxT("public"))
		return wxT("?7");

	if(entry.GetKind() == wxT("member") && entry.GetAccess() == wxT("protected"))
		return wxT("?8");

	if((entry.GetKind() == wxT("function") || entry.GetKind() == wxT("prototype")) && entry.GetAccess() == wxT("private"))
		return wxT("?9");

	if((entry.GetKind() == wxT("function") || entry.GetKind() == wxT("prototype")) && (entry.GetAccess() == wxT("public") || entry.GetAccess().IsEmpty()))
		return wxT("?10");

	if((entry.GetKind() == wxT("function") || entry.GetKind() == wxT("prototype")) && entry.GetAccess() == wxT("protected"))
		return wxT("?11");

	if(entry.GetKind() == wxT("macro"))
		return wxT("?12");

	return wxEmptyString;
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

void ContextCpp::CallTipCancel()
{
	LEditor &rCtrl = GetCtrl();
	rCtrl.CallTipCancel();
	m_tipKind = TipNone;
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

//=============================================================================
// >>>>>>>>>>>>>>>>>>>>>>>> CodeCompletion API - START
//=============================================================================

//user pressed ., -> or ::
void ContextCpp::CodeComplete()
{
	LEditor &rCtrl = GetCtrl();

	VALIDATE_PROJECT();

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
		if(rCtrl.PreviousChar(pos1, pos2) == wxT(':'))
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

	// The auto-completion functionality needs the following input from user
	// inorder to try display a auto-completion box:
	// -the text up till the current position
	// -the statement (an example of statement: 'm_somemember.getName()->' excluding the '->') 
	// -an output array that will be populated with possible matches

	// The full expression is obtained by simply searching backwards for 
	// the first '{' or ';' or SOT
	int semiColPos = rCtrl.FindString(wxT(";"), 0, false, rCtrl.GetCurrentPos());
	int lcurlyPos  = rCtrl.FindString(wxT("{"), 0, false, rCtrl.GetCurrentPos());
	// get the full text of the current page
	wxString text = rCtrl.GetTextRange(0, rCtrl.GetCurrentPos());
	
	int start;
	semiColPos > lcurlyPos ? start = semiColPos : start = lcurlyPos;
	if(start < 0){
		start = 0;
	}

	wxString expr = rCtrl.GetTextRange(start, pos);
	std::vector<TagEntryPtr> candidates;
	if( showFuncProto )
	{
		//display function tooltip 
	}
	else
	{
		if(TagsManagerST::Get()->AutoCompleteCandidates(expr, text, candidates)){
			DisplayCompletionBox(candidates, wxEmptyString);
		}
	}
}

wxString ContextCpp::GetWordUnderCaret()
{
	LEditor &rCtrl = GetCtrl();
	// Get the partial word that we have
	long pos = rCtrl.GetCurrentPos();
	long start = rCtrl.WordStartPosition(pos, true);
	long end   = rCtrl.WordEndPosition(pos, true);

	return rCtrl.GetTextRange(start, end);
}

void ContextCpp::CompleteWord()
{
	LEditor &rCtrl = GetCtrl();

	VALIDATE_PROJECT();

	std::vector<TagEntryPtr> tags;
	wxString scope;
	wxString scopeName;
	wxString word;

	//	Make sure we are not on a comment section
	if(IsCommentOrString(rCtrl.GetCurrentPos()))
		return;

	// get the word under the cursor
	word = GetWordUnderCaret();

	if(word.IsEmpty())
		return;

	TagsManager *mgr = TagsManagerST::Get();

	//get the current expression
	int semiColPos = rCtrl.FindString(wxT(";"), 0, false, rCtrl.GetCurrentPos());
	int lcurlyPos  = rCtrl.FindString(wxT("{"), 0, false, rCtrl.GetCurrentPos());
	int start;
	semiColPos > lcurlyPos ? start = semiColPos : start = lcurlyPos;
	if(start < 0){
		start = 0;
	}

	wxString expr = rCtrl.GetTextRange(start, rCtrl.GetCurrentPos());
	std::vector<TagEntryPtr> candidates;

	//get the full text of the current page
	wxString text = rCtrl.GetTextRange(0, rCtrl.GetCurrentPos());
	if(mgr->WordCompletionCandidates(expr, text, word, candidates))
	{
		DisplayCompletionBox(candidates, word);
	}
}

void ContextCpp::DisplayCompletionBox(const std::vector<TagEntryPtr> &tags, const wxString &word)
{
	LEditor &rCtrl = GetCtrl();
	wxString list;
	size_t i=0;
	if( tags.empty() == false )
	{
		for(; i<tags.size()-1; i++)
			list.Append(tags[i]->GetName() + GetImageString(*tags[i]) + wxT("@"));
		list.Append(tags[i]->GetName() + GetImageString(*tags[i]));
		rCtrl.AutoCompSetAutoHide(false);
		rCtrl.AutoCompSetIgnoreCase(true);
		rCtrl.AutoCompShow(static_cast<int>(word.Length()), list);
		rCtrl.AutoCompSetFillUps(wxT("("));
	}
}

//=============================================================================
// <<<<<<<<<<<<<<<<<<<<<<<<<<< CodeCompletion API - END
//=============================================================================

void ContextCpp::GotoPreviousDefintion()
{
	if(LEditor::GetHistory().empty())
		return;

	// Get the last tag visited
	BrowseRecord record = LEditor::GetHistory().top();
	ManagerST::Get()->OpenFile(record.filename, record.project, record.lineno, record.position);

	// remove it from the stack
	LEditor::GetHistory().pop();
}

void ContextCpp::GotoDefinition()
{
	LEditor &rCtrl = GetCtrl();

	VALIDATE_PROJECT();

	std::vector<TagEntryPtr> tags;

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
	BrowseRecord record;
	record.lineno = rCtrl.LineFromPosition(rCtrl.GetCurrentPos())+1; // scintilla counts from zero, while tagentry from 1
	record.filename = rCtrl.GetFileName().GetFullPath();

	//if the file is part of the workspace set the project name
	//else, open it with empty project
	record.position = rCtrl.GetCurrentPos();

	// Keep the current position as well
	LEditor::GetHistory().push(record);

	// Did we get a single match?
	if(tags.size() == 1)
	{
		// Just open the file and set the cursor on the match we found
		wxString projectName = ManagerST::Get()->GetProjectNameByFile(tags[0]->GetFile());
		ManagerST::Get()->OpenFile(tags[0]->GetFile(), projectName, tags[0]->GetLine()-1);
	}
	else
	{
		// popup a dialog offering the results to the user
		SymbolsDialog *dlg = new SymbolsDialog(&GetCtrl());
		dlg->AddSymbols( tags, 0 );
		if(dlg->ShowModal() == wxID_OK){
			ManagerST::Get()->OpenFile(dlg->GetFile(), dlg->GetProject(), dlg->GetLine()-1);
		}
		dlg->Destroy();
	}
}

void ContextCpp::SwapFiles(const wxFileName &fileName)
{
	wxFileName otherFile(fileName);
	wxString ext = fileName.GetExt();
	wxArrayString exts;

	//replace the file extension
	if(IsSourceFile(ext)){
		//try to find a header file
		exts.Add(wxT("h"));
		exts.Add(wxT("hpp"));
		exts.Add(wxT("hxx"));
	}else{
		//try to find a implementation file
		exts.Add(wxT("cpp"));
		exts.Add(wxT("cxx"));
		exts.Add(wxT("cc"));
		exts.Add(wxT("c"));
	}

	for(size_t i=0; i<exts.GetCount(); i++){
		otherFile.SetExt(exts.Item(i));
		if(TryOpenFile(otherFile)){
			return;
		}
	}	
}

bool ContextCpp::TryOpenFile(const wxFileName &fileName)
{
	if(fileName.FileExists()){
		//we got a match
		wxString proj = ManagerST::Get()->GetProjectNameByFile(fileName.GetFullPath());
		ManagerST::Get()->OpenFile(fileName.GetFullPath(), proj);
		return true;
	}

	//ok, the file does not exist in the current directory, try to find elsewhere 
	//whithin the workspace files
	std::vector<wxFileName> files;
	ManagerST::Get()->GetWorkspaceFiles(files, true);

	for(size_t i=0; i<files.size(); i++)
	{
		if(files.at(i).GetFullName() == fileName.GetFullName())
		{
			wxString proj = ManagerST::Get()->GetProjectNameByFile(files.at(i).GetFullPath());
			ManagerST::Get()->OpenFile(files.at(i).GetFullPath(), proj);
		}
	}
	return false;
}
