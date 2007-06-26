#ifndef CLTOKEN_H
#define CLTOKEN_H

#include "wx/string.h"
#include "stdlib.h"
#include "list"
#include "smart_ptr.h"

/**
 * \class clToken a token class used by CodeLite2 library. this class represents a valid
 *	language token
 */
class clToken 
{
public:
	enum SymbolKind 
	{
		macro_t = 0, 
		class_t, 
		namespace_t,
		struct_t,
		enum_t, 
		enumee_t,
		union_t, 
		member_t,
		function_t,
		prototype_t, 		//we distinguish between function implemntation and declaration
		typedef_t,
		global_scope_t
	};

	size_t		id;
	SymbolKind  kind;		//symbol kind
	wxString	name;		//symbol name, 
	wxString	fullpath;	//FULL path name
	size_t		line;		//line number it can be found
	wxString	signature;	//signature, for functions
	wxString	scope;		//the scope name
	wxString	retValue;	//for functions, this one contains the return value
};

typedef SmartPtr<clToken> clTokenPtr;
typedef std::list<clTokenPtr> clTokenPtrList;

#endif // CLTOKEN_H
