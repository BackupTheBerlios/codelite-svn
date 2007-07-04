#include "variable.tab.h"
#include <stdio.h>
#include <wx/string.h>
#include <wx/arrstr.h>
#include <wx/app.h>
#include <map>

void initLexer(const char *filename);

//std::string yyparse();
YYSTYPE yyparse();

typedef wxArrayString Strings;
typedef std::map<wxString, wxString> tokens;
typedef tokens::iterator Itokens;

Strings TheOutput;
Strings TheUnmatched;
Strings TheError;
tokens TheTokens;


int main(int argv, char* argc[])
{
	wxInitialize();
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
		printf("%s\n", TheOutput[i].c_str());
	}

	printf("============ UNMATCHED ===========\n");
	for(int i = 0; i < TheUnmatched.size(); i++)
	{
		printf("%s\n", TheUnmatched[i].c_str());
	}

	printf("============== ERROR =============\n");
	for(int i = 0; i < TheError.size(); i++)
	{
		printf("%s\n", TheError[i].c_str());
	}

	printf("============= TOKENS =============\n");
        for(Itokens it = TheTokens.begin(); it != TheTokens.end(); it++)
        {
         	printf("'%s'='%s'\n", it->first.c_str(), it->second.c_str());
        }

	printf("=============== DONE =============\n");
	wxUninitialize();
	return 0;
}
