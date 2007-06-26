#ifndef CODELITE_H
#define COEDLITE_H

#include "idatabase.h"
#include "iparser.h"
#include "cltoken.h"

/**
 * \class CodeLite defines the codecompletion API to be used by the IDE
 */
class CodeLite 
{
private:
	CodeLite();
	virtual ~CodeLite();
public:

	/**
	 * \return the single CodeLite instance
	 */
	static CodeLite *Get();

public:
	
	//============================================================================
	//General API
	//============================================================================

	/**
	 * Return a list of tokens that matches 'partialName', 'context' is the code fraction that 
	 * 'partialName' was obtained from
	 * \param partialName 
	 * \param context code context 
	 * \param parser parser to use for evaluating the context
	 * \param db the db interface to allow the library to perform queries
	 * \param tokens output
	 */
	void GetTokensByPartialName(const wxString &partialName, 
								const wxString &context, 
								const IParser &parser,
								const IDatabase &db,
								clTokenPtrList &tokens); 

	/**
	 * Return the direct children of 'parent' from the db
	 * \param parent parent to token 
	 * \param db the db interface to allow the library to perform queries
	 * \param tokens output
	 */
	void GetChildren (	const clTokenPtr parent,
						const IDatabase &db,
						clTokenPtrList &tokens);

	/**
	 * evaluate an expression and try to determine it clTokenPtr
	 * \param parent parent to token 
	 * \param db the db interface to allow the library to perform queries
	 * \param parser parser to use for evaluating the expression
	 * \return clTokenPtr or NULL is it failed on parsing the expression
	 */
	clTokenPtr ProcessExpression (const wxString &expr,
								  const IParser &parser,
								  const IDatabase &db);
};
#endif // CODELITE_H
