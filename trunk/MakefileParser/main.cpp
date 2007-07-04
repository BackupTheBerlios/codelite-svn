#include "variable.tab.h"
#include <stdio.h>
#include <string>
#include <vector>

void initLexer(const char *filename);

//std::string yyparse();
YYSTYPE yyparse();

typedef std::vector<std::string> Strings;
Strings TheOutput;


int main(int argv, char* argc[])
{
	if(argv>1)
	{
		initLexer(argc[1]);
	}
	else
	{
		initLexer("input");
	}

	yyparse();
	return 0;
}
