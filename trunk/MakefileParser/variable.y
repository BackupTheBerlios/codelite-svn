%{
/**** Includes and Defines *****************************/
#include <stdio.h>
#include <string>

#define YYDEBUG 1        		/* get the pretty debugging code to compile*/
#define YYSTYPE std::string

extern int lineno;

int yylex(void);

void yyerror(char* string)
{
//	printf("parser error: %s\n", string);
}
/*

	| assignline '\n'		{	printf("|- assignment: %s\n", $1.c_str());	}


aline:	normalline			{	printf("nline\n"); $$ = $1;		}
    |	vline				{	printf("vline\n"); $$ = $1;		}
;

*/
/*************** Standard variable.y: continues here *********************/
%}

/* Keywords */
%token WORD
%token SPACE
%token ASSIGN

/* Start of grammar */
%%
input:	/* empty */
	| input line
;

line:	'\n'				{	printf("|- newline\n");			}
	| vars_line '\n'		{	printf("|- vars  line: %s\n", $1.c_str());	}
    	| wordsline '\n'		{	printf("|- words line: %s\n", $1.c_str());	}
	| assgnline '\n'		{	printf("|- assgn line: %s\n", $1.c_str());	}
	| error	'\n'			{
						printf("unexpected token '%s' at line %d\n", yylval.c_str(), lineno);
						yyerrok;
					}
;

open:	'$' '('				{	/* do nothing */			}

name:	WORD				{	$$ = $1					}

close:	')'				{	/* do nothing */			}

variable: open name close 		{	
						if(!$2.compare("test"))
						{
							$$ = "roflcoptr";
						}
						else
						{
							$$ = $2;	
						}
					}

spword:	WORD				{	$$ = $1					}
      | WORD SPACE			{	$$ = $1	+ " "				}
;

words: spword				{	$$ = $1;				}
     | words spword 			{	$$ = $1 + $2;				}
;

vars_line: variable			{	$$ = $1					}
         | vars_line variable		{	$$ = $1 + $2;				}
;

wordsline: words			{	$$ = $1					}

assignm:	ASSIGN			{	/* do nothing */			}
       |	'='			{	/* do nothing */			}
;

assgnline: spword assignm spword	{	$$ = $1 + " = " + $3;			}

%%
/* End of grammar */










