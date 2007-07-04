%{
/**** Includes and Defines *****************************/
#include <stdio.h>
#include <string>
#include <map>
#include <vector>

#define YYDEBUG 0        		/* get the pretty debugging code to compile*/
#define YYSTYPE std::string


typedef std::vector<std::string> Strings;
typedef std::map<std::string, std::string> tokens;
typedef tokens::iterator Itokens;

extern Strings TheOutput;
extern Strings TheUnmatched;
extern Strings TheError;
extern tokens TheTokens;
extern int lineno;

bool append = false;
int yylex(void);

void Trim(std::string& line)
{
	size_t to = line.find_first_not_of(" \t");
	if(to != -1)
		line.erase(0, to);

	size_t from = line.find_last_not_of(" \t")+1;
	if(to != -1)
		line.erase(from);
}

void yyerror(char* string)
{
//	printf("parser error: %s\n", string);
}
/*

	| assignline '\n'		{	printf("|- assignment: %s\n", $1.c_str());	}


aline:	normalline			{	printf("nline\n"); $$ = $1;		}
    |	vline				{	printf("vline\n"); $$ = $1;		}
;



%token SPACE
*/
/*************** Standard variable.y: continues here *********************/
%}

/* Keywords */
%token WORD
%token ASSIGN
%token SPACE
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
						char buf[1024];
						buf[0] = '\0';
						sprintf(buf, "Line %3d: Unexpected token '%s'");
						std::string msg(buf);
						TheError.push_back(msg);
						yyerrok;
					}
;

open:	'$' '('				{	/* do nothing */			}

name:	wordvars			{	$$ = $1;				}

close:	')'				{	/* do nothing */			}

variable: open name close 		{
						Trim($2);
						if(TheTokens[$2].size() > 0)
						{
							$$ = TheTokens[$2];
						}
						else
						{
							TheUnmatched.push_back($2);
							$$ = "";
						}
					}

words: WORD				{	$$ = $1;				}
     | words WORD 			{	$$ = $1 + $2;				}
;

optwords:				{	$$ = "";				}
	| words				{	$$ = $1;				}
;	

optvars:				{	$$ = "";				}	
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
	 					Trim($1);
						Trim($3);

	 					if(append)
						{
	 						TheTokens[$1] += $3;
						}
						else
						{
							TheTokens[$1] = $3;
						}
	 					$$ = $1 + "=" + $3;			
					}

printline:	PRINT			{
	 					std::string result = "Tokens: \n";
						for(Itokens it = TheTokens.begin(); it != TheTokens.end(); it++)
						{
							result += "'" + it->first + "'='" + it->second + "'\n";
						}
						result += "Done.";
						$$ = result;
					}

wordvars: WORD 				{	$$ = $1;				}
	| variable 			{	$$ = $1;				}
	| wordvars variable		{	$$ = $1 + $2;				}
	| wordvars WORD 		{	$$ = $1 + $2;				}
;
%%
/* End of grammar */










