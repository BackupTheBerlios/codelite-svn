#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define yyparse cl_scope_parse
#define yylex cl_scope_lex
#define yyerror cl_scope_error
#define yychar cl_scope_char
#define yyval cl_scope_val
#define yylval cl_scope_lval
#define yydebug cl_scope_debug
#define yynerrs cl_scope_nerrs
#define yyerrflag cl_scope_errflag
#define yyss cl_scope_ss
#define yyssp cl_scope_ssp
#define yyvs cl_scope_vs
#define yyvsp cl_scope_vsp
#define yylhs cl_scope_lhs
#define yylen cl_scope_len
#define yydefred cl_scope_defred
#define yydgoto cl_scope_dgoto
#define yysindex cl_scope_sindex
#define yyrindex cl_scope_rindex
#define yygindex cl_scope_gindex
#define yytable cl_scope_table
#define yycheck cl_scope_check
#define yyname cl_scope_name
#define yyrule cl_scope_rule
#define YYPREFIX "cl_scope_"
/* Copyright Eran Ifrah(c)*/
/*************** Includes and Defines *****************************/
#include "string"
#include "vector"
#include "stdio.h"
#include "map"
	
#define YYDEBUG_LEXER_TEXT (cl_scope_lval) 
#define YYSTYPE std::string
#define YYDEBUG 0        /* get the pretty debugging code to compile*/

int cl_scope_parse();
void cl_scope_error(char *string);
void syncParser();

