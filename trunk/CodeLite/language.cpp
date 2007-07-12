#include "precompiled_header.h"

#include "language.h"
#include "variable.h"
#include "function.h"
#include "ctags_manager.h"
#include "y.tab.h"
#include <wx/stopwatch.h>
#include <wx/ffile.h>

#ifdef __VISUALC__
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif 

//===============================================================
//defined in generated files from the yacc grammar:
//cpp_scope_garmmar.y
//cpp_variables_grammar.y
//expr_garmmar.y

extern std::string get_scope_name(const std::string &in);
extern ExpressionResult &parse_expression(const std::string &in);
extern void get_variables(const std::string &in, VariableList &li);
extern void get_functions(const std::string &in, FunctionList &li);

//===============================================================

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

bool Language::ProcessExpression(const wxString& stmt, const wxString& text, 
								 wxString &typeName, //output
								 wxString &typeScope)//output
{
	ExpressionResult result;
	wxString statement( stmt );
	bool evaluationSucceeded = true;

	// Trim whitespace from right and left
	static wxString trimString(_T("{};\r\n\t\v "));

	statement.erase(0, statement.find_first_not_of(trimString)); 
	statement.erase(statement.find_last_not_of(trimString)+1);
	wxString dbgStmnt = statement;

	// First token is handled sepratly
	wxString word;
	wxString oper;
	wxString str;
	wxString left;
	wxString qualifier;

	std::vector<TagEntry> tags;
	wxString visibleScope = GetScope( text, wxEmptyString );
	wxString scopeName = GetScopeName(visibleScope);
	wxLogMessage(wxT("Current scope: [") + scopeName + wxT("]"));

	wxString parentTypeName, parentTypeScope;
	while(NextToken(word, oper, statement))
	{
		result = ParseExpression(word);
		//parsing failed?
		if(result.m_name.empty())
		{
			wxLogMessage(wxT("Failed to parse expression: ") + word);
			evaluationSucceeded = false;
			break;
		}

		//no tokens before this, what we need to do now, is find the TagEntry
		//that corrseponds to the result
		if(result.m_isaType)
		{
			//-------------------------------------------
			// Handle type (usually when casting is found
			//--------------------------------------------

			typeScope = result.m_scope.empty() ? wxT("<global>") : _U(result.m_scope.c_str());
			if(oper == wxT("->") || oper == wxT("."))
			{
				wxLogMessage(wxT("Illegal usage of '->', '.' operator on a type :") + _U(result.m_name.c_str()));
				evaluationSucceeded = false;
				break;
			} // if(oper == wxT("->") || oper == wxT("."))  
			typeName = _U(result.m_name.c_str());
		} // if(result.m_isaType) 
		else if(result.m_isThis)
		{
			//-----------------------------------------
			// special handle for 'this' keyword
			//-----------------------------------------

			typeScope = result.m_scope.empty() ? wxT("<global>") : _U(result.m_scope.c_str());
			if(scopeName == wxT("<global>"))
			{
				wxLogMessage(wxT("Illegal use of 'this' keyword outside of valid scope"));
				evaluationSucceeded = false;
				break;
			} // if(typeScope == wxT("<global>")) 

			if(oper == wxT("::"))
			{
				wxLogMessage(wxT("Illegal usage of '::' operator on 'this' keyword"));
				evaluationSucceeded = false;
				break;
			} // if(oper == wxT("::"))

			if(result.m_isPtr && oper == wxT("."))
			{
				wxLogMessage(wxT("Did u mean to use '->' instead of '.'?"));
				evaluationSucceeded = false;
				break;
			}
			if(!result.m_isPtr && oper == wxT("->"))
			{
				wxLogMessage(wxT("Did u mean to use '.' instead of '->'?"));
				evaluationSucceeded = false;
				break;
			}
			typeName = scopeName;
		}
		else
		{
			//-------------------------------------------
			// found an identifier
			//--------------------------------------------
			wxString scopeToSearch(scopeName);
			if(parentTypeScope.IsEmpty() == false && parentTypeScope != wxT("<global>"))
			{
				scopeToSearch = parentTypeScope + wxT("::") + parentTypeName;
			}
			else if((parentTypeScope.IsEmpty()|| parentTypeScope == wxT("<global>")) && !parentTypeName.IsEmpty())
			{
				scopeToSearch = parentTypeName;
			}

			//get the derivation list of the typename
			bool res(false);
			res = TypeFromName(	_U(result.m_name.c_str()), 
								visibleScope, 
								scopeToSearch, 
								parentTypeName.IsEmpty(),
								typeName,	//output
								typeScope);	//output
			if(!res){
				evaluationSucceeded = false;
				break;
			}
		}
		parentTypeName = typeName;
		parentTypeScope = typeScope;
	} // while(NextToken(word, oper, statement))

	if(evaluationSucceeded)
	{
		wxLogMessage(wxT("The expression [") + dbgStmnt + wxT("], evaluated into [") + parentTypeScope + wxT("::") + parentTypeName + wxT("]"));
	}

	return evaluationSucceeded;
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

// return hover tip
void Language::GetHoverTip(const wxString & token, const wxString & scope, const wxString & scopeName, bool isFunc, std::vector<wxString> & tips)
{
	wxUnusedVar(token);
	wxUnusedVar(scope);
	wxUnusedVar(scopeName);
	wxUnusedVar(isFunc);
	wxUnusedVar(tips);
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

wxString Language::GetScopeName(const wxString &in)
{
	const wxCharBuffer buf = _C(in);
	std::string scope_name = get_scope_name(buf.data());
	wxString scope = _U(scope_name.c_str());
	if(scope.IsEmpty())
		scope = wxT("<global>");
	return scope;
}

ExpressionResult Language::ParseExpression(const wxString &in)
{
	const wxCharBuffer buf = _C(in);
	ExpressionResult result = parse_expression(buf.data());
	return result;
}

bool Language::TypeFromName(const wxString &name, 
							const wxString &text, 
							const wxString &scopeName, 
							bool firstToken,
							wxString &type, 
							wxString &typeScope)
{
	//try local scope
	VariableList li;
	FunctionList fooList;

	//first we try to match the current scope
	std::vector<TagEntry> tags;
	TagsManagerST::Get()->FindByNameAndScope(name, scopeName, tags);
	if(tags.size() == 1)
	{
		TagEntry tag(tags.at(0));
		//we have a single match!
		if(tag.GetKind() == wxT("function") || tag.GetKind() == wxT("prototype"))
		{
			Function foo;
			if(FunctionFromPattern(tag.GetPattern(), foo))
			{
				type = _U(foo.m_returnValue.m_type.c_str());
				typeScope = foo.m_returnValue.m_typeScope.empty() ? wxT("<global>") : _U(foo.m_returnValue.m_typeScope.c_str());
				return true;
			} // if(FunctionFromPattern(tag.GetPattern(), foo))
			return false;
		} // if(tag.GetKind() == wxT("function") || tag.GetKind() == wxT("prototype")) 
		else if(tag.GetKind() == wxT("member") || tag.GetKind() == wxT("variable"))
		{
			Variable var;
			if(VariableFromPattern(tag.GetPattern(), var))
			{
				type = _U(var.m_type.c_str());
				typeScope = var.m_typeScope.empty() ? wxT("<global>") : _U(var.m_typeScope.c_str());
				return true;
			}
			return false;
		}
		else
		{
			type = tag.GetName();
			typeScope = tag.GetScopeName();
		}
		return true;
	} // if(tags.size() == 1)
	else 
	{
		//if list contains more than one entry, check if all entries are of type 'function' or 'prototype'
		//(they can be mixed). If all entries are of one of these types, test their return value,
		//if all have the same return value, then we are ok
		Function foo;
		wxString tmpType, tmpTypeScope;
		bool allthesame(true);
		for(size_t i=0; i<tags.size(); i++)
		{
			TagEntry tag(tags.at(i));
			if(!FunctionFromPattern(tag.GetPattern(), foo))
			{
				allthesame = false;
				break;
			} // if(!FunctionFromPattern(tag.GetPattern(), foo))
			
			tmpType = _U(foo.m_returnValue.m_type.c_str());
			tmpTypeScope = foo.m_returnValue.m_typeScope.empty() ? wxT("<global>") : _U(foo.m_returnValue.m_typeScope.c_str());
			if(i > 0 && (tmpType != type || tmpTypeScope != typeScope))
			{
				allthesame = false;
				break;
			} // if(i > 0 && (tmpType != type || tmpTypeScope != typeScope))
			type = tmpType;
			typeScope = tmpTypeScope;
		}

		if(allthesame && !tags.empty())
		{
			return true;
		}

		//can we test visible scope?
		if(firstToken)
		{
			const wxCharBuffer buf = _C(text);
			get_variables(buf.data(), li);

			//search for a full match in the returned list
			for(VariableList::iterator iter = li.begin(); iter != li.end(); iter++)
			{
				Variable var = (*iter);
				wxString var_name = _U(var.m_name.c_str());
				if(var_name == name)
				{
					type = _U(var.m_type.c_str());
					typeScope = var.m_typeScope.empty() ? wxT("<global>") : _U(var.m_typeScope.c_str());
					return true;
				}
			} // for(VariableList::iterator iter = li.begin(); iter != li.end(); iter++)
			//no match in local scope, try the database
		} // if(firstToken)

		//too many matches or none at all
		wxLogMessage(wxT("too many matches or none at all for name: ") + name + wxT(" at scope: ") + scopeName);
		return false;
	}
}


bool Language::VariableFromPattern(const wxString &in, Variable &var)
{
	VariableList li;
	wxString pattern(in);
	//we need to extract the return value from the pattern
	pattern = pattern.BeforeLast(wxT('$'));
	pattern = pattern.AfterFirst(wxT('^'));

	const wxCharBuffer patbuf = _C(pattern);
	li.clear();
	get_variables(patbuf.data(), li);
	if(li.size() == 1)
	{
		var = (*li.begin());
		return true;
	} // if(li.size() == 1)
	return false;
}

bool Language::FunctionFromPattern(const wxString &in, Function &foo)
{
	FunctionList fooList;
	wxString pattern(in);
	//we need to extract the return value from the pattern
	pattern = pattern.BeforeLast(wxT('$'));
	pattern = pattern.AfterFirst(wxT('^'));

	//a limitiation of the function parser...
	pattern += wxT(';');

	const wxCharBuffer patbuf = _C(pattern);
	get_functions(patbuf.data(), fooList);
	if(fooList.size() == 1)
	{
		foo = (*fooList.begin());
		return true;
	}
	return false;
}

