#ifndef SYMBOL_TABLE_ENTRY
#define SYMBOL_TABLE_ENTRY

#include "string"
#include "smart_ptr.h"

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

/**
 * A symbol entry. Each time the parser succeeded in detecing a rule, it creates an entry in 
 * the symbol table and stores it there.
 * Currently, symbols can be one of:
 * 		macro, 
			class, 
			namespace, 
			struct, 
			enum, 
			enumee, (enum member)
			union, 
			member,
			function,
			typedef
 */
class SymbolData
{
public:
	SymbolData(): line(0) {};
	virtual ~SymbolData(){}
	
	/**
	 * create a unique key identifier to this symbol enntry
	 */ 
	std::string MakeKey() const
	{
		std::string key;
		if(kind == prototype_t)
			key += "proto_";
		key += scope;
		key += "::";
		key += name;
		key += signature;
		return key;
	}
	
public:	
	SymbolKind  kind;				//symbol kind
	std::string name;				//symbol name
	std::string file;				//file it exists
	int 			line;				//line number it can be found
	std::string signature;		//signature, for functions
	std::string scope;			//the scope name
	std::string retValue;		//for functions, this one contains the return value
};

typedef SmartPtr<SymbolData> SymbolDataPtr;
#endif // SYMBOL_TABLE_ENTRY
