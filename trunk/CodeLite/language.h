#ifndef CODELITE_LANGUAGE_H
#define CODELITE_LANGUAGE_H

#include "tokenizer.h"
#include "cpp_scanner.h"
#include "y.tab.h"
#include "singleton.h"
#include <map>
#include "entry.h"
#include <wx/filename.h>
#include "db_record.h"
#include "expression_result.h"
#include "variable.h"
#include "function.h"

#ifdef WXMAKINGDLL_CODELITE
#    define WXDLLIMPEXP_CL WXEXPORT
#elif defined(WXUSINGDLL_CODELITE)
#    define WXDLLIMPEXP_CL WXIMPORT
#else
#    define WXDLLIMPEXP_CL
#endif

/**
 * Language, a helper class that parses (currently) C/C++ code. 
 * It conatains all the parsing of small fragments of code, in places where 
 * ctags chosose to ignore (for example, ctags provides us with a member name, 
 * but it does not provide us with its qualifier).
 *
 * Whenever possible, it is advised to use TagsManager API instead of Language API. 
 *
 * \ingroup CodeLite
 * \date 09-02-2006
 * \author Eran
 */
class WXDLLIMPEXP_CL Language  
{
	friend class Singleton<Language>;

public: 
	/**
	 * Test if word is a language reserved word.
	 * \param word Language token
	 * \return true if it is a reserved word
	 */
	virtual bool IsReservedWord(const wxString &word);

	/**
	 * Return the visible scope of syntax starting at depth zero.
	 * 
	 * For example, given the following pchSrcString:
	 * \code
	 * int main()
	 * {
	 *		{
	 *			int i =0;
	 *		}
	 * }
	 * \endcode
	 * 
	 * the output will be:
	 * \code
	 * int main()
	 * {
	 *   ;
	 * }
	 * \code
	 * 
	 * \note The scopes with depth greater then 0 will be removed.
	 * This function is useful for searching for local members.
	 * \param pchSrcString Input string
	 * \param pchStopWord Stop word, the scan will continue until end of string or until stop word is encountered
	 * \return visible scope 
	 */
	wxString GetScope(const wxString& srcString, const wxString& stopWord);
	
	/**
	 * Get the variable qualifier from a given string.
	 * \param identifier Identifier
	 * \param srcString Input string
	 * \param bVarIsFunc Variable is a function
	 * \return qualifier
	 */
	wxString GetVarQualifier(const wxString& identifier, const wxString& srcString, const bool& bVarIsFunc = false);

	/**
	 * Match closing braces, and return the string in between them. The returned string does not include 
	 * the open barce itself
	 * \param ch Open brace ( can be one of '<', '(', '{', '[' )
	 * \param s Source string
	 * \return true if matched brace was found
	 */
	bool Match(const char& ch, wxString &s, const wxString& sourceString = wxEmptyString);

	/**
	 * Set the language specific auto completion delimeteres, for example: for C++ you should populate
	 * the array with { . , -> , :: }
	 * \param delimArr delimeter array
	 */
	void SetAutoCompDeliemters(const std::vector<wxString>& delimArr);

	/**
	 * Get a hover tip
	 * \param token the token under the cursor
	 * \param scope scope where token was found
	 * \param scopeName scope name
	 * \param isFunc is token is a function
	 * \param tips array of tip strings
	 */
	void GetHoverTip(const wxString & token, const wxString & scope, const wxString & scopeName, bool isFunc, std::vector<wxString> & tips);
	
	/**
	 * Parse comments from source file
	 * \param name file name 
	 * \param comments [output] returned vector of comments
	 */
	void ParseComments(const wxFileName &fileName, std::vector<DbRecordPtr>* comments);

	//==========================================================
	// New API based on the yacc grammar files
	//==========================================================


	/**
	 * Evaluate a C++ expression. for example, the following expression: '((wxFlatNotebook*)book)->'
	 * will be processed into typeName=wxFlatNotebook, and typeScope=<global> (assuming wxFlatNotebook is not 
	 * placed within any namespace)
	 * \param stmt c++ expression
	 * \param text text where this expression was found
	 * \param typeName [output]
	 * \param typeScope [output]
	 * \return true on success, false otherwise. The output fields are only to be checked with the return 
	 * valus is 'true'
	 */
	bool ProcessExpression(const wxString& stmt, const wxString& text, wxString &typeName, wxString &typeScope);

	/**
	 * return scope name from given input string
	 * \param in input string
	 * \return scope name or empty string
	 */
	wxString GetScopeName(const wxString &in);

	/**
	 * parse an expression and return the result. this functions uses 
	 * the sqlite database as its symbol table
	 * \param in input string expression
	 * \return ExpressionResult, if it fails to parse it, check result.m_name.empty() for success
	 */
	ExpressionResult ParseExpression(const wxString &in);

	/**
	 * get the name's type, using local scope when possible. if parsing of local scope fails to find
	 * a match, try the symbol database
	 * \param name name to search for
	 * \param text the scope
	 * \param scopeName the scope name
	 * \param firstToken set to true if the 'name' is the first token in the chain (myClass.some_func(). --> when testing for some_func(), firstToken = false
	 * and when testing myClass. --> first token is set to true
	 * \param type [output] name's type, incase 'name' is matched to a function, type will contain the type of the reutrn value of the function
	 * \param typeScope [output] type's scope
	 * \return true on success false otherwise
	 */
	bool TypeFromName(	const wxString &name, 
						const wxString &text, 
						const wxString &scopeName, 
						bool firstToken,
						wxString &type, 
						wxString &typeScope);

private:
	/**
	 * Private constructor
	 */
	Language();
	
	/**
	 * Private destructor
	 */
	virtual ~Language();

	/**
	 * Return the next token and the delimiter found
	 * \param token next token
	 * \param delim delimiter found
	 * \param srcStr source string. Note that this function write/delete to the src string
	 * so pass a copy of the original string. same source string should be passed to this function 
	 * to get the next token
	 * \return true if token was found false otherwise
	 */
	bool NextToken(wxString &token, wxString &delim, wxString &srcStr);

	/**
	 * Test whether the qualifier string contains pointer character ('*')
	 * \param qualifier qualifier string
	 * \return true if it contains trailing '*' else false
	 */
	bool IsPointer(const wxString &qualifier);

	// helper methods
	wxString Qualifier(const TagEntry& tag, bool & isPtr, bool truncateTemplate = true, wxString *fullQualifier = NULL);
	
	/// Return tip string from tag
	wxString GetStringTip(TagEntry& tag);

	bool VariableFromPattern(const wxString &pattern, Variable &var);
	bool FunctionFromPattern(const wxString &pattern, Function &foo);
private:
	
	std::map<wxString, int> m_operatorMap;
	std::map<wxString, int> m_reservedWords;
	std::map<char, char>    m_braces;
	std::vector<wxString>	m_delimArr;
	wxString m_expression;
	CppScannerPtr m_scanner;
#ifdef USE_TRACE
	wxStopWatch m_watch;
#endif
};

typedef Singleton<Language> LanguageST;

#endif // CODELITE_LANGUAGE_H
