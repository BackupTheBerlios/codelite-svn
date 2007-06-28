#ifndef CXX_PARSER_H
#define CXX_PARSER_H

#include "cltoken.h"

void createClassToken(	const std::string &optTmpQualifier, 
						const std::string &keyWord, 
						const std::string &optClassQualifer, 
						const std::string &className, 
						const bool isImpl,
						clTokenPtr &data);

#endif //CXX_PARSER_H
