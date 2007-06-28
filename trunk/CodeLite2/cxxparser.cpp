#include "cxxparser.h"
#include "idatabase.h"
#include "defs.h"

extern std::string &getFileName();
extern std::string getCurrentScope();
extern int cl_scope_lineno;
extern IDatabase *getDatabase();

void createClassToken(	const std::string &optTmpQualifier, 
						const std::string &keyWord, 
						const std::string &optClassQualifer, 
						const std::string &className, 
						const bool isImpl,
						clTokenPtr &data)
{
	wxUnusedVar(keyWord);
	wxUnusedVar(optClassQualifer);

	data->fullpath = _U(getFileName().c_str());
	data->kind = clToken::class_t;
	data->name = _U(className.c_str());
	data->line = cl_scope_lineno;
	data->scope = _U(getCurrentScope().c_str());
}
