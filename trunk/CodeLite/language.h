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

#ifdef WXMAKINGDLL_CODELITE
#    define WXDLLIMPEXP_CL WXEXPORT
#elif defined(WXUSINGDLL_CODELITE)
#    define WXDLLIMPEXP_CL WXIMPORT
#else /* not making nor using FNB as DLL */
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
	 * Process a grammar expression and return the type resolved.
	 * \param stmt c++ statement
	 * \param parent [output] the parent of the last token of the expressions (example, ClassA::FuncThatReturnClassB().PrintFuncOfClassB() - parent will be ClassB)
	 * \param scope Local scope
	 * \param calltipContext this function is called in GetFunctionTip() context
	 * \param scopeName Scope name
	 * \return qualifier of the statement
	 */
	wxString ProcessExpression(const wxString& stmt, wxString & parent, bool calltipContext = false, const wxString& scope = wxEmptyString, const wxString& scopeName = wxEmptyString);

	/**
	 * Set the language specific auto completion delimeteres, for example: for C++ you should populate
	 * the array with { . , -> , :: }
	 * \param delimArr delimeter array
	 */
	void SetAutoCompDeliemters(const std::vector<wxString>& delimArr);

	/**
	 * Parse a function signature and return vector of pairs of Qualifier:Identifier
	 * \param signature signature to parse
	 * \param args [output] vector of pairs of wxStrings
	 * \param trailer return the string that contains the trailer of the signature (e.g. const)
	 * \return true if succeeded, false otherwise
	 */
	bool ParseSignature(const wxString &signature, std::vector<std::pair<wxString, wxString> >& args, wxString & trailer);

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
	 * Return the qualifier from a casting expression.
	 * \param expression Input expression
	 * \return qualifier or an empty string
	 */
	wxString ResolveCppCast(const wxString& expression);

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

	/**
	 * Get tag qualifier from array of tags. 
	 * - If tags contains only one qualifer, and it is a function, return its returned value qualifier based on the pattern
	 * - If tags contains only one qualifer, and it is a member/variable, return its qualifier based on the pattern
	 * - If the array contains more than one tag and all consist of the same the qualifers and of the same kind - return it.
	 * - any other case, return empty string
	 * \param tags array of tags
	 * \param isPtr set to true of the qualifier is a pointer
	 * \return tag qualifier or empty string
	 */
	wxString GetTagQualifier(std::vector<TagEntry> & tags, bool & isPtr);

	// helper methods
	wxString Qualifier(const TagEntry& tag, bool & isPtr, bool truncateTemplate = true, wxString *fullQualifier = NULL);
	
	/**
	 * Return word's qualifier searching local database, external database and the local scope
	 * \param word token 
	 * \param scope string containing our token
	 * \param scopeName scope name
	 * \param isPtr [output] return true if the qualifier is a pointer
	 * \return qualifier or wxEmptyString
	 */
	wxString GetWordQualifier(const wxString & word, const wxString & scope, const wxString & scopeName, bool & isPtr );

	/**
	 * Construct tips array from vector of tags
	 * \param tags [input] vector of matched tags
	 * \param tips [output] vector of formatted tips constructed from the tags
	 */
	void ConstructTips( std::vector<TagEntry> & tags, std::vector<wxString> & tips );

	/**
	 * Second filter function, which removes duplicate entries of prototype & functions
	 * if isFunc is true, is will also remove any non 'function' 'prototype' entry
	 * \param src input tags
	 * \param isFunc requested tag is a function
	 * \param name tag name
	 * \param tags [output] filtered tags
	 */
	void FilterResults(const std::vector<TagEntry>& src, bool isFunc, const wxString &name, std::vector<TagEntry>& tags);

	/// Return tip string from tag
	wxString GetStringTip(TagEntry& tag);

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