/*---------------------------------------------*/
/* externs defined in the lexer*/
/*---------------------------------------------*/
extern char *cl_scope_text;
extern int cl_scope_lex();
extern bool setLexerInput(const std::string &in);
extern int cl_scope_lineno;
extern std::vector<std::string> currentScope;
/*extern std::map<std::string, bool> g_symbols;*/
extern void printScopeName();	/*print the current scope name*/
extern void increaseScope();	/*increase scope with anonymouse value*/
extern std::string getCurrentScope();
extern void cl_scope_lex_clean();

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
short cl_scope_lhs[] = {                                        -1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    0,
    0,    2,    2,    2,    2,    2,    2,    2,    2,   10,
   10,   12,   12,   11,   11,   13,   14,   14,   15,   15,
   15,   16,    7,    7,    3,    3,    8,    9,   21,   21,
    6,    6,   18,   18,   26,   25,   25,   22,   22,   17,
   17,   27,   27,   28,   28,   19,   20,   20,   23,   23,
   23,   29,   29,   30,   24,   24,   24,   32,    4,   33,
   33,   33,   33,   34,   31,   31,   31,    5,
};
short cl_scope_len[] = {                                         2,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    0,
    2,    1,    1,    1,    1,    1,    1,    1,    1,    0,
    2,    1,    3,    1,    1,    0,    0,    4,    0,    1,
    3,    4,    4,    3,    6,    6,    1,    1,    1,    1,
   11,   10,    0,    2,    2,    0,    2,    0,    1,    0,
    1,    0,    1,    0,    2,    2,    0,    1,    4,    4,
    7,    0,    1,    2,    0,    1,    3,    0,    7,    0,
    2,    2,    2,    2,    0,    1,    3,    4,
};
short cl_scope_defred[] = {                                     10,
    0,   19,   38,   58,   37,   11,   12,   13,   14,   15,
   16,   17,   18,    0,    0,    0,   49,    0,    0,    0,
    0,    0,    0,    0,    0,   34,   40,   39,   26,   51,
   43,   43,   68,   78,   24,   25,   22,    0,    0,   33,
    0,    0,    0,    0,   21,    0,   28,    0,    6,    1,
    4,    2,    5,    3,    8,    7,    9,    0,   54,   44,
    0,    0,    0,   76,   23,   36,   35,   45,    0,   60,
    0,   59,    0,    0,   74,    0,   69,    0,   30,   43,
   53,   55,   56,    0,    0,   66,   73,   72,   71,   77,
    0,   54,    0,   63,   64,    0,    0,   31,   61,    0,
   67,    0,   32,   42,    0,    0,   47,   41,
};
short cl_scope_dgoto[] = {                                       1,
   59,    6,    7,    8,    9,   10,   11,   12,   13,   37,
   38,   39,   41,   20,   78,   79,   31,   42,   70,   14,
   29,   21,   84,   85,  106,   60,   83,   71,   95,   86,
   63,   44,   75,   64,
};
short cl_scope_sindex[] = {                                      0,
  -49,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0, -240, -283, -254,    0,   -5, -112, -233,
 -210,  -48,  -44, -250,  -38,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0, -221,    3,    0,
 -220, -147, -205, -204,    0, -250,    0,  -42,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  -59,    0,    0,
  -40,   39,  -31,    0,    0,    0,    0,    0, -210,    0,
    8,    0, -210, -216,    0, -204,    0,   10,    0,    0,
    0,    0,    0, -198,    7,    0,    0,    0,    0,    0,
 -210,    0, -197,    0,    0, -210, -210,    0,    0, -218,
    0,  -39,    0,    0, -195,   51,    0,    0,
};
short cl_scope_rindex[] = {                                      0,
 -235,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0, -172,    0,    0,    0,    0,    0,    0,
 -139,    0,    0,   27,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  -30,    0,   27,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  -36,    0,    0,
    0,  -29,    0,    0,    0,    0,    0,    0,  -41,    0,
  -32,    0,  -37,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   15,    0,    0,    0,    0,    0,    0,
 -189,    0,    0,    0,    0, -139,  -43,    0,    0,   -9,
    0,   56,    0,    0,    0,    0,    0,    0,
};
short cl_scope_gindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   71,
    0,    0,    0,    0,    0,   32,  -27,  -13,  -23,    0,
    0,    0,   97,    0,    0,    0,    0,    0,    0,   29,
    0,    0,    0,   50,
};
#define YYTABLESIZE 287
short cl_scope_table[] = {                                      73,
   69,   54,   29,   65,   54,   54,   65,   54,   52,    4,
   26,   52,   76,   75,   70,   50,   67,   50,   43,   22,
   29,  105,   57,   57,   57,   15,   27,   57,   54,   52,
   57,   16,   54,   57,   54,   72,   57,   57,   57,   57,
   57,   80,   35,   57,   57,   81,   46,   97,   23,   82,
   96,   36,   54,   91,   24,   62,   17,   57,   62,   28,
   18,   57,   30,   80,   47,   57,   93,   57,   99,  102,
   20,   92,   19,    3,   33,    5,  103,   57,   34,   50,
   66,   45,   48,  104,   40,   48,   48,   27,   20,   87,
   48,   88,   89,   77,   75,   70,   48,   61,   62,   74,
   48,   48,   48,   48,   94,  100,   48,   48,   68,  108,
   49,   50,  107,   50,   46,   51,   65,   32,   50,   50,
   27,   52,   98,   50,  101,   90,   53,   54,   55,   50,
   48,   56,   57,    0,   50,   50,   50,    0,    0,   50,
   50,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   58,    0,    0,    0,    0,
    0,    0,    0,   50,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   25,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    2,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   50,   50,    0,    0,    0,   50,    0,    0,    0,    0,
    0,   50,    0,    0,    0,    0,   50,   50,   50,    0,
    0,   50,   50,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   50,    0,    0,    0,   50,   54,   68,    0,    0,
   52,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   68,
};
short cl_scope_check[] = {                                      40,
   60,   38,   44,   41,   41,   42,   44,   44,   41,   59,
  123,   44,   44,   44,   44,   59,   59,   61,   32,  303,
   62,   61,  258,  259,  260,  266,  260,  263,   38,   62,
  266,  272,   42,  269,   44,   59,  272,  273,  274,  275,
  276,   69,  293,  279,  280,   38,   44,   41,  303,   42,
   44,  302,   62,   44,   60,   41,  297,  293,   44,  293,
  301,  297,  273,   91,   62,  301,   80,  303,   92,   97,
   44,   62,  313,  123,  123,  125,  100,  313,  123,  123,
  123,  303,  303,  123,  123,  258,  259,  260,   62,  306,
  263,  308,  309,  125,  125,  125,  269,  303,  303,   61,
  273,  274,  275,  276,  303,  303,  279,  280,  327,   59,
  258,  259,  308,  303,   59,  263,   46,   21,  258,  259,
  293,  269,   91,  263,   96,   76,  274,  275,  276,  269,
  303,  279,  280,   -1,  274,  275,  276,   -1,   -1,  279,
  280,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  303,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  303,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  303,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  256,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  258,  259,   -1,   -1,   -1,  263,   -1,   -1,   -1,   -1,
   -1,  269,   -1,   -1,   -1,   -1,  274,  275,  276,   -1,
   -1,  279,  280,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  303,   -1,   -1,   -1,  303,  303,  327,   -1,   -1,
  303,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  327,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 339
#if YYDEBUG
char *cl_scope_name[] = {
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
char *cl_scope_rule[] = {
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
"external_decl : enum_decl",
"external_decl : union_decl",
"external_decl : function_decl",
"external_decl : namespace_decl",
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
"opt_template_qualifier :",
"opt_template_qualifier : LE_TEMPLATE '<' template_arg_list '>'",
"template_parameter_list :",
"template_parameter_list : template_parameter",
"template_parameter_list : template_parameter_list ',' template_parameter",
"template_parameter : const_spec nested_scope_specifier LE_IDENTIFIER special_star_amp",
"namespace_decl : stmnt_starter LE_NAMESPACE LE_IDENTIFIER '{'",
"namespace_decl : stmnt_starter LE_NAMESPACE '{'",
"class_decl : stmnt_starter opt_template_qualifier class_keyword opt_class_qualifier LE_IDENTIFIER ';'",
"class_decl : stmnt_starter opt_template_qualifier class_keyword opt_class_qualifier LE_IDENTIFIER '{'",
"scope_reducer : '}'",
"scope_increaer : '{'",
"class_keyword : LE_CLASS",
"class_keyword : LE_STRUCT",
"function_decl : stmnt_starter virtual_spec variable_decl nested_scope_specifier LE_IDENTIFIER '(' variable_list ')' const_spec pure_virtual_spec ';'",
"function_decl : stmnt_starter virtual_spec variable_decl nested_scope_specifier LE_IDENTIFIER '(' variable_list ')' const_spec '{'",
"nested_scope_specifier :",
"nested_scope_specifier : nested_scope_specifier scope_specifier",
"scope_specifier : LE_IDENTIFIER LE_CLCL",
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
"stmnt_starter :",
"stmnt_starter : ';'",
"variable_decl : const_spec nested_scope_specifier basic_type_name special_star_amp",
"variable_decl : const_spec nested_scope_specifier LE_IDENTIFIER special_star_amp",
"variable_decl : const_spec nested_scope_specifier LE_IDENTIFIER '<' template_parameter_list '>' special_star_amp",
"variable_name :",
"variable_name : LE_IDENTIFIER",
"variable_arg : variable_decl variable_name",
"variable_list :",
"variable_list : variable_arg",
"variable_list : variable_list ',' variable_arg",
"$$1 :",
"enum_decl : stmnt_starter LE_ENUM LE_IDENTIFIER '{' $$1 enum_arg_list '}'",
"enum_optional_assign :",
"enum_optional_assign : '=' LE_HEXconstant",
"enum_optional_assign : '=' LE_OCTALconstant",
"enum_optional_assign : '=' LE_INTEGERconstant",
"enum_argument : LE_IDENTIFIER enum_optional_assign",
"enum_arg_list :",
"enum_arg_list : enum_argument",
"enum_arg_list : enum_arg_list ',' enum_argument",
"union_decl : stmnt_starter LE_UNION LE_IDENTIFIER '{'",
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
	int ch = cl_scope_lex();
}

void consumeFuncArgList()
{
	int depth = 1;
	while(depth > 0)
	{
		int ch = cl_scope_lex();
		if(ch == EOF)
		{
			break;
		}
		if(ch == ')')
		{
			depth--;
			continue;
		}
		else if(ch == '(')
		{
			depth ++ ;
			continue;
		}
	}
}

/**
 * consume all token until matching closing brace is found
 */
void consumeDecl()
{
	int depth = 1;
	while(depth > 0)
	{
		int ch = cl_scope_lex();
		//printf("ch=%d\n", ch);
		fflush(stdout);
		if(ch ==0)
		{
			break;
		}
		if(ch == '}')
		{
			depth--;
			if(depth == 0) currentScope.pop_back();//reduce the scope
			continue;
		}
		else if(ch == '{')
		{
			depth ++ ;
			continue;
		}
	}
	
}

// return the scope name at the end of the input string
std::string get_scope_name(const std::string &in)
{
	if( !setLexerInput(in) )
	{
		return "";
	}
	//call tghe main parsing routine
	cl_scope_parse();
	std::string scope = getCurrentScope();
	//do the lexer cleanup
	cl_scope_lex_clean();
	return scope;
}

#ifdef __YMAIN
int main()
{
	FILE *fp;
	long len;
	char *buf = NULL;
	
	fp = fopen("test.h", "rb");
	if(!fp)
	{
		printf("failed to open file 'test.h': %s\n", strerror(errno));
		return false;
	}	
	
	//read the whole file
	fseek(fp, 0, SEEK_END); 		//go to end
	len = ftell(fp); 					//get position at end (length)
	fseek(fp, 0, SEEK_SET); 		//go to begining
	buf = (char *)malloc(len+1); 	//malloc buffer
	
	//read into buffer
	long bytes = fread(buf, sizeof(char), len, fp);
	printf("read: %ld\n", bytes);
	if(bytes != len)
	{
		fclose(fp);
		printf("failed to read from file 'test.h': %s\n", strerror(errno));
		return false;
	}
	
	buf[len] = 0;	// make it null terminated string
	fclose(fp);
	
	std::string scope = get_scope_name(buf);
	free(buf);
}
#endif // __YMAIN
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
case 19:
{ 
								printf("CodeLite: syntax error, unexpected token '%s' found at line %d \n", cl_scope_text, cl_scope_lineno);
								syncParser();
							}
break;
case 20:
{ yyval = "";}
break;
case 21:
{yyval = yyvsp[-1] + " " + yyvsp[0];}
break;
case 22:
{ yyval = yyvsp[0]; }
break;
case 23:
{ yyval = yyvsp[-2] + " " + yyvsp[-1] + " " + yyvsp[0]; }
break;
case 24:
{ yyval = yyvsp[0]; }
break;
case 25:
{ yyval = yyvsp[0]; }
break;
case 26:
{ yyval = "";}
break;
case 28:
{ yyval = yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 29:
{yyval = "";}
break;
case 30:
{yyval = yyvsp[0];}
break;
case 31:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 32:
{yyval = yyvsp[-3] + yyvsp[-2] + yyvsp[-1] +yyvsp[0];}
break;
case 33:
{
							/*g_symbols[$3] = true;*/
							currentScope.push_back(yyvsp[-1]);
						}
break;
case 34:
{
							/*anonymouse namespace*/
							printf("found anonymous namespace\n");
							increaseScope();
							printScopeName();
						}
break;
case 35:
{
					printf("Found class decl: %s\n", yyvsp[-1].c_str());
					/*g_symbols[$5] = true;*/
				}
break;
case 36:
{
					printf("Found class impl: %s\n", yyvsp[-1].c_str());
					/*add new symbol to symbol table*/
					/*g_symbols[$5] = true;*/
					
					/*increase the scope level*/
					currentScope.push_back(yyvsp[-1]);
					printScopeName();
				}
break;
case 37:
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
case 38:
{
								/*increase random scope*/
								increaseScope();
								printScopeName();
							 }
break;
case 39:
{yyval = yyvsp[0];}
break;
case 40:
{yyval = yyvsp[0];}
break;
case 41:
{
							printf("Found function: %s\n", yyvsp[-6].c_str());
						}
break;
case 42:
{
							printf("Found function: %s\n", yyvsp[-5].c_str());
							currentScope.push_back(yyvsp[-5]);
							printScopeName();
						}
break;
case 43:
{yyval = "";}
break;
case 44:
{yyval = yyvsp[-1] + " " + yyvsp[0];}
break;
case 45:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 46:
{yyval = ""; }
break;
case 47:
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
break;
case 48:
{yyval = ""; }
break;
case 49:
{ yyval = yyvsp[0]; }
break;
case 50:
{yyval = ""; }
break;
case 51:
{ yyval = yyvsp[0]; }
break;
case 52:
{yyval = ""; }
break;
case 53:
{ yyval = yyvsp[0]; }
break;
case 54:
{yyval = ""; }
break;
case 55:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 56:
{ yyval = yyvsp[-1] + yyvsp[0]; }
break;
case 57:
{yyval = "";}
break;
case 58:
{yyval = ";";}
break;
case 59:
{yyval = yyvsp[-3] + yyvsp[-2] + yyvsp[-1]  + yyvsp[0];}
break;
case 60:
{yyval = yyvsp[-3] + yyvsp[-2] + yyvsp[-1]  + yyvsp[0];}
break;
case 61:
{yyval = yyvsp[-6] + yyvsp[-5] + yyvsp[-4]  + yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 62:
{yyval = "";}
break;
case 63:
{yyval = yyvsp[0];}
break;
case 64:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 65:
{yyval = "";}
break;
case 66:
{yyval = yyvsp[0];}
break;
case 67:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 68:
{currentScope.push_back(yyvsp[-1]); printScopeName();}
break;
case 69:
{	
							currentScope.pop_back();/*reduce the scope*/
							printScopeName();
							printf("found enum: %s, args are: %s\n", yyvsp[-5].c_str(), yyvsp[-2].c_str());
						}
break;
case 70:
{yyval = "";}
break;
case 71:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 72:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 73:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 74:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 75:
{yyval = "";}
break;
case 76:
{yyval = yyvsp[0];}
break;
case 77:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 78:
{	
								currentScope.push_back(yyvsp[-1]);
								printScopeName();
								consumeDecl();
							}
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
