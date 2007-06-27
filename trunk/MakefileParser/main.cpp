#include "variable.tab.h"
#include <stdio.h>

void initLexer(const char *filename);

YYSTYPE yyparse();

int main()
{
	initLexer("input");
	yyparse();
	return 0;
}
