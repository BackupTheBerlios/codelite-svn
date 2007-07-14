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

Language::Language()
: m_expression(wxEmptyString)
, m_scanner( new CppScanner() )
{
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

/// Return the visible scope until pchStopWord is encountered
wxString Language::GetScope(const wxString& srcString)
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
	wxString visibleScope = GetScope(text);
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
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->FindByNameAndScope(name, scopeName, tags);
	if(tags.size() == 1)
	{
		TagEntryPtr tag(tags.at(0));
		//we have a single match!
		if(tag->GetKind() == wxT("function") || tag->GetKind() == wxT("prototype"))
		{
			Function foo;
			if(FunctionFromPattern(tag->GetPattern(), foo))
			{
				type = _U(foo.m_returnValue.m_type.c_str());
				typeScope = foo.m_returnValue.m_typeScope.empty() ? wxT("<global>") : _U(foo.m_returnValue.m_typeScope.c_str());
				return true;
			} // if(FunctionFromPattern(tag->GetPattern(), foo))
			return false;
		} // if(tag->GetKind() == wxT("function") || tag->GetKind() == wxT("prototype")) 
		else if(tag->GetKind() == wxT("member") || tag->GetKind() == wxT("variable"))
		{
			Variable var;
			if(VariableFromPattern(tag->GetPattern(), var))
			{
				type = _U(var.m_type.c_str());
				typeScope = var.m_typeScope.empty() ? wxT("<global>") : _U(var.m_typeScope.c_str());
				return true;
			}
			return false;
		}
		else
		{
			type = tag->GetName();
			typeScope = tag->GetScopeName();
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
			TagEntryPtr tag(tags.at(i));
			if(!FunctionFromPattern(tag->GetPattern(), foo))
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

void Language::GetLocalVariables(const wxString &in, std::vector<TagEntryPtr> &tags, const wxString &name, SearchFlags flags)
{
	VariableList li;
	Variable var;
	wxString pattern(in);
	
	const wxCharBuffer patbuf = _C(pattern);
	li.clear();
	get_variables(patbuf.data(), li);
	VariableList::iterator iter = li.begin();
	for(; iter != li.end(); iter++)
	{
		var = (*iter);
		wxString tagName = _U(var.m_name.c_str());

		//if we have name, collect only tags that matches name
		if(name.IsEmpty())
			continue;

		if(flags == PartialMatch && !tagName.StartsWith(name))
			continue;

		if(flags == ExactMatch && tagName != name)
			continue;

		TagEntryPtr tag(new TagEntry());
		tag->SetName(tagName);
		tag->SetKind(wxT("variable"));
		tag->SetParent(wxT("<local>"));
		tag->SetAccess(wxT("public"));
		tag->SetPattern(_U(var.m_pattern.c_str()));
		tags.push_back(tag);
	}
}
