#include "variable.tab.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <map>

void initLexer(const char *filename);

//std::string yyparse();
YYSTYPE yyparse();

typedef std::vector<std::string> Strings;
typedef std::map<std::string, std::string> tokens;
typedef tokens::iterator Itokens;

Strings TheOutput;
Strings TheUnmatched;
Strings TheError;
tokens TheTokens;


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

	printf("============= RESULT =============\n");
	for(int i = 0; i < TheOutput.size(); i++)
	{
		printf("%s\n", TheOutput.at(i).c_str());
	}

	printf("============ UNMATCHED ===========\n");
	for(int i = 0; i < TheUnmatched.size(); i++)
	{
		printf("%s\n", TheUnmatched.at(i).c_str());
	}

	printf("============== ERROR =============\n");
	for(int i = 0; i < TheError.size(); i++)
	{
		printf("%s\n", TheError.at(i).c_str());
	}

	printf("============ TOKENS ==============\n");
        for(Itokens it = TheTokens.begin(); it != TheTokens.end(); it++)
        {
         	printf("'%s'='%s'\n", it->first.c_str(), it->second.c_str());
        }

	printf("=============== DONE =============\n");
	return 0;
}
