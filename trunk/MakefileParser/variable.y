%{
/**** Includes and Defines *****************************/
#include <stdio.h>
#include <string>
#include <map>

#define YYDEBUG 0        		/* get the pretty debugging code to compile*/
#define YYSTYPE std::string

extern int lineno;

typedef std::map<std::string, std::string> tokens;
typedef tokens::iterator Itokens;
tokens TheTokens;
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
	| input line
;

line:	'\n'				{	printf("|%2d- newline\n", lineno);				}
	| optwords vars_line '\n'	{	printf("|%2d- vars  line: %s%s\n", lineno, $1.c_str(), $2.c_str());	}
	| wordsline '\n'		{	printf("|%2d- words line: %s\n", lineno, $1.c_str());	}
	| assgnline '\n'		{	printf("|%2d- assgn line: %s\n", lineno, $1.c_str());	}
	| printline '\n'		{	printf("|%2d- print line: %s\n", lineno, $1.c_str());	}
	| error	'\n'			{
						printf("unexpected token '%s' at line %d\n", yylval.c_str(), lineno);
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
							printf("Awr... unmatched token '%s'!\n", $2.c_str());
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










