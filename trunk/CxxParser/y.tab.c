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
/* Copyright Eran Ifrah(c)*/
/*************** Includes and Defines *****************************/
#include "string"
#include "vector"
#include "symbol_table.h"
#include "cxxparser.h"

#define YYDEBUG_LEXER_TEXT (yylval) /* our lexer loads this up each time.
				     We are telling the graphical debugger
				     where to find the spelling of the 
				     tokens.*/
extern char *yytext;
#define YYSTYPE std::string
#define YYDEBUG 0        /* get the pretty debugging code to compile*/

#include "stdio.h"
int yyparse();
void yyerror(char *string);
extern int yylex();
extern bool setLexerInput(const char *fileName);
void syncParser();
extern int yylineno;
extern std::vector<std::string> currentScope;

extern void printScopeName();	/*print the current scope name*/
extern void increaseScope();	/*increase scope with anonymouse value*/

/*************** Standard ytab.c continues here *********************/
#define LE_AUTO 257
#define LE_DOUBLE 258
#define LE_INT 259
#define LE_STRUCT 260
#define LE_BREAK 261
#define LE_ELSE 262
#define LE_LONG 263
#define LE_SWITCH 264
#define LE_CASE 265
#define LE_ENUM 266
#define LE_REGISTER 267
#define LE_TYPEDEF 268
#define LE_CHAR 269
#define LE_EXTERN 270
#define LE_RETURN 271
#define LE_UNION 272
#define LE_CONST 273
#define LE_FLOAT 274
#define LE_SHORT 275
#define LE_UNSIGNED 276
#define LE_CONTINUE 277
#define LE_FOR 278
#define LE_SIGNED 279
#define LE_VOID 280
#define LE_DEFAULT 281
#define LE_GOTO 282
#define LE_SIZEOF 283
#define LE_VOLATILE 284
#define LE_DO 285
#define LE_IF 286
#define LE_STATIC 287
#define LE_WHILE 288
#define LE_NEW 289
#define LE_DELETE 290
#define LE_THIS 291
#define LE_OPERATOR 292
#define LE_CLASS 293
#define LE_PUBLIC 294
#define LE_PROTECTED 295
#define LE_PRIVATE 296
#define LE_VIRTUAL 297
#define LE_FRIEND 298
#define LE_INLINE 299
#define LE_OVERLOAD 300
#define LE_TEMPLATE 301
#define LE_TYPENAME 302
#define LE_IDENTIFIER 303
#define LE_STRINGliteral 304
#define LE_FLOATINGconstant 305
#define LE_INTEGERconstant 306
#define LE_CHARACTERconstant 307
#define LE_OCTALconstant 308
#define LE_HEXconstant 309
#define LE_POUNDPOUND 310
#define LE_CComment 311
#define LE_CPPComment 312
#define LE_NAMESPACE 313
#define LE_TYPEDEFname 314
#define LE_ARROW 315
#define LE_ICR 316
#define LE_DECR 317
#define LE_LS 318
#define LE_RS 319
#define LE_LE 320
#define LE_GE 321
#define LE_EQ 322
#define LE_NE 323
#define LE_ANDAND 324
#define LE_OROR 325
#define LE_ELLIPSIS 326
#define LE_CLCL 327
#define LE_DOTstar 328
#define LE_ARROWstar 329
#define LE_MULTassign 330
#define LE_DIVassign 331
#define LE_MODassign 332
#define LE_PLUSassign 333
#define LE_MINUSassign 334
#define LE_LSassign 335
#define LE_RSassign 336
#define LE_ANDassign 337
#define LE_ERassign 338
#define LE_ORassign 339
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    0,
    0,    2,    2,    2,    2,    2,    7,    7,    9,    9,
    8,    8,   10,   10,   11,   11,   12,   12,   12,   13,
    3,    3,    5,    6,   17,   17,    4,   15,   15,   21,
   20,   20,   18,   18,   14,   14,   22,   22,   23,   23,
   16,   19,   19,   19,   24,   24,
};
short yylen[] = {                                         2,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    0,
    2,    1,    1,    1,    1,    1,    0,    2,    1,    3,
    1,    1,    0,    1,    0,    4,    0,    1,    3,    4,
    5,    5,    1,    1,    1,    1,    9,    0,    2,    2,
    0,    2,    0,    1,    0,    1,    0,    1,    0,    2,
    2,    4,    5,    8,    0,    2,
};
short yydefred[] = {                                     10,
    0,   16,   44,    0,   34,   33,   11,   12,   13,   14,
   15,    0,    0,    0,   36,   35,    0,   46,   38,   38,
   21,   22,   19,    0,    0,   24,    0,    0,    0,   18,
    0,   26,    0,    6,    1,    4,    2,    5,    3,    8,
    7,    9,    0,   49,   39,    0,    0,   20,   31,   32,
   40,    0,    0,    0,   52,    0,    0,   28,   38,    0,
   53,   48,   50,   51,    0,    0,   49,    0,   56,    0,
   29,    0,    0,    0,    0,   54,   30,   42,   37,
};
short yydgoto[] = {                                       1,
   44,    7,    8,    9,   10,   11,   23,   24,   25,   27,
   12,   57,   58,   59,   28,   53,   17,   13,   20,   75,
   45,   64,   54,   61,
};
short yysindex[] = {                                      0,
 -118,    0,    0,  -44,    0,    0,    0,    0,    0,    0,
    0, -246, -250, -263,    0,    0, -285,    0,    0,    0,
    0,    0,    0, -266,  -27,    0, -260, -225, -290,    0,
 -263,    0,  -53,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  -57,    0,    0,   16, -255,    0,    0,    0,
    0, -250, -230,   26,    0,   33,  -26,    0,    0,   14,
    0,    0,    0,    0, -250, -250,    0, -237,    0,   17,
    0, -230, -255, -229,   21,    0,    0,    0,    0,
};
short yyrindex[] = {                                      0,
 -248,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0, -217,  -22,    0,    0, -222,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  -22,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  -38,    0,    0,    0,    0,    0,    0,    0,
    0,  -42, -231,  -43,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   10, -228,    0,    0,    0,   25,
    0, -231,   23,    0,    0,    0,    0,    0,    0,
};
short yygindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,   56,    0,    0,    0,
    0,    0,   22,   -5,  -11,    9,    0,    0,    0,    0,
    0,    0,    0,   18,
};
#define YYTABLESIZE 276
short yytable[] = {                                      49,
   47,   27,   52,   49,    5,   49,    6,   19,   29,   43,
   43,   25,   46,   15,   43,   14,   31,   66,   47,   27,
   43,   17,   18,   47,   43,   43,   43,   43,   26,   21,
   43,   43,   34,   35,   32,   67,   30,   36,   22,   17,
   45,   45,   33,   37,   25,   45,   16,   68,   38,   39,
   40,   45,   55,   41,   42,   56,   45,   45,   45,   70,
   49,   45,   45,   62,   49,   43,   49,   63,   45,   50,
   45,   51,   60,   65,   69,   72,   73,   74,   78,   79,
   23,   77,   55,   41,   49,   45,   48,   71,   43,   76,
    0,    0,    0,    0,    0,    0,   45,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    2,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    3,    0,
    0,    0,    4,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   47,
    0,    0,    0,    0,   49,    0,    0,    0,    0,   51,
   47,   45,    0,    0,    0,   49,
};
short yycheck[] = {                                      38,
   44,   44,   60,   42,  123,   59,  125,   13,   20,  258,
  259,  260,  303,  260,  263,   60,   44,   44,   62,   62,
  269,   44,  273,  314,  273,  274,  275,  276,  314,  293,
  279,  280,  258,  259,   62,   62,  303,  263,  302,   62,
  258,  259,  303,  269,  293,  263,  293,   59,  274,  275,
  276,  269,   44,  279,  280,   40,  274,  275,  276,   65,
   38,  279,  280,   38,   42,  314,   44,   42,   59,  123,
   61,  327,  303,   41,   61,   67,  314,   61,  308,   59,
  303,   73,  314,   59,   62,  314,   31,   66,  314,   72,
   -1,   -1,   -1,   -1,   -1,   -1,  314,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  256,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  297,   -1,
   -1,   -1,  301,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  303,
   -1,   -1,   -1,   -1,  303,   -1,   -1,   -1,   -1,  327,
  314,  314,   -1,   -1,   -1,  314,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 339
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,"'&'",0,"'('","')'","'*'",0,"','",0,0,0,0,0,0,0,0,0,0,0,0,0,0,"';'",
"'<'","'='","'>'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'{'",0,"'}'",0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,"LE_AUTO","LE_DOUBLE","LE_INT","LE_STRUCT","LE_BREAK","LE_ELSE",
"LE_LONG","LE_SWITCH","LE_CASE","LE_ENUM","LE_REGISTER","LE_TYPEDEF","LE_CHAR",
"LE_EXTERN","LE_RETURN","LE_UNION","LE_CONST","LE_FLOAT","LE_SHORT",
"LE_UNSIGNED","LE_CONTINUE","LE_FOR","LE_SIGNED","LE_VOID","LE_DEFAULT",
"LE_GOTO","LE_SIZEOF","LE_VOLATILE","LE_DO","LE_IF","LE_STATIC","LE_WHILE",
"LE_NEW","LE_DELETE","LE_THIS","LE_OPERATOR","LE_CLASS","LE_PUBLIC",
"LE_PROTECTED","LE_PRIVATE","LE_VIRTUAL","LE_FRIEND","LE_INLINE","LE_OVERLOAD",
"LE_TEMPLATE","LE_TYPENAME","LE_IDENTIFIER","LE_STRINGliteral",
"LE_FLOATINGconstant","LE_INTEGERconstant","LE_CHARACTERconstant",
"LE_OCTALconstant","LE_HEXconstant","LE_POUNDPOUND","LE_CComment",
"LE_CPPComment","LE_NAMESPACE","LE_TYPEDEFname","LE_ARROW","LE_ICR","LE_DECR",
"LE_LS","LE_RS","LE_LE","LE_GE","LE_EQ","LE_NE","LE_ANDAND","LE_OROR",
"LE_ELLIPSIS","LE_CLCL","LE_DOTstar","LE_ARROWstar","LE_MULTassign",
"LE_DIVassign","LE_MODassign","LE_PLUSassign","LE_MINUSassign","LE_LSassign",
"LE_RSassign","LE_ANDassign","LE_ERassign","LE_ORassign",
};
char *yyrule[] = {
"$accept : translation_unit",
"basic_type_name : LE_INT",
"basic_type_name : LE_CHAR",
"basic_type_name : LE_SHORT",
"basic_type_name : LE_LONG",
"basic_type_name : LE_FLOAT",
"basic_type_name : LE_DOUBLE",
"basic_type_name : LE_SIGNED",
"basic_type_name : LE_UNSIGNED",
"basic_type_name : LE_VOID",
"translation_unit :",
"translation_unit : translation_unit external_decl",
"external_decl : class_decl",
"external_decl : function_decl",
"external_decl : scope_reducer",
"external_decl : scope_increaer",
"external_decl : error",
"template_arg :",
"template_arg : template_specifiter LE_IDENTIFIER",
"template_arg_list : template_arg",
"template_arg_list : template_arg_list ',' template_arg",
"template_specifiter : LE_CLASS",
"template_specifiter : LE_TYPENAME",
"opt_class_qualifier :",
"opt_class_qualifier : LE_TYPEDEFname",
"opt_template_qualifier :",
"opt_template_qualifier : LE_TEMPLATE '<' template_arg_list '>'",
"template_parameter_list :",
"template_parameter_list : template_parameter",
"template_parameter_list : template_parameter_list ',' template_parameter",
"template_parameter : const_spec nested_scope_specifier LE_TYPEDEFname special_star_amp",
"class_decl : opt_template_qualifier class_keyword opt_class_qualifier LE_IDENTIFIER ';'",
"class_decl : opt_template_qualifier class_keyword opt_class_qualifier LE_IDENTIFIER '{'",
"scope_reducer : '}'",
"scope_increaer : '{'",
"class_keyword : LE_CLASS",
"class_keyword : LE_STRUCT",
"function_decl : virtual_spec variable_decl nested_scope_specifier LE_IDENTIFIER '(' ')' const_spec pure_virtual_spec ';'",
"nested_scope_specifier :",
"nested_scope_specifier : nested_scope_specifier scope_specifier",
"scope_specifier : LE_TYPEDEFname LE_CLCL",
"pure_virtual_spec :",
"pure_virtual_spec : '=' LE_OCTALconstant",
"virtual_spec :",
"virtual_spec : LE_VIRTUAL",
"const_spec :",
"const_spec : LE_CONST",
"amp_item :",
"amp_item : '&'",
"star_list :",
"star_list : star_list '*'",
"special_star_amp : star_list amp_item",
"variable_decl : const_spec nested_scope_specifier basic_type_name special_star_amp",
"variable_decl : const_spec nested_scope_specifier LE_TYPEDEFname special_star_amp var_name_hack",
"variable_decl : const_spec nested_scope_specifier LE_TYPEDEFname '<' template_parameter_list '>' special_star_amp var_name_hack",
"var_name_hack :",
"var_name_hack : LE_IDENTIFIER '='",
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
void yyerror(char *s) {}

void syncParser(){
	//move lexer to the next ';' line or scope opening '{'
	int ch = yylex();
}

int main(void) {
	if( !setLexerInput("test.h") ){
		return -1;
	}
	yyparse();
	return 0;
}
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
case 1:
{ yyval = yyvsp[0]; }
break;
case 2:
{ yyval = yyvsp[0]; }
break;
case 3:
{ yyval = yyvsp[0]; }
break;
case 4:
{ yyval = yyvsp[0]; }
break;
case 5:
{ yyval = yyvsp[0]; }
break;
case 6:
{ yyval = yyvsp[0]; }
break;
case 7:
{ yyval = yyvsp[0]; }
break;
case 8:
{ yyval = yyvsp[0]; }
break;
case 9:
{ yyval = yyvsp[0]; }
break;
case 16:
{ 
								printf("CodeLite: syntax error, unexpected token '%s' found at line %d \n", yytext, yylineno);
								syncParser();
							}
break;
case 18:
{yyval = yyvsp[-1] + " " + yyvsp[0];}
break;
case 19:
{ yyval = yyvsp[0]; }
break;
case 20:
{ yyval = yyvsp[-2] + " " + yyvsp[-1] + " " + yyvsp[0]; }
break;
case 21:
{ yyval = yyvsp[0]; }
break;
case 22:
{ yyval = yyvsp[0]; }
break;
case 24:
{ yyval = yyvsp[0];}
break;
case 26:
{ yyval = yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 27:
{yyval = "";}
break;
case 28:
{yyval = yyvsp[0];}
break;
case 29:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 30:
{yyval = yyvsp[-3] + yyvsp[-2] + yyvsp[-1] +yyvsp[0];}
break;
case 31:
{
																												printf("Found class decl: %s\n", yyvsp[-1].c_str());
																												SymbolData data;
																												/*create class symbol and add it*/
																												createClassSymbol(yyvsp[-4], yyvsp[-3], yyvsp[-2], yyvsp[-1], false, data);
																												SymbolTable::instance().AddSymbol(data);
																											}
break;
case 32:
{
																												printf("Found class impl: %s\n", yyvsp[-1].c_str());
																												SymbolData data;
																												/*create class symbol and add it*/
																												createClassSymbol(yyvsp[-4], yyvsp[-3], yyvsp[-2], yyvsp[-1], true, data);
																												SymbolTable::instance().AddSymbol(data);
																												
																												/*increase the scope level*/
																												currentScope.push_back(yyvsp[-1]);
																												printScopeName();
																											}
break;
case 33:
{
									if(currentScope.empty())
									{
										/*fatal error!*/
										printf("CodeLite: fatal error - cant go beyond global scope!\n");
									}
									else
									{
										currentScope.pop_back();
										printScopeName();
									}
								}
break;
case 34:
{
								/*increase random scope*/
								increaseScope();
								printScopeName();
							 }
break;
case 35:
{yyval = yyvsp[0];}
break;
case 36:
{yyval = yyvsp[0];}
break;
case 37:
{
																																						printf("Found function: %s\n", yyvsp[-5].c_str());
																																					}
break;
case 38:
{yyval = "";}
break;
case 39:
{yyval = yyvsp[-1] + " " + yyvsp[0];}
break;
case 40:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 41:
{yyval = ""; }
break;
case 42:
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
break;
case 43:
{yyval = ""; }
break;
case 44:
{ yyval = yyvsp[0]; }
break;
case 45:
{yyval = ""; }
break;
case 46:
{ yyval = yyvsp[0]; }
break;
case 47:
{yyval = ""; }
break;
case 48:
{ yyval = yyvsp[0]; }
break;
case 49:
{yyval = ""; }
break;
case 50:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 51:
{ yyval = yyvsp[-1] + yyvsp[0]; }
break;
case 52:
{yyval = yyvsp[-3] + yyvsp[-2] + yyvsp[-1]  + yyvsp[0];}
break;
case 53:
{yyval = yyvsp[-4] + yyvsp[-3] + yyvsp[-2]  + yyvsp[-1];}
break;
case 54:
{yyval = yyvsp[-7] + yyvsp[-6] + yyvsp[-5]  + yyvsp[-4] + yyvsp[-3] + yyvsp[-2] + yyvsp[-1];}
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
