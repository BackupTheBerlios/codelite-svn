#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define YYPREFIX "yy"
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
/*	printf("parser error: %s\n", string);*/
}
/*

	| assignline '\n'		{	printf("|- assignment: %s\n", $1.c_str());	}


aline:	normalline			{	printf("nline\n"); $$ = $1;		}
    |	vline				{	printf("vline\n"); $$ = $1;		}
;



%token SPACE
*/
/*************** Standard variable.y: continues here *********************/
#define WORD 257
#define ASSIGN 258
#define SPACE 259
#define PRINT 260
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    0,    1,    1,    1,    1,    1,    1,    7,    8,
   10,   11,   12,   12,   12,   12,    2,    2,    3,    3,
    4,   13,   13,    5,    6,   14,   14,    9,    9,    9,
};
short yylen[] = {                                         2,
    0,    2,    1,    3,    2,    2,    2,    2,    2,    1,
    1,    3,    1,    1,    2,    2,    0,    1,    2,    3,
    1,    1,    1,    3,    1,    0,    1,    1,    2,    2,
};
short yydefred[] = {                                      1,
    0,    0,   14,   13,   25,    3,    2,    0,    0,    0,
    0,    0,    8,    0,    0,    0,    0,    5,    6,    7,
   15,   22,   16,   23,    0,    9,    4,    0,    0,    0,
    0,    0,   19,    0,   24,   20,   11,   12,   30,   27,
   29,
};
short yydgoto[] = {                                       1,
    7,    8,   15,    9,   10,   11,   16,   29,   30,   38,
   17,   34,   25,   41,
};
short yysindex[] = {                                      0,
  -10,    3,    0,    0,    0,    0,    0,  -26,    4,   15,
   17,  -59,    0,  -12,   -7,  -35, -239,    0,    0,    0,
    0,    0,    0,    0, -239,    0,    0, -239,   -3,  -26,
 -226, -238,    0, -238,    0,    0,    0,    0,    0,    0,
    0,
};
short yyrindex[] = {                                      0,
   -1,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   -6,    0,    0,    0,    0,   -5,    0,    0,    0,
    0,    0,    0,    0,   24,    0,    0,   -5,    0,    1,
  -25,  -24,    0,   -4,    0,    0,    0,    0,    0,    0,
    0,
};
short yygindex[] = {                                      0,
    0,   -2,    0,    0,    0,    0,    0,    0,    0,    0,
   -8,    8,    0,    0,
};
#define YYTABLESIZE 250
short yytable[] = {                                       6,
   14,   24,   27,   21,   17,   18,   28,   31,   12,   14,
   26,   28,   13,   18,   33,   26,   28,    3,   21,    4,
   23,   39,   35,   32,   19,   36,   20,   26,   14,   18,
   17,   18,   40,   17,   17,    0,    0,   37,    0,    0,
    0,   10,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   21,   22,   23,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    3,    0,    4,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    2,    3,    0,    4,    5,
};
short yycheck[] = {                                      10,
   36,   61,   10,   10,   10,   10,   15,   16,    1,   36,
   36,   36,   10,   10,   17,   41,   41,  257,  257,  259,
  259,   30,   25,   16,   10,   28,   10,   40,   36,   36,
   36,   36,  259,   10,   36,   -1,   -1,   41,   -1,   -1,
   -1,   41,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  257,  258,  259,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  257,   -1,  259,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  256,  257,   -1,  259,  260,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 260
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,"'\\n'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,"'$'",0,0,0,"'('","')'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'='",0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"WORD",
"ASSIGN","SPACE","PRINT",
};
char *yyrule[] = {
"$accept : input",
"input :",
"input : input line",
"line : '\\n'",
"line : optwords vars_line '\\n'",
"line : wordsline '\\n'",
"line : assgnline '\\n'",
"line : printline '\\n'",
"line : error '\\n'",
"open : '$' '('",
"name : wordvars",
"close : ')'",
"variable : open name close",
"words : SPACE",
"words : WORD",
"words : words WORD",
"words : words SPACE",
"optwords :",
"optwords : words",
"vars_line : variable optwords",
"vars_line : vars_line variable optwords",
"wordsline : words",
"assignm : ASSIGN",
"assignm : '='",
"assgnline : words assignm optwords",
"printline : PRINT",
"optspace :",
"optspace : SPACE",
"wordvars : words",
"wordvars : variable optspace",
"wordvars : wordvars variable",
};
#endif
#ifndef YYSTYPE
typedef int YYSTYPE;
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short yyss[YYSTACKSIZE];
YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE
/* End of grammar */










#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse()
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register char *yys;
    extern char *getenv();

    if (yys = getenv("YYDEBUG"))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if (yyn = yydefred[yystate]) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#ifdef lint
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#ifdef lint
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 3:
{	printf("|- newline\n");			}
break;
case 4:
{	printf("|- vars  line: %s%s\n", yyvsp[-2].c_str(), yyvsp[-1].c_str());	}
break;
case 5:
{	printf("|- words line: %s\n", yyvsp[-1].c_str());	}
break;
case 6:
{	printf("|- assgn line: %s\n", yyvsp[-1].c_str());	}
break;
case 7:
{	printf("|- print line: %s\n", yyvsp[-1].c_str());	}
break;
case 8:
{
									printf("unexpected token '%s' at line %d\n", yylval.c_str(), lineno);
									yyerrok;
								}
break;
case 9:
{	/* do nothing */			}
break;
case 10:
{	yyval = yyvsp[0];					}
break;
case 11:
{	/* do nothing */			}
break;
case 12:
{
						printf("name '%s'\n", yyvsp[-1].c_str());
						Trim(yyvsp[-1]);
						if(TheTokens[yyvsp[-1]].size() > 0)
						{
							yyval = TheTokens[yyvsp[-1]];
						}
						else
						{
							printf("Awr... unmatched token '%s'!\n", yyvsp[-1].c_str());
							yyval = "";
						}
					}
break;
case 13:
{	yyval = " ";				}
break;
case 14:
{	yyval = yyvsp[0];				}
break;
case 15:
{	yyval = yyvsp[-1] + yyvsp[0];				}
break;
case 16:
{	yyval = yyvsp[-1] + " ";				}
break;
case 17:
{	yyval = "";				}
break;
case 18:
{	yyval = yyvsp[0];				}
break;
case 19:
{	yyval = yyvsp[-1] + yyvsp[0];				}
break;
case 20:
{	yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];			}
break;
case 21:
{	yyval = yyvsp[0];				}
break;
case 22:
{	append = true;				}
break;
case 23:
{	append = false;				}
break;
case 24:
{
	 					Trim(yyvsp[-2]);
						Trim(yyvsp[0]);

	 					if(append)
						{
	 						TheTokens[yyvsp[-2]] += yyvsp[0];
						}
						else
						{
							TheTokens[yyvsp[-2]] = yyvsp[0];
						}
	 					yyval = yyvsp[-2] + "=" + yyvsp[0];			
					}
break;
case 25:
{
	 					std::string result = "Tokens: \n";
						for(Itokens it = TheTokens.begin(); it != TheTokens.end(); it++)
						{
							result += it->first + "=" + it->second + "\n";
						}
						result += "Done.";
						yyval = result;
					}
break;
case 28:
{	yyval = yyvsp[0];				}
break;
case 29:
{	yyval = yyvsp[-1];				}
break;
case 30:
{	yyval = yyvsp[-1] + yyvsp[0];				}
break;
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
