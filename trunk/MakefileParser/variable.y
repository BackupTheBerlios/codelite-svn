%{
/**** Includes and Defines *****************************/
#include <stdio.h>
#include <wx/string.h>
#include <map>
#include <wx/arrstr.h>
#include <wx/wxchar.h>

#define YYDEBUG 0        		/* get the pretty debugging code to compile*/
#define YYSTYPE wxString


typedef wxArrayString Strings;
typedef std::map<wxString, wxString> tokens;
typedef tokens::iterator Itokens;

extern Strings TheOutput;
extern Strings TheUnmatched;
extern Strings TheError;
extern tokens TheTokens;
extern int lineno;

bool append = false;
int yylex(void);

#define TrimString(string) { \
	wxPrintf(wxT("before: '%s'\n"), string.c_str()); \
	string = string.Trim(true);\
	string = string.Trim(false);\
	wxPrintf(wxT("after: '%s'\n"), string.c_str());\
}

void yyerror(char* string)
{
//	printf("parser error: %s\n", string);
}

/*************** Standard variable.y: continues here *********************/
%}

/* Keywords */
%token WORD
%token ASSIGN
%token PRINT

/* Start of grammar */
%%
input:	/* empty */
	| input line			{	/* Do Nothing */				}
;

line:	'\n'				{	/* Do Nothing */				}
	| optwords vars_line '\n'	{	TheOutput.push_back($1+$2);			}
	| wordsline '\n'		{	TheOutput.push_back($1);			}
	| assgnline '\n'		{	/* Do Nothing */				}
	| printline '\n'		{	/* Do Nothing */				}
	| error	'\n'			{
						YYSTYPE msg;
						msg << wxT("Line ") << lineno << wxT(": Unexpected token '") << yylval << wxT("'.");
						TheError.push_back(msg);
						yyerrok;
					}
;

open:	'$' '('				{	/* do nothing */			}

name:	wordvars			{	$$ = $1;				}

close:	')'				{	/* do nothing */			}

variable: open name close 		{
						TrimString($2);
						if(TheTokens[$2].size() > 0)
						{
							$$ = TheTokens[$2];
						}
						else
						{
							TheUnmatched.push_back($2);
							$$ = wxEmptyString;
						}
					}

words: WORD				{	$$ = $1;				}
     | words WORD 			{	$$ = $1 + $2;				}
;

optwords:				{	$$ = wxEmptyString;			}
	| words				{	$$ = $1;				}
;	

optvars:				{	$$ = wxEmptyString;			}	
	| wordvars			{	$$ = $1;				}
;


vars_line: variable optwords		{	$$ = $1 + $2;				}
         | vars_line variable optwords	{	$$ = $1 + $2 + $3;			}
;

wordsline: words			{	$$ = $1;				}

assignm:	ASSIGN			{	append = true;				}
       |	'='			{	append = false;				}
;

assgnline: words assignm optvars	{
	 					TrimString($1);
						TrimString($3);

	 					if(append)
						{
	 						TheTokens[$1] += $3;
						}
						else
						{
							TheTokens[$1] = $3;
						}
	 					$$ = $1 + wxT("=") + $3;			
					}

printline:	PRINT			{
	 					YYSTYPE result = wxT("Tokens: \n");
						for(Itokens it = TheTokens.begin(); it != TheTokens.end(); it++)
						{
							result += wxT("'") + it->first + wxT("'='") + it->second + wxT("'\n");
						}
						result += wxT("Done.");
						$$ = result;
					}

wordvars: WORD 				{	$$ = $1;				}
	| variable 			{	$$ = $1;				}
	| wordvars variable		{	$$ = $1 + $2;				}
	| wordvars WORD 		{	$$ = $1 + $2;				}
;
%%
/* End of grammar */










