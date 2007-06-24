#ifndef CXX_PARSER_H
#define CXX_PARSER_H

#include "symbol_entry.h"

void createClassSymbol(	const std::string &optTmpQualifier, 
							const std::string &keyWord, 
							const std::string &optClassQualifer, 
							const std::string &className, 
							const bool isImpl,
							SymbolData &data);

#endif //CXX_PARSER_H
