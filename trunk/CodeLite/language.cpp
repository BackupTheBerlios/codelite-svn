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


// Some useful macros
#define CHECK_STATE(x) { if(state != x) { flag = 0; break; } }
#define CHECK_STATE2(x, y) { if(state != x && state != y) { flag = 0; break; } }
#define STATE(x) { state = x; }

Language::Language()
: m_expression(wxEmptyString)
, m_scanner( new CppScanner() )
{
	// Init the operator map
	m_operatorMap[_T("()")]  = 1; m_operatorMap[_T("[]")]  = 1; m_operatorMap[_T("->")]  = 1;
	m_operatorMap[_T("++")]  = 1; m_operatorMap[_T("--")]  = 1; m_operatorMap[_T("+=")]  = 1;
	m_operatorMap[_T("-=")]  = 1; m_operatorMap[_T("<<=")] = 1; m_operatorMap[_T(">>=")] = 1;
	m_operatorMap[_T("->*")] = 1; m_operatorMap[_T("-=")]  = 1; m_operatorMap[_T("*=")]  = 1;
	m_operatorMap[_T("%=")]  = 1; m_operatorMap[_T("||")]  = 1; m_operatorMap[_T("|=")]  = 1;
	m_operatorMap[_T("&&")]  = 1; m_operatorMap[_T("&=")]  = 1; m_operatorMap[_T("^=")]  = 1;
	m_operatorMap[_T(".*")]  = 1; m_operatorMap[_T("==")]  = 1; m_operatorMap[_T("!=")]  = 1;
	m_operatorMap[_T("<=")]  = 1; m_operatorMap[_T("<<")]  = 1; m_operatorMap[_T(">>")]  = 1;
	m_operatorMap[_T(">=")]  = 1; m_operatorMap[_T("=")]   = 1; m_operatorMap[_T("<")]   = 1;
	m_operatorMap[_T(">")]   = 1;

	// Init the reserved words map (words which cannot be a class/function/members etc.)
	m_reservedWords[_T("__asm")] = 1; m_reservedWords[_T("__cdecl")] = 1; m_reservedWords[_T("__declspec")] = 1; 
	m_reservedWords[_T("__export")] = 1; m_reservedWords[_T("__far16")] = 1; m_reservedWords[_T("__fastcall")] = 1; 
	m_reservedWords[_T("__fortran")] = 1; m_reservedWords[_T("__import")] = 1; m_reservedWords[_T("__asm")] = 1; 
	m_reservedWords[_T("__pascal")] = 1; m_reservedWords[_T("__stdcall")] = 1; m_reservedWords[_T("__rtti")] = 1; 
	m_reservedWords[_T("_asm")] = 1; m_reservedWords[_T("_cdecl")] = 1; m_reservedWords[_T("__except")] = 1; 
	m_reservedWords[_T("_export")] = 1; m_reservedWords[_T("_far16")] = 1; m_reservedWords[_T("_fastcall")] = 1; 
	m_reservedWords[_T("__finally")] = 1; m_reservedWords[_T("_fortran")] = 1; m_reservedWords[_T("_import")] = 1; 
	m_reservedWords[_T("_pascal")] = 1; m_reservedWords[_T("_stdcall")] = 1; m_reservedWords[_T("__thread")] = 1; 
	m_reservedWords[_T("__try")] = 1; m_reservedWords[_T("asm")] = 1; m_reservedWords[_T("auto")] = 1; 
	m_reservedWords[_T("break")] = 1; m_reservedWords[_T("case")] = 1; 
	m_reservedWords[_T("catch")] = 1; m_reservedWords[_T("cdecl")] = 1; 
	m_reservedWords[_T("class")] = 1; m_reservedWords[_T("const")] = 1; m_reservedWords[_T("const_cast")] = 1; 
	m_reservedWords[_T("continue")] = 1; m_reservedWords[_T("default")] = 1; m_reservedWords[_T("delete")] = 1; 
	m_reservedWords[_T("do")] = 1; m_reservedWords[_T("dynamic_cast")] = 1; 
	m_reservedWords[_T("else")] = 1; m_reservedWords[_T("enum")] = 1; m_reservedWords[_T("explicit")] = 1; 
	m_reservedWords[_T("extern")] = 1; m_reservedWords[_T("false")] = 1; m_reservedWords[_T("float")] = 1; 
	m_reservedWords[_T("for")] = 1; m_reservedWords[_T("friend")] = 1; m_reservedWords[_T("goto")] = 1; 
	m_reservedWords[_T("if")] = 1; m_reservedWords[_T("inline")] = 1; 
	m_reservedWords[_T("mutable")] = 1; m_reservedWords[_T("namespace")] = 1; 
	m_reservedWords[_T("new")] = 1; m_reservedWords[_T("pascal")] = 1; m_reservedWords[_T("private")] = 1; 
	m_reservedWords[_T("protected")] = 1; m_reservedWords[_T("public")] = 1; m_reservedWords[_T("register")] = 1; 
	m_reservedWords[_T("reinterpret_cast")] = 1; m_reservedWords[_T("return")] = 1;  
	m_reservedWords[_T("sizeof")] = 1; m_reservedWords[_T("static")] = 1; 
	m_reservedWords[_T("static_cast")] = 1; m_reservedWords[_T("struct")] = 1; m_reservedWords[_T("switch")] = 1; 
	m_reservedWords[_T("template")] = 1; m_reservedWords[_T("this")] = 1; m_reservedWords[_T("throw")] = 1; 
	m_reservedWords[_T("true")] = 1; m_reservedWords[_T("try")] = 1; m_reservedWords[_T("typedef")] = 1; 
	m_reservedWords[_T("typeid")] = 1; m_reservedWords[_T("typename")] = 1; m_reservedWords[_T("union")] = 1; 
	m_reservedWords[_T("virtual")] = 1; 
	m_reservedWords[_T("volatile")] = 1; m_reservedWords[_T("wchar_t")] = 1; m_reservedWords[_T("while")] = 1;

	// Initialise the braces map
	m_braces['<'] = '>';
	m_braces['('] = ')';
	m_braces['['] = ']';
	m_braces['{'] = '}';

	
	// C++ / C auto complete delimiters for tokens 
	std::vector<wxString> delimArr;
	delimArr.push_back(_T("::"));
	delimArr.push_back(_T("->"));
	delimArr.push_back(_T("."));
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
				currScope += _T(" ");
				currScope += _U(m_scanner->YYText());
				scope_stack.push_back(currScope);
				break;
			}
		}
		switch(type)
		{
		case (int)'{':
			currScope += _T("\n");
			scope_stack.push_back(currScope);
			currScope = _T("{\n");
			break;
		case (int)'}':

			// Discard the current scope since it is completed
			if( !scope_stack.empty() )
			{
				currScope = scope_stack.back();
				scope_stack.pop_back();
				currScope += _T("\n{}\n");
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
			currScope += _T(" ");
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
		currScope += _T(";");
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
				s.erase(s.find_last_not_of(_T(" "))+1);
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
					s.erase(s.find_last_not_of(_T(" "))+1);
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
				s += _T(" ");	
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
				specialOperators << _T(" ") << _U(m_scanner->YYText()) << _T(" ");
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
				s += _T("<");
			}
			else
				s = _T("<");
			
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
				s += _T("{");
			}
			else
				s = _T("{");
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
				s += _T("(");
			}
			else
				s = _T("(");
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
				s += _T("::");
			}
			else
				s = _T("::");
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
	static wxString validPrefix(_T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"));
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
	int start = (int)s.find_first_not_of(_T("("));
	if(start < 0)
		return wxEmptyString;
	int end = (int)s.find_first_not_of(_T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890*&:_"), start);
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
	tmp.erase(tmp.find_last_not_of(_T("\n\r\v\t "))+1);

	if(tmp.Last() == _T('*'))
		return true;
	return false;
}

wxString Language::ProcessExpression(const wxString& stmt, wxString & parent, bool calltipContext, const wxString& scope, const wxString& scopeName)
{
	wxString statement( stmt );

	// Trim whitespace from right and left
	static wxString trimString(_T("{};\r\n\t\v "));

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
	delims.Add(_T(" "));
	delims.Add(_T("\n"));
	delims.Add(_T("\r\n"));

	StringTokenizer tok(word, delims);
	word = tok.Last();

	// try to resolve C-Style casting
	if( word.StartsWith(_T("("), &left) )
	{
		if( Match(_T('('), str, left) )
		{
			wxString truncStr(_T("(") + str);
			
			// Get qualifier name from the truncated string
			qualifier = ResolveCppCast( truncStr );

			qualifier.erase(0, qualifier.find_first_not_of(_T("*& "))); 

			// Before we do trim right, test whether this qualifier is a 
			// pointer
			isPointer = IsPointer(qualifier);
			qualifier.erase(qualifier.find_last_not_of(_T("*& "))+1);
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
		if(isPointer && oper != _T("->"))
		{
			return wxEmptyString;
		}

		if(!isPointer && oper == _T("->"))
		{
			return wxEmptyString;
		}
		
		// the parent of the first token is empty
		parent = _T("");
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
			word = word.BeforeFirst(_T('('));
			
			TagsManagerST::Get()->GetMembers( tmpScopeName, word, tags );
			qualifier = GetTagQualifier( tags, isPointer );
			
			if( qualifier.IsEmpty())
				return wxEmptyString;
			
			if(oper == _T("::") && (tags[0].GetKind() == _T("function") || tags[0].GetKind() == _T("prototype")))
			{
				// A function cant be followed by a scope operator
				return wxEmptyString;
			}

			// test whether our qualifier is a pointer
			isPointer = IsPointer(qualifier);

			//----------------------------------------------------------------------
			// Incase the word we are searching for, is 'this', we override the 
			// IsPointer() function result: If word is 'this' it is set to true,
			// elss if it is '*this' it is set to false
			//----------------------------------------------------------------------
			if(word == _T("this"))
			{
				if( foundThisKeyword )
				{
					return wxEmptyString;
				}
				foundThisKeyword = true;
				isPointer = true;
			}
			
			if(word == _T("*this"))
			{
				if( foundThisKeyword )
				{
						return wxEmptyString;
				}
				foundThisKeyword = true;
				isPointer = false;
			}

			if(isPointer && oper != _T("->"))
			{
				return wxEmptyString;
			}

			if(!isPointer && oper == _T("->"))
			{
				return wxEmptyString;
			}

			// Remove any specifiers from the qualifier
			qualifier.erase(0, qualifier.find_first_not_of(_T("*& "))); 
			isPointer = IsPointer(qualifier);
			qualifier.erase(qualifier.find_last_not_of(_T("*& "))+1);

			// Print debug message

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

	src_string = signature.AfterFirst(_T('('));
	if(false == Match(_T('('), work_string, src_string))
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
			currArg += _T(" ");
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

	if(kind == _T("function") || kind == _T("prototype"))
		qualifier = GetVarQualifier(tag.GetName(), tag.GetPattern(), true);
	else if(kind == _T("member") || kind == _T("local") || kind == _T("variable"))
		qualifier = GetVarQualifier(tag.GetName(), tag.GetPattern());
	else if(kind == _T("class") || kind == _T("struct") || kind == _T("union") || kind == _T("namespace") || kind == _T("typedef"))
		qualifier = tag.GetName();
	else
		return wxEmptyString;

	// Did we succeed?
	if(qualifier.IsEmpty())
		return wxEmptyString;

	if( fullQualifier )
		*fullQualifier = qualifier;

	// remove any prefix scope infront of the qualifier (e.g. std::vector )
	StringTokenizer t(qualifier, _T("::"));
	qualifier = t.Last();
	
	// if the qualifier is template, remove any template specifiers

	if( truncateTemplate )
		qualifier = qualifier.BeforeFirst(_T('<'));

	// does our qualifier is a real qualifier? or a typeref?
	// maybe our parent is a typeref?
	wxString sql;
	std::vector<TagEntry> tags;

	// remove specifiers from the qualifier before checking the database
	isPtr = IsPointer( qualifier );
	qualifier.erase(qualifier.find_last_not_of(_T("*& "))+1);

	if( tag.GetKind() == _T("typedef") || tag.GetKind() == _T("macro") )
	{
		sql << _T("SELECT * FROM TAGS WHERE name='") << qualifier << _T("' AND typeref != ''");
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

	// Support fot 'this' & '*this' keyword
	if(word == _T("this") || word == _T("*this")) 
	{
		if(scopeName.IsEmpty() || scopeName == _T("<global>"))
		{
			return wxEmptyString;
		}
		TagEntry tag;
		TagsManagerST::Get()->GetClassTagByName(scopeName, tag);
		if(tag.IsOk())
			return tag.GetKind();
		return wxEmptyString;
	}

	// If we have local scope, we process it first
	if( scope.IsEmpty() == false )
	{
		// Get a list of tags from the current scope, first remove the non-visible scope from the 
		// row scope
		wxString visibleScope = GetScope( scope, wxEmptyString );
		localTags = TagsManagerST::Get()->ParseLocals( visibleScope );
		if( !localTags){
			return wxEmptyString;
		}

		// filter all non qualified names from the local scope,
		// consider flags (PartialMatch or ExactMatch)
		TagsManagerST::Get()->FilterResults( *localTags, word, tags, ExactMatch );

		delete localTags;

		// if we have a single match in the local scope, we use it
		if( tags.size() == 1)
			return GetTagQualifier( tags, isPtr );
	}

	// there are cases where 'name' is a function name (e.g. GetName()), 
	// if we will use this string, we will not find it in the database, 
	// since the function signature is kept separatley, so we remove it
	escapedName = escapedName.BeforeFirst(_T('('));
	if(escapedName.IsEmpty()) 
		return wxEmptyString;

	wxString sql;
	sql << _T("SELECT * FROM TAGS WHERE name='") << escapedName << _T("'");
	TagsManagerST::Get()->GetTagsBySQL( sql, tags, _T("macro") );
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
		wxString visibleScope = LanguageST::Get()->GetScope(scope, wxEmptyString);
		localTags = TagsManagerST::Get()->ParseLocals(visibleScope);
		// filter all non qualified names from the local scope,
		FilterResults(*localTags, isFunc, token, tags2);

		// if the tag is a function

		delete localTags;

		if( tags2.size() > 0 )
		{
			ConstructTips( tags2, tips );
			return;
		}
	}

	// if scope name is provided, try with the parent members
	if( scopeName.IsEmpty() == false /*&& scopeName != _T("<global>")*/ )
	{
		wxString sql;
		sql << _T("SELECT * FROM TAGS WHERE parent='") << scopeName << _T("' AND name='") << token << _T("'");
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
	sql << _T("SELECT * FROM TAGS WHERE NAME='") << token << _T("' AND KIND != 'project'");
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
		bool tagKindFunc = (src[i].GetKind() == _T("prototype") || src[i].GetKind() == _T("function"));
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
	wxString qualifier(_T(""));
	wxString comment;
	if( TagsManagerST::Get()->GetParseComments() )
	{
		// get the comment string from the database
		comment = TagsManagerST::Get()->GetComment( tag.GetFile(), tag.GetLine() );
		if( comment.IsEmpty() == false )
			comment << _T("\n");
	}

	if( tag.GetKind() == _T("local") )
	{
		Qualifier( tag, isPtr, false, &qualifier );
		tip << comment 
			<< qualifier << _T(" ")
			<< tag.GetName();
		return tip;
	}
	else if(tag.GetKind() == _T("class") || tag.GetKind() == _T("namespace") || tag.GetKind() == _T("struct") ||
			tag.GetKind() == _T("enum") ||  tag.GetKind() == _T("union")     || tag.GetKind() == _T("typedef") )
	{
		tip << comment << tag.GetKind() << _T(" ") << tag.GetName();
		return tip;
	}
	else
	{
		if( tag.GetProject().IsEmpty() == false )
		{
			// the project is part of the path, remove it
			StringTokenizer tkz( tag.GetPath(), wxString( tag.GetProject() + _T("::") ) );
			path = tkz.Last();
		}
		else
			path = tag.GetPath();

		Qualifier( tag, isPtr, false, &qualifier );
		tip << comment << qualifier << _T(" ")
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

	wxString comment(_T(""));
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

