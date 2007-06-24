#ifndef CXX_PARSER_H
#define CXX_PARSER_H

#include "symbol_entry.h"

SymbolTableEntry *createClassSymbol(	const std::string &optTmpQualifier, 
											const std::string &keyWord, 
											const std::string &optClassQualifer, 
											const std::string &className, 
											bool isImpl);


#endif //CXX_PARSER_H
