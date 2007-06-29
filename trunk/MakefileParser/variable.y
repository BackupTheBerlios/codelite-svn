%{
/**** Includes and Defines *****************************/
#include <stdio.h>
#include <string>
#include <map>

#define YYDEBUG 1        		/* get the pretty debugging code to compile*/
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

line:	'\n'				{	printf("|- newline\n");			}
	| optwords vars_line '\n'	{	printf("|- vars  line: %s%s\n", $1.c_str(), $2.c_str());	}
    	| wordsline '\n'		{	printf("|- words line: %s\n", $1.c_str());	}
	| assgnline '\n'		{	printf("|- assgn line: %s\n", $1.c_str());	}
	| printline '\n'		{	printf("|- print line: %s\n", $1.c_str());	}
	| error	'\n'			{
						printf("unexpected token '%s' at line %d\n", yylval.c_str(), lineno);
						yyerrok;
					}
;

open:	'$' '('				{	/* do nothing */			}

name:	WORD				{	$$ = $1					}

close:	')'				{	/* do nothing */			}

variable: open name close 		{
						Trim($2);
						if(TheTokens[$2].size() > 0)
						{
							$$ = TheTokens[$2];
						}
						else
						{
							//printf("Awr... unmatched token '%s'!\n", $2.c_str());
							$$ = "";
						}
					}

words: SPACE				{	$$ = " ";			}
     | WORD				{	$$ = $1;			}
     | words WORD 			{	$$ = $1 + $2;			}
     | words SPACE			{	$$ = $1 + " ";			}
;

optwords:				{	$$ = "";				}	
	| words				{	$$ = $1;				}
;

vars_line: variable optwords		{	$$ = $1 + $2;				}
         | vars_line variable optwords	{	$$ = $1 + $2 + $3;			}
;

wordsline: words			{	$$ = $1;				}

assignm:	ASSIGN			{	append = true;				}
       |	'='			{	append = false;				}
;

assgnline: words assignm optwords		{
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
							result += it->first + "=" + it->second + "\n";
						}
						result += "Done.";
						$$ = result;
					}

%%
/* End of grammar */










