#include "precompiled_header.h"

#include "language.h"
#include "ctags_manager.h"
#include "y.tab.h"
#include <wx/stopwatch.h>
#include <wx/ffile.h>

#ifdef __VISUALC__
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif 

namespace codelite
{

#ifdef USE_TRACE
#define DEBUG_START_TIMER(msg) { wxString logline; logline << wxT("Timer started ===> ") << msg; m_watch.Start(); wxLogMessage(logline); }
#define DEBUG_STOP_TIMER()   { wxString msg; msg << wxT("Done, total time elapsed: ") << m_watch.Time() << wxT(" milliseconds"); wxLogMessage(msg); }
#else
#define DEBUG_START_TIMER(msg)
#define DEBUG_STOP_TIMER()
#endif

// Some useful macros
#define CHECK_STATE(x) { if(state != x) { flag = 0; break; } }
#define CHECK_STATE2(x, y) { if(state != x && state != y) { flag = 0; break; } }
#define STATE(x) { state = x; }

Language::Language()
: m_expression(wxEmptyString)
, m_scanner( new CppScanner() )
{
	// Init the operator map
	m_operatorMap[wxT("()")]  = 1; m_operatorMap[wxT("[]")]  = 1; m_operatorMap[wxT("->")]  = 1;
	m_operatorMap[wxT("++")]  = 1; m_operatorMap[wxT("--")]  = 1; m_operatorMap[wxT("+=")]  = 1;
	m_operatorMap[wxT("-=")]  = 1; m_operatorMap[wxT("<<=")] = 1; m_operatorMap[wxT(">>=")] = 1;
	m_operatorMap[wxT("->*")] = 1; m_operatorMap[wxT("-=")]  = 1; m_operatorMap[wxT("*=")]  = 1;
	m_operatorMap[wxT("%=")]  = 1; m_operatorMap[wxT("||")]  = 1; m_operatorMap[wxT("|=")]  = 1;
	m_operatorMap[wxT("&&")]  = 1; m_operatorMap[wxT("&=")]  = 1; m_operatorMap[wxT("^=")]  = 1;
	m_operatorMap[wxT(".*")]  = 1; m_operatorMap[wxT("==")]  = 1; m_operatorMap[wxT("!=")]  = 1;
	m_operatorMap[wxT("<=")]  = 1; m_operatorMap[wxT("<<")]  = 1; m_operatorMap[wxT(">>")]  = 1;
	m_operatorMap[wxT(">=")]  = 1; m_operatorMap[wxT("=")]   = 1; m_operatorMap[wxT("<")]   = 1;
	m_operatorMap[wxT(">")]   = 1;

	// Init the reserved words map (words which cannot be a class/function/members etc.)
	m_reservedWords[wxT("__asm")] = 1; m_reservedWords[wxT("__cdecl")] = 1; m_reservedWords[wxT("__declspec")] = 1; 
	m_reservedWords[wxT("__export")] = 1; m_reservedWords[wxT("__far16")] = 1; m_reservedWords[wxT("__fastcall")] = 1; 
	m_reservedWords[wxT("__fortran")] = 1; m_reservedWords[wxT("__import")] = 1; m_reservedWords[wxT("__asm")] = 1; 
	m_reservedWords[wxT("__pascal")] = 1; m_reservedWords[wxT("__stdcall")] = 1; m_reservedWords[wxT("__rtti")] = 1; 
	m_reservedWords[wxT("_asm")] = 1; m_reservedWords[wxT("_cdecl")] = 1; m_reservedWords[wxT("__except")] = 1; 
	m_reservedWords[wxT("_export")] = 1; m_reservedWords[wxT("_far16")] = 1; m_reservedWords[wxT("_fastcall")] = 1; 
	m_reservedWords[wxT("__finally")] = 1; m_reservedWords[wxT("_fortran")] = 1; m_reservedWords[wxT("_import")] = 1; 
	m_reservedWords[wxT("_pascal")] = 1; m_reservedWords[wxT("_stdcall")] = 1; m_reservedWords[wxT("__thread")] = 1; 
	m_reservedWords[wxT("__try")] = 1; m_reservedWords[wxT("asm")] = 1; m_reservedWords[wxT("auto")] = 1; 
	m_reservedWords[wxT("break")] = 1; m_reservedWords[wxT("case")] = 1; 
	m_reservedWords[wxT("catch")] = 1; m_reservedWords[wxT("cdecl")] = 1; 
	m_reservedWords[wxT("class")] = 1; m_reservedWords[wxT("const")] = 1; m_reservedWords[wxT("const_cast")] = 1; 
	m_reservedWords[wxT("continue")] = 1; m_reservedWords[wxT("default")] = 1; m_reservedWords[wxT("delete")] = 1; 
	m_reservedWords[wxT("do")] = 1; m_reservedWords[wxT("dynamic_cast")] = 1; 
	m_reservedWords[wxT("else")] = 1; m_reservedWords[wxT("enum")] = 1; m_reservedWords[wxT("explicit")] = 1; 
	m_reservedWords[wxT("extern")] = 1; m_reservedWords[wxT("false")] = 1; m_reservedWords[wxT("float")] = 1; 
	m_reservedWords[wxT("for")] = 1; m_reservedWords[wxT("friend")] = 1; m_reservedWords[wxT("goto")] = 1; 
	m_reservedWords[wxT("if")] = 1; m_reservedWords[wxT("inline")] = 1; 
	m_reservedWords[wxT("mutable")] = 1; m_reservedWords[wxT("namespace")] = 1; 
	m_reservedWords[wxT("new")] = 1; m_reservedWords[wxT("pascal")] = 1; m_reservedWords[wxT("private")] = 1; 
	m_reservedWords[wxT("protected")] = 1; m_reservedWords[wxT("public")] = 1; m_reservedWords[wxT("register")] = 1; 
	m_reservedWords[wxT("reinterpret_cast")] = 1; m_reservedWords[wxT("return")] = 1;  
	m_reservedWords[wxT("sizeof")] = 1; m_reservedWords[wxT("static")] = 1; 
	m_reservedWords[wxT("static_cast")] = 1; m_reservedWords[wxT("struct")] = 1; m_reservedWords[wxT("switch")] = 1; 
	m_reservedWords[wxT("template")] = 1; m_reservedWords[wxT("this")] = 1; m_reservedWords[wxT("throw")] = 1; 
	m_reservedWords[wxT("true")] = 1; m_reservedWords[wxT("try")] = 1; m_reservedWords[wxT("typedef")] = 1; 
	m_reservedWords[wxT("typeid")] = 1; m_reservedWords[wxT("typename")] = 1; m_reservedWords[wxT("union")] = 1; 
	m_reservedWords[wxT("virtual")] = 1; 
	m_reservedWords[wxT("volatile")] = 1; m_reservedWords[wxT("wchar_t")] = 1; m_reservedWords[wxT("while")] = 1;

	// Initialise the braces map
	m_braces['<'] = '>';
	m_braces['('] = ')';
	m_braces['['] = ']';
	m_braces['{'] = '}';

	
	// C++ / C auto complete delimiters for tokens 
	std::vector<wxString> delimArr;
	delimArr.push_back(wxT("::"));
	delimArr.push_back(wxT("->"));
	delimArr.push_back(wxT("."));
	SetAutoCompDeliemters(delimArr);
}

/// Destructor
Language::~Language()
{
}

bool Language::IsReservedWord(const wxString &word)
{
	std::map<wxString, int>::const_iterator iter = m_reservedWords.find(word);
	return (iter != m_reservedWords.end());
}

/// Return the visible scope until pchStopWord is encountered
wxString Language::GetScope(const wxString& srcString, const wxString& stopWord)
{
	std::vector<wxString> scope_stack;
	wxString currScope;
	int type;
	
	// Initialize the scanner with the string to search 
	const wxCharBuffer scannerText =  _C(srcString);
	m_scanner->SetText( scannerText.data());
	bool changedLine = false;
	bool consumeLine = false;
	int curline = 0;
	while(true)
	{
		type = m_scanner->yylex();
		

		// Eof ?
		if(type == 0)
		{
			if(!currScope.empty())
				scope_stack.push_back(currScope);
			break;
		}

		// eat up all tokens until next line
		if( consumeLine && m_scanner->lineno() == curline)
			continue;
	
		consumeLine = false;

		// Get the current line number, it will help us detect preprocessor lines
		changedLine = (m_scanner->lineno() > curline);
		curline = m_scanner->lineno();

		// For debug purposes
		wxString word = _U(m_scanner->YYText());

		// If stop word is provided try to Match it
		if(stopWord.IsEmpty() == false)
		{
			if(stopWord == _U(m_scanner->YYText()))
			{
				currScope += wxT(" ");
				currScope += _U(m_scanner->YYText());
				scope_stack.push_back(currScope);
				break;
			}
		}
		switch(type)
		{
		case (int)'{':
			currScope += wxT("\n");
			scope_stack.push_back(currScope);
			currScope = wxT("{\n");
			break;
		case (int)'}':

			// Discard the current scope since it is completed
			if( !scope_stack.empty() )
			{
				currScope = scope_stack.back();
				scope_stack.pop_back();
				currScope += wxT("\n{}\n");
			}
			else
				currScope = wxEmptyString;
			break;
		case (int)'#':
			if(changedLine)
			{
				// We are at the start of a new line
				// consume everything until new line is found or end of text
				consumeLine = true;
				break;
			}
		default:
			currScope += wxT(" ");
			currScope += _U(m_scanner->YYText());
			break;
		}
	}
	
	m_scanner->Reset();

	if(scope_stack.empty())
		return srcString;

	currScope = wxEmptyString;
	size_t i = 0;
	for(; i < scope_stack.size(); i++)
		currScope += scope_stack[i];

	// if the current scope is not empty, terminate it with ';' and return
	if( currScope.IsEmpty() == false )
	{
		currScope += wxT(";");
		return currScope;
	}

	return srcString;
}

/// Match the closing brace for ch
bool Language::Match(const char& ch, wxString &s, const wxString& sourceString)
{
	std::map<char, char>::iterator iter = m_braces.find(ch);
	if( iter == m_braces.end() )
		return false;

	char chFindChar = iter->second;

	std::stack<char> bs; // braces stack
	int type(0);

	bs.push(ch);

	if(sourceString.IsEmpty() == false)
	{
		m_scanner->Reset();

		// Set the scanner text
		const wxCharBuffer text = _C(sourceString);
		m_scanner->SetText( text.data() );
	}

	while(true)
	{
		type = m_scanner->yylex();
		if(type == 0)
		{
			return false;
		}

		switch( type )
		{
		case '<':
		case '[':
		case '{':
		case '(':
			{
				// Increase the depth
				bs.push(static_cast<char>(type));

				// Update the closing brace incase it is different 
				chFindChar = m_braces.find(static_cast<char>(type))->second;
				s += _U(m_scanner->YYText());

				// Remove the space
				s.erase(s.find_last_not_of(wxT(" "))+1);
				break;
			}
		case '>':
		case ')':
		case ']':
		case '}':
			{
				if(type == chFindChar)
				{
					// Redeuce the nesting depth
					bs.pop();
			
					// Remove the space
					s.erase(s.find_last_not_of(wxT(" "))+1);
					s += _U(m_scanner->YYText());

					// Do we have more barces to close?
					if(bs.empty())
						return true;

					// More braces to match, update the chFindChar
					chFindChar = m_braces.find(bs.top())->second;
					break;
				}
				else
					// We find a closing brace that does not match the last 
					// open one 
					return false;
			}
		default:
			{
				s += _U(m_scanner->YYText());
				s += wxT(" ");	
				break;
			}
		}
					
	}
}

/// Get the Qualifier of the identifier specified in the line, if no Match is found return empty string
wxString Language::GetVarQualifier(const wxString& identifier, const wxString& srcString, const bool &bVarIsFunc /* false */)
{
	// State table
	// 0 - Accept all tokens
	// 1 - Expecting identifier only
	// 2 - Assign operator was detected, we now accept every thing until we Match 
	//     the following: ',' (Comma) or '{expression}', or '(expression)'
	// 3 - Special operator was detected (* or &) they are added to a special container
	//     which will be added to the qualifier incase the end state was 3

	int flag = 1;
	int state = 0;
	int type;
	std::vector<wxString> stack;
	wxString s;
	wxString specialOperators;
	
	const wxCharBuffer text = _C(srcString);
	m_scanner->SetText(text.data());

	// First get all the valid tokens from the line
	while(flag)
	{
		if( (type = m_scanner->yylex()) == 0)
		{
			// We reached end of buffer but we didnt manage to find our pchIdentifier!!
			flag = 0;
			break;
		}

		const wxCharBuffer id = _C(identifier);

		if(strcmp(m_scanner->YYText(), id.data()) == 0)
			break;
		
		switch(type)
		{
		case IDENTIFIER:
			switch(state)
			{
			case 0: case 3: default:
				stack.push_back(_U(m_scanner->YYText()));
				STATE(0)
				break;
			case 1:
				// Previous was CLCL (::)
				s = stack.back();
				stack.pop_back();
				s += _U(m_scanner->YYText());
				stack.push_back(s);
				STATE(0)
				break;
			case 2:
				break;
			}
			break;
		case lexCONST:
			// A trailer const, something like 'const string& const my_str;'
			if(state == 1 || state == 3)
			{
				specialOperators << wxT(" ") << _U(m_scanner->YYText()) << wxT(" ");
			}
			else if(state == 0)
			{
				stack.push_back(_U(m_scanner->YYText()));
				STATE(0)
			}
			break;
		case (int)'*': case (int)'&':
			STATE(3)
			specialOperators += _U(m_scanner->YYText());
			break;
		case (int)',':
			// In this case, we pop the last input from the stack and clear the 
			// operator container
			CHECK_STATE(0);
			if(!stack.empty())
				stack.pop_back();
			specialOperators.erase();
			STATE(0)
			break;
		case (int)'=':
			CHECK_STATE(0)
			STATE(2)
			break;
		case lexDOUBLE:
		case lexINT:
		case lexSTRUCT:
		case lexLONG:
		case REGISTER:
		case lexCHAR:
		case UNION:
		case lexFLOAT:
		case lexSHORT:
		case UNSIGNED:
		case SIGNED:
		case VOLATILE:
		case STATIC:
			CHECK_STATE(0)
			stack.push_back(_U(m_scanner->YYText()));
			STATE(0)
			break;
		case (int)'<':
			CHECK_STATE(0)
			if(!stack.empty())
			{
				s = stack.back();
				stack.pop_back();
				s += wxT("<");
			}
			else
				s = wxT("<");
			
			if(!Match('<', s))
			{
				flag = 0;
				break;
			}
			stack.push_back(s);
			STATE(0)
			break;
		case (int)'{':
			CHECK_STATE(2)
			if(!stack.empty())
			{
				s = stack.back();
				stack.pop_back();
				s += wxT("{");
			}
			else
				s = wxT("{");
			if(!Match('{', s))
			{
				flag = 0;
				break;
			}
			stack.push_back(s);
			STATE(0)
			break;
		case (int)'(':
			CHECK_STATE2(0, 2);
			if(!stack.empty())
			{
				s = stack.back();
				stack.pop_back();
				s += wxT("(");
			}
			else
				s = wxT("(");
			if(!Match('(', s))
			{
				flag = 0;
				break;
			}
			stack.push_back(s);
			STATE(0)
			break;
		case CLCL:
			CHECK_STATE(0);
			if(!stack.empty())
			{
				s = stack.back(); 
				stack.pop_back();
				s += wxT("::");
			}
			else
				s = wxT("::");
			stack.push_back(s);
			STATE(1);
			break;
		case (int)';':
			// Clear the stack
			stack.clear(); specialOperators.erase();
			STATE(0);
			break;
		case lexVOID:
			if(bVarIsFunc)
			{
				// In case the var is function, we allow void
				CHECK_STATE(0);
				stack.push_back(_U(m_scanner->YYText()));
				STATE(0);
				break;
			}
			else
			{
				stack.clear();
				STATE(0);
				break;
			}
		default:
			// We got any-non valid token from the lexer, clear the stack
			stack.clear();
			STATE(0);
			break;
		}
	}
	if(!flag || stack.empty())
	{
		m_scanner->Reset();
		return wxEmptyString;
	}

	wxString qual = stack.back();

	// make sure that the qualifier is has a valid name
	wxChar ch( qual[0] );
	static wxString validPrefix(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"));
	if(-1 == validPrefix.Find(ch))
	{
		m_scanner->Reset();
		return wxEmptyString;
	}

	// Make sure that the qual is not a reserved word
	if(IsReservedWord(qual))
	{
		m_scanner->Reset();
		return wxEmptyString;
	}

	stack.pop_back();	// Physically remove it
	switch(state)
	{
	case 1:
		// We got something like:
		// Class::Identifier
		// So, the Qualifier is the 2nd on the stack and not
		// the first one
		if(stack.empty())
		{
			m_scanner->Reset();
			return wxEmptyString;
		}
		qual = stack.back();
		break;
	case 2:
		// Last token was '='
		m_scanner->Reset();
		return wxEmptyString;
	default:
		break;
	}

	// Check if there are any special operators that should be 
	// added to the qualifier
	if(!specialOperators.empty())
		qual += specialOperators;
	
	m_scanner->Reset();

	return qual;
}

wxString Language::ResolveCppCast(const wxString& expression)
{
	wxString s = expression;
	int start = (int)s.find_first_not_of(wxT("("));
	if(start < 0)
		return wxEmptyString;
	int end = (int)s.find_first_not_of(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890*&:_"), start);
	if(end < 0)
		return wxEmptyString;
	return expression.substr(start, end-start);
}

bool Language::NextToken(wxString &token, wxString &delim, wxString &srcStr)
{
	if(srcStr.IsEmpty())
		return false;

	if(m_delimArr.empty())
		return false;

	// Find the closest delimiter
	int pos(-1);
	for(size_t i=0; i<m_delimArr.size(); i++)
	{
		int curPos(0);
		curPos = srcStr.Find(m_delimArr[i].GetData());
		
		// two conditions:
		// 1. the new operator was found and it is closer to begin of string, we replace the match
		// 2. operator was found and it is the first time we have a match
		if((curPos < pos && curPos >=0 ) || (pos < 0 && curPos >= 0))
		{
			pos = curPos;
			delim = m_delimArr[i];
		}
	}


	if(pos <= 0)
	{
		token = srcStr;
		srcStr.Empty();
		return true;
	}

	token = srcStr.Mid(0, pos);
	srcStr.Remove(0, pos + delim.Length());
	return true;
}

void Language::SetAutoCompDeliemters(const std::vector<wxString> &delimArr)
{
	m_delimArr = delimArr;
}

bool Language::IsPointer(const wxString &qualifier)
{
	wxString tmp(qualifier);

	// Trim right
	tmp.erase(tmp.find_last_not_of(wxT("\n\r\v\t "))+1);

	if(tmp.Last() == wxT('*'))
		return true;
	return false;
}

wxString Language::ProcessExpression(const wxString& stmt, wxString & parent, bool calltipContext, const wxString& scope, const wxString& scopeName)
{
	wxString statement( stmt );

	// Trim whitespace from right and left
	static wxString trimString(wxT("{};\r\n\t\v "));

	statement.erase(0, statement.find_first_not_of(trimString)); 
	statement.erase(statement.find_last_not_of(trimString)+1);

	// First token is handled sepratly
	wxString word;
	wxString oper;
	wxString str;
	wxString left;
	wxString qualifier;
	bool isPointer(false);

	std::vector<TagEntry> tags;
	
	if( !NextToken(word, oper, statement) )
	{
		return wxEmptyString;
	}

	// Trim whitespaces and such
	word.erase(0, word.find_first_not_of(trimString)); 
	word.erase(word.find_last_not_of(trimString)+1);
	
	// Make sure that word is a single token
	wxArrayString delims;
	delims.Add(wxT(" "));
	delims.Add(wxT("\n"));
	delims.Add(wxT("\r\n"));

	StringTokenizer tok(word, delims);
	word = tok.Last();

	// try to resolve C-Style casting
	if( word.StartsWith(wxT("("), &left) )
	{
		if( Match(wxT('('), str, left) )
		{
			wxString truncStr(wxT("(") + str);
			
			// Get qualifier name from the truncated string
			qualifier = ResolveCppCast( truncStr );

			qualifier.erase(0, qualifier.find_first_not_of(wxT("*& "))); 

			// Before we do trim right, test whether this qualifier is a 
			// pointer
			isPointer = IsPointer(qualifier);
			qualifier.erase(qualifier.find_last_not_of(wxT("*& "))+1);
		}
		else
			return wxEmptyString;
	}
	else
	{
		//---------------------------------------------------------------------		
		// TODO::
		// check for other casting style
		// (static_cast, dynamic_cast, reinterpret_cast, const_cast)
		//---------------------------------------------------------------------

		// try to find word qualifier either from local scope or from
		// glocal scope
		wxString tmpScope(scope);
		wxString tmpScopeName(scopeName);
		bool foundThisKeyword(false);

		// get the qualifier of 'word'
		qualifier = GetWordQualifier( word, scope, scopeName, isPointer );
		if( qualifier.IsEmpty() )
			return wxEmptyString;
	
		// do some validations
		if(isPointer && oper != wxT("->"))
		{
			DEBUG_MSG(wxT("cant use operator '") << oper << wxT("' on a pointer !"));
			return wxEmptyString;
		}

		if(!isPointer && oper == wxT("->"))
		{
			DEBUG_MSG(wxT("cant use operator '") << oper << wxT("' on a non pointer qualifier"));
			return wxEmptyString;
		}
		
		// the parent of the first token is empty
		parent = wxT("");
		word.Empty();
		oper.Empty();

		while( NextToken( word, oper, statement) )
		{
			tmpScopeName = qualifier;
			parent = qualifier;

			if( calltipContext && statement.IsEmpty() )
				// We can end here for calltip context 
				break;

			tags.clear();

			// incase word is a function, remove the braces
			word = word.BeforeFirst(wxT('('));
			
			TagsManagerST::Get()->GetMembers( tmpScopeName, word, tags );
			qualifier = GetTagQualifier( tags, isPointer );
			
			if( qualifier.IsEmpty())
				return wxEmptyString;
			
			if(oper == wxT("::") && (tags[0].GetKind() == wxT("function") || tags[0].GetKind() == wxT("prototype")))
			{
				// A function cant be followed by a scope operator
				DEBUG_MSG(wxT("A function can not be followed by a scope operator ( :: )") );
				return wxEmptyString;
			}

			// test whether our qualifier is a pointer
			isPointer = IsPointer(qualifier);

			//----------------------------------------------------------------------
			// Incase the word we are searching for, is 'this', we override the 
			// IsPointer() function result: If word is 'this' it is set to true,
			// elss if it is '*this' it is set to false
			//----------------------------------------------------------------------
			if(word == wxT("this"))
			{
				if( foundThisKeyword )
				{
					DEBUG_MSG(wxT("Expressions can not be evaluated, you cant use 'this' more then once"))
					return wxEmptyString;
				}
				foundThisKeyword = true;
				isPointer = true;
			}
			
			if(word == wxT("*this"))
			{
				if( foundThisKeyword )
				{
					DEBUG_MSG(wxT("Expressions can not be evaluated, you cant use 'this' more then once"))
						return wxEmptyString;
				}
				foundThisKeyword = true;
				isPointer = false;
			}

			if(isPointer && oper != wxT("->"))
			{
				DEBUG_MSG(wxT("cant use operator '") << oper << wxT("' on a pointer !"));
				return wxEmptyString;
			}

			if(!isPointer && oper == wxT("->"))
			{
				DEBUG_MSG(wxT("cant use operator '") << oper << wxT("' on a non pointer qualifier"));
				return wxEmptyString;
			}

			// Remove any specifiers from the qualifier
			qualifier.erase(0, qualifier.find_first_not_of(wxT("*& "))); 
			isPointer = IsPointer(qualifier);
			qualifier.erase(qualifier.find_last_not_of(wxT("*& "))+1);

			// Print debug message
			DEBUG_MSG(wxT("Expression result is: ") << qualifier);

			// If we have more tokens, keep processing them
			// but now the parent is set the previous qualifier
			tmpScope.Empty();
			oper.Empty();
			word.Empty();
		}
	}
	return qualifier;
}

bool Language::ParseSignature(const wxString &signature, std::vector<std::pair<wxString, wxString> >& args, wxString & trailer)
{
	wxString src_string;
	wxString work_string;
	int type;

	src_string = signature.AfterFirst(wxT('('));
	if(false == Match(wxT('('), work_string, src_string))
		return false;
	
	while((type = m_scanner->yylex()) != 0)
	{
		trailer << _U(m_scanner->YYText());
	}
	
	// Break the string into comma sperated tokens
	std::vector<std::pair<wxString, wxString> > arguments;
	wxString currArg;
	wxString identifier;

	const wxCharBuffer scannerText =  _C(work_string);
	m_scanner->SetText( scannerText.data());
	bool flag = true;

	while( flag )
	{
		type =m_scanner->yylex();
		if(type == 0) // EOF
			break;
	
		wxString token = _U(m_scanner->YYText());
		
		switch(type)
		{
		case (int)'<':
		case (int)'(':
		case (int)'[':
		case (int)'{':
			{
				// consume everything between the braces
				wxString tmpStr;
				if(false == Match(type, tmpStr))
					return false;
				break;
			}
		case (int)',':
		case (int)')':
			{
				// Add this pair <identifier:argument expression> to the vector
				std::pair<wxString, wxString> p;
				p.first = identifier;
				p.second = currArg;
				arguments.push_back( p );
				currArg.Empty();
				identifier.Empty();
				
				if(type == (int)')')
					flag = false;
				break;
			}
		case (int)'*':
			{
				currArg += token;
				break;
			}
		case IDENTIFIER:
			identifier = token;
		default:
			currArg += token;
			currArg += wxT(" ");
			break;
		}
	}

	// arguments contains set of parameters, 
	if(arguments.empty())
		return false;

	for(size_t i=0; i<arguments.size(); i++)
	{
		std::pair<wxString, wxString> p;

		// debug
		wxString s1 = arguments[i].second;
		wxString s2 = arguments[i].first;

		wxString qualifier = GetVarQualifier(arguments[i].first, arguments[i].second);

		if(qualifier.IsEmpty())
			return false;

		p.second = qualifier;
		p.first = arguments[i].first;
		args.push_back(p);
	}
	return true;
}

wxString Language::GetTagQualifier(std::vector<TagEntry> & tags, bool & isPtr )
{
	wxString qualifier;
	wxString tmpQualifer;
	size_t i=0;
	
	if(tags.empty())
		return wxEmptyString;

	if(tags.size() > 1)
	{
		// more than one tag, check that all are of the same kind
		wxString kind = tags[0].GetKind();
		for(i=0; i<tags.size(); i++)
		{
			if(kind != tags[i].GetKind())
				return wxEmptyString;
		}
	}

	for(i=0; i<tags.size(); i++)
	{
		tmpQualifer = Qualifier( tags[i], isPtr );
		if(tmpQualifer != qualifier && i != 0)
			return wxEmptyString;
		qualifier = tmpQualifer;
	}

	return qualifier;
}

wxString Language::Qualifier(const TagEntry& tag, bool & isPtr, bool truncateTemplate, wxString *fullQualifier )
{
	wxString qualifier;

	// If the tag is a function, we use its return value, else we use its qualifier
	// since ctags does not provide us with the return type of the member qualifier, 
	// we provide our own function that will extract it from the pattern
	wxString kind = tag.GetKind();

	if(kind == wxT("function") || kind == wxT("prototype"))
		qualifier = GetVarQualifier(tag.GetName(), tag.GetPattern(), true);
	else if(kind == wxT("member") || kind == wxT("local") || kind == wxT("variable"))
		qualifier = GetVarQualifier(tag.GetName(), tag.GetPattern());
	else if(kind == wxT("class") || kind == wxT("struct") || kind == wxT("union") || kind == wxT("namespace") || kind == wxT("typedef"))
		qualifier = tag.GetName();
	else
		return wxEmptyString;

	// Did we succeed?
	if(qualifier.IsEmpty())
		return wxEmptyString;

	if( fullQualifier )
		*fullQualifier = qualifier;

	// remove any prefix scope infront of the qualifier (e.g. std::vector )
	StringTokenizer t(qualifier, wxT("::"));
	qualifier = t.Last();
	
	// if the qualifier is template, remove any template specifiers

	if( truncateTemplate )
		qualifier = qualifier.BeforeFirst(wxT('<'));

	// does our qualifier is a real qualifier? or a typeref?
	// maybe our parent is a typeref?
	wxString sql;
	std::vector<TagEntry> tags;

	// remove specifiers from the qualifier before checking the database
	isPtr = IsPointer( qualifier );
	qualifier.erase(qualifier.find_last_not_of(wxT("*& "))+1);

	if( tag.GetKind() == wxT("typedef") || tag.GetKind() == wxT("macro") )
	{
		sql << wxT("SELECT * FROM TAGS WHERE name='") << qualifier << wxT("' AND typeref != ''");
		TagsManagerST::Get()->GetTagsBySQL( sql , tags );

		if( tags.empty() == false )
		{
			wxString tmpQualifer = tags[0].NameFromTyperef();
			if(tmpQualifer.IsEmpty() == false)
				qualifier = tmpQualifer;
		}
	}
	return qualifier;
}

wxString Language::GetWordQualifier( const wxString & word, const wxString & scope, const wxString & scopeName, bool & isPtr )
{
	std::vector<TagEntry> *localTags(NULL);
	std::vector<TagEntry> tags;
	wxString escapedName( word );

	DEBUG_START_TIMER(wxT("Testing for 'this' keyword") );

	// Support fot 'this' & '*this' keyword
	if(word == wxT("this") || word == wxT("*this")) 
	{
		if(scopeName.IsEmpty() || scopeName == wxT("<global>"))
		{
			DEBUG_MSG(wxT("'this' keyword must be in a class / struct context"));
			return wxEmptyString;
		}
		TagEntry tag;
		TagsManagerST::Get()->GetClassTagByName(scopeName, tag);
		if(tag.IsOk())
			return tag.GetKind();
		return wxEmptyString;
	}

	DEBUG_STOP_TIMER()

	// If we have local scope, we process it first
	if( scope.IsEmpty() == false )
	{
		// Get a list of tags from the current scope, first remove the non-visible scope from the 
		// row scope
		DEBUG_START_TIMER(wxT("Getting local scope ...") );
		wxString visibleScope = GetScope( scope, wxEmptyString );
		DEBUG_STOP_TIMER()

		DEBUG_START_TIMER(wxT("Processing local scope..") );
		localTags = TagsManagerST::Get()->ParseLocals( visibleScope );
		DEBUG_STOP_TIMER()

		// filter all non qualified names from the local scope,
		// consider flags (PartialMatch or ExactMatch)
		TagsManagerST::Get()->FilterResults( *localTags, word, tags, ExactMatch );
		DEBUG_MSG(wxT("Matched ") << static_cast<int>(tags.size()) << wxT(" in local scope"))

		delete localTags;

		// if we have a single match in the local scope, we use it
		if( tags.size() == 1)
			return GetTagQualifier( tags, isPtr );
	}

	// there are cases where 'name' is a function name (e.g. GetName()), 
	// if we will use this string, we will not find it in the database, 
	// since the function signature is kept separatley, so we remove it
	escapedName = escapedName.BeforeFirst(wxT('('));
	if(escapedName.IsEmpty()) 
		return wxEmptyString;

	wxString sql;
	sql << wxT("SELECT * FROM TAGS WHERE name='") << escapedName << wxT("'");
	TagsManagerST::Get()->GetTagsBySQL( sql, tags, wxT("macro") );
	return GetTagQualifier( tags, isPtr );
}

// return hover tip
void Language::GetHoverTip(const wxString & token, const wxString & scope, const wxString & scopeName, bool isFunc, std::vector<wxString> & tips)
{
	std::vector<TagEntry> *localTags(NULL);
	std::vector<TagEntry> tags1;
	std::vector<TagEntry> tags2;
	std::vector<TagEntry> members_tags;

	if( token.IsEmpty() )
		return;

	// try to get a match in local scope
	if( scope.IsEmpty() == false )
	{
		// Get a list of tags from the current scope, first remove the non-visible scope from the 
		// row scope
		DEBUG_START_TIMER(wxT("Getting local scope ...") );
		wxString visibleScope = LanguageST::Get()->GetScope(scope, wxEmptyString);
		DEBUG_STOP_TIMER()

		DEBUG_START_TIMER(wxT("Processing local scope..") );
		localTags = TagsManagerST::Get()->ParseLocals(visibleScope);
		DEBUG_STOP_TIMER()

		// filter all non qualified names from the local scope,
		FilterResults(*localTags, isFunc, token, tags2);

		// if the tag is a function
		DEBUG_MSG(wxT("Matched ") << static_cast<int>(tags2.size()) << wxT(" in local scope"))

		delete localTags;

		if( tags2.size() > 0 )
		{
			ConstructTips( tags2, tips );
			return;
		}
	}

	// if scope name is provided, try with the parent members
	if( scopeName.IsEmpty() == false /*&& scopeName != wxT("<global>")*/ )
	{
		wxString sql;
		sql << wxT("SELECT * FROM TAGS WHERE parent='") << scopeName << wxT("' AND name='") << token << wxT("'");
		TagsManagerST::Get()->GetTagsBySQL(sql, members_tags);

		// filter non matched tags
		FilterResults(members_tags, isFunc, token, tags2);

		// do we have a match?
		if( tags2.size() > 0 )
		{
			ConstructTips( tags2, tips );
			return;
		}
	}

	// ok, last attempt now: try the database with wide search !
	wxString sql;
	sql << wxT("SELECT * FROM TAGS WHERE NAME='") << token << wxT("' AND KIND != 'project'");
	TagsManagerST::Get()->GetTagsBySQL( sql, tags1 );
	FilterResults( tags1, isFunc, token, tags2 );

	// do we have a match?
	if( tags2.size() > 0 )
	{
		ConstructTips( tags2, tips );
		return;
	}
	tips.clear();
}

void Language::ConstructTips( std::vector<TagEntry> & tags, std::vector<wxString> & tips )
{
	for(size_t i=0; i<tags.size() ; i++)
		tips.push_back( GetStringTip( tags[i] ) );
}

void Language::FilterResults(const std::vector<TagEntry> & src, bool isFunc, const wxString &name, std::vector<TagEntry> &tags)
{
	std::vector<TagEntry> tmptags;
	for(size_t i=0; i<src.size(); i++)
	{
		bool tagKindFunc = (src[i].GetKind() == wxT("prototype") || src[i].GetKind() == wxT("function"));
		if( src[i].GetName() == name )
		{
			if( isFunc )
			{
				if( tagKindFunc )
					tmptags.push_back( src[i] );
			}
			else
			{
				// need non functions only
				if(! tagKindFunc )
					tmptags.push_back( src[i] );
			}
		}
	}

	// remove duplicate entries
	std::map<wxString, TagEntry> m;
	for(size_t i=0; i<tmptags.size(); i++)
		m[GetStringTip(tmptags[i])] = tmptags[i];

	std::map<wxString, TagEntry>::iterator iter = m.begin();
	for(; iter != m.end(); iter++)
		tags.push_back( iter->second );
}

wxString Language::GetStringTip(TagEntry &tag)
{
	// format a tip from the tag
	wxString tip;
	wxString path;
	bool isPtr;
	wxString qualifier(wxT(""));
	wxString comment;
	if( TagsManagerST::Get()->GetParseComments() )
	{
		// get the comment string from the database
		comment = TagsManagerST::Get()->GetComment( tag.GetFile(), tag.GetLine() );
		if( comment.IsEmpty() == false )
			comment << wxT("\n");
	}

	if( tag.GetKind() == wxT("local") )
	{
		Qualifier( tag, isPtr, false, &qualifier );
		tip << comment 
			<< qualifier << wxT(" ")
			<< tag.GetName();
		return tip;
	}
	else if(tag.GetKind() == wxT("class") || tag.GetKind() == wxT("namespace") || tag.GetKind() == wxT("struct") ||
			tag.GetKind() == wxT("enum") ||  tag.GetKind() == wxT("union")     || tag.GetKind() == wxT("typedef") )
	{
		tip << comment << tag.GetKind() << wxT(" ") << tag.GetName();
		return tip;
	}
	else
	{
		if( tag.GetProject().IsEmpty() == false )
		{
			// the project is part of the path, remove it
			StringTokenizer tkz( tag.GetPath(), wxString( tag.GetProject() + wxT("::") ) );
			path = tkz.Last();
		}
		else
			path = tag.GetPath();

		Qualifier( tag, isPtr, false, &qualifier );
		tip << comment << qualifier << wxT(" ")
			<< path
			<< tag.GetSignature();
		return tip;
	}
}

void Language::ParseComments(const wxFileName &fileName, std::vector<DbRecordPtr> *comments)
{
	wxString content;
	try
	{
		wxFFile f(fileName.GetFullPath().GetData());
		if( !f.IsOpened() )
			return;

		// read the content of the file and parse it
		f.ReadAll( &content );
		f.Close();
	}
	catch( ... )
	{
		return;
	}

	m_scanner->Reset();
	m_scanner->SetText( _C(content) );
	m_scanner->KeepComment( 1 );

	int type( 0 );

	wxString comment(wxT(""));
	int line(-1);

	while( true )
	{
		type = m_scanner->yylex();
		if( type == 0 ) //eof
			break;

		
		// we keep only comments
		if( type == CPPComment )
		{
			// incase the previous comment was one line above this one,
			// concatenate them to a single comment
			if( m_scanner->lineno() - 1 == line )
			{
				comment << m_scanner->GetComment();
				line = m_scanner->lineno();
				m_scanner->ClearComment();
				continue;
			}

			// first time or no comment is buffer
			if( comment.IsEmpty() )
			{
				comment = m_scanner->GetComment();
				line = m_scanner->lineno();
				m_scanner->ClearComment();
				continue;
			}
			
			if( comment.IsEmpty() == false )
			{
				comments->push_back( static_cast<DbRecord*>( new Comment( comment, fileName.GetFullPath(), line - 1)) );
				comment.Empty();
				line = -1;
			}
			
			comments->push_back( static_cast<DbRecord*>( new Comment( m_scanner->GetComment(), fileName.GetFullPath(), m_scanner->lineno()-1)) );
			m_scanner->ClearComment();
		}
		else if( type == CComment )
		{
			comments->push_back( static_cast<DbRecord*>( new Comment( m_scanner->GetComment(), fileName.GetFullPath(), m_scanner->lineno()) ) );
			m_scanner->ClearComment();
		}
	}

	if( comment.IsEmpty() == false )
	{
		comments->push_back( static_cast<DbRecord*>( new Comment( comment, fileName.GetFullPath(), line - 1) ) );
	}
				
	// reset the scanner
	m_scanner->KeepComment( 0 );
	m_scanner->Reset();
}

} // codelite
