#include "variable.tab.h"
#include <stdio.h>

void initLexer(const char *filename);

YYSTYPE yyparse();

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
