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

static bool g_onlyNamedScope = true;

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
#define LE_MACRO 340
#define YYERRCODE 256
short cl_scope_lhs[] = {                                        -1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    0,
    0,    2,    2,    2,    2,    2,    2,    2,    2,   10,
   10,   12,   12,   11,   11,   13,   13,   14,   14,   14,
   15,   17,   17,   16,   16,   16,   16,   18,   18,   18,
   19,    7,    7,   24,   24,    3,    3,    8,    9,   25,
   25,   26,   26,   27,   27,   27,   27,   27,   27,   27,
   27,   27,   27,   27,   27,   27,   27,   27,   27,   27,
   27,   27,   27,   27,   27,   27,   27,   27,   27,   27,
   27,   27,   27,   27,    6,   21,   21,   31,   28,   28,
   20,   20,   32,   32,   33,   33,   22,   23,   23,   23,
   29,   29,   29,   34,   34,   35,   30,   30,   30,   37,
    4,   38,   38,   38,   38,   39,   36,   36,   36,    5,
};
short cl_scope_len[] = {                                         2,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    0,
    2,    1,    1,    1,    1,    1,    1,    1,    1,    0,
    2,    1,    3,    1,    1,    0,    4,    0,    1,    3,
    3,    0,    3,    0,    1,    1,    1,    0,    1,    3,
    4,    4,    3,    0,    1,    6,    8,    1,    1,    1,
    1,    1,    2,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    2,
    2,    1,    1,    1,   11,    0,    2,    2,    0,    1,
    0,    1,    0,    1,    0,    2,    2,    0,    1,    1,
    3,    3,    6,    0,    1,    3,    0,    1,    3,    0,
    7,    0,    2,    2,    2,    2,    0,    1,    3,    4,
};
short cl_scope_defred[] = {                                     10,
    0,   19,   49,  100,   48,   99,   11,   12,   13,   14,
   15,   16,   17,   18,    0,    0,    0,   90,    0,    0,
    0,    0,    0,    0,    0,    0,   43,   51,   50,    0,
   92,   86,  110,  120,   24,   25,   22,    0,    0,   42,
   45,    0,    0,   86,    0,   21,    0,   27,    0,    6,
    1,    4,    2,    5,    3,    8,    7,    9,    0,   95,
   87,    0,    0,    0,  118,   23,   46,    0,   88,    0,
  102,    0,  101,    0,    0,    0,    0,  116,    0,  111,
   35,   37,   36,    0,   29,    0,    0,   39,   86,   96,
   94,   97,   82,   83,   70,   74,   75,   66,   67,   76,
   77,   78,   79,   68,   69,   73,   71,   84,   64,   65,
   54,   55,   56,   57,   58,   59,   60,   61,   62,   63,
   72,    0,    0,   53,    0,  115,  114,  113,  119,    0,
   47,    0,    0,   95,    0,   80,   81,   86,    0,  108,
   30,    0,   31,   40,  103,    0,    0,    0,    0,    0,
   41,  105,  106,  109,    0,   33,   85,
};
short cl_scope_dgoto[] = {                                       1,
   60,    7,    8,    9,   10,   11,   12,   13,   14,   37,
   38,   39,   21,   84,   85,   86,  143,   87,   88,   89,
   43,   71,   15,   42,   30,   76,  124,   22,   44,  139,
   61,   92,   72,  153,  140,   64,   45,   78,   65,
};
short cl_scope_sindex[] = {                                      0,
  -28,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0, -228, -278, -255,    0,   -6, -107,
 -221, -218,  -61,  -56, -210,  -48,    0,    0,    0, -263,
    0,    0,    0,    0,    0,    0,    0, -215,   -3,    0,
    0, -182, -121,    0, -181,    0, -210,    0,  -25,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  -59,    0,
    0, -216,   21,  -22,    0,    0,    0, -186,    0, -218,
    0,   58,    0,  -33, -203,   85, -207,    0, -181,    0,
    0,    0,    0,  -16,    0, -177,    3,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   90,   39,    0, -218,    0,    0,    0,    0, -186,
    0,   76, -218,    0, -168,    0,    0,    0,   74,    0,
    0, -218,    0,    0,    0, -203, -164, -218, -218,   24,
    0,    0,    0,    0,   17,    0,    0,
};
short cl_scope_rindex[] = {                                      0,
 -223,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0, -146,    0,    0,    0,    0,    0,
    0, -113,    0,    0,   27,    0,    0,    0,    0, -162,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  -21,    0,   27,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  -23,    0,
    0,    0,  -20,    0,    0,    0,    0,  -42,    0,  -36,
    0,    1,    0,    0,  109,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  -38,    0,    0,    0,    0, -152,
    0,  -12, -151,    0,    0,    0,    0,    0,    0,    0,
    0,  -36,    0,    0,    0,   22,   75, -113,   37,    0,
    0,    0,    0,    0,    0,    0,    0,
};
short cl_scope_gindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  117,
    0,    0,    0,    0,   35,    0,    0,   26,   36,   -5,
  -10,  -40,    0,    0,    0,    0,    0,    0,   32,    0,
    0,    0,    0,    0,   23,    0,    0,    0,   93,
};
#define YYTABLESIZE 304
short cl_scope_table[] = {                                     120,
   70,   28,  107,  115,  117,  107,  122,   38,  113,  111,
  108,  112,  121,  114,   95,   27,   32,   95,   95,   73,
   95,   79,  117,  112,   23,   38,  109,  130,  110,    4,
    6,   32,   68,   62,   98,   98,   98,   16,   28,   98,
   47,   93,   98,   17,   93,   98,  133,   24,   98,   98,
   98,   98,   98,   25,   31,   98,   98,  123,   48,   95,
  116,   33,   93,   95,  134,   95,   34,  133,   18,   98,
   20,   29,   19,   98,   40,   74,   41,   98,  135,   98,
   28,   77,   35,   95,   20,  156,   75,   46,   20,   98,
  118,   36,  119,  145,    3,   91,    5,   67,  126,   90,
  127,  128,   80,  117,  112,  151,  131,   81,   82,   83,
   32,   89,   89,   26,  149,  104,   89,  148,  104,  138,
   49,   63,   89,   69,  125,  132,   89,   89,   89,   89,
  136,  137,   89,   89,  146,  142,   50,   51,  152,  157,
   44,   52,  138,  155,   91,   91,   26,   53,   52,   91,
   34,   91,   54,   55,   56,   91,   89,   57,   58,   91,
   91,   91,   91,   66,  141,   91,   91,  150,  144,  147,
  154,  129,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   59,    0,    0,    0,    0,    0,    0,    0,   91,
    0,    0,    0,    0,    0,   26,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   91,
   91,    0,    0,    0,   91,    0,    0,    2,    0,    0,
   91,    0,    0,    0,    0,   91,   91,   91,    0,    0,
   91,   91,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   93,   94,    0,    0,    0,
   34,    0,    0,    0,   91,    0,   91,   69,   95,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   95,
    0,   95,   96,   97,   98,   99,  100,  101,  102,  103,
  104,  105,   93,    0,  106,  107,    0,    0,    0,    0,
    0,    0,    0,   93,
};
short cl_scope_check[] = {                                      33,
   60,   44,   41,   37,   38,   44,   40,   44,   42,   43,
   44,   45,   46,   47,   38,  123,   22,   41,   42,   60,
   44,   44,   44,   44,  303,   62,   60,   44,   62,   58,
   59,   44,   58,   44,  258,  259,  260,  266,  260,  263,
   44,   41,  266,  272,   44,  269,   44,  303,  272,  273,
  274,  275,  276,   60,  273,  279,  280,   91,   62,   38,
   94,  123,   62,   42,   62,   44,  123,   44,  297,  293,
   44,  293,  301,  297,  123,  292,  340,  301,   89,  303,
  123,   61,  293,   62,  313,   62,  303,  303,   62,  313,
  124,  302,  126,  134,  123,   38,  125,  123,  306,   42,
  308,  309,  125,  125,  125,  146,  123,  294,  295,  296,
  123,  258,  259,  260,   41,   41,  263,   44,   44,  125,
  303,  303,  269,  327,   40,  303,  273,  274,  275,  276,
   41,   93,  279,  280,  303,   60,  258,  259,  303,  123,
  303,  263,  148,  149,  258,  259,  293,  269,   40,  263,
  303,  303,  274,  275,  276,  269,  303,  279,  280,  123,
  274,  275,  276,   47,  130,  279,  280,  142,  133,  138,
  148,   79,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  303,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  303,
   -1,   -1,   -1,   -1,   -1,  303,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  258,
  259,   -1,   -1,   -1,  263,   -1,   -1,  256,   -1,   -1,
  269,   -1,   -1,   -1,   -1,  274,  275,  276,   -1,   -1,
  279,  280,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  289,  290,   -1,   -1,   -1,
  303,   -1,   -1,   -1,  303,   -1,  303,  327,  292,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  303,
   -1,  315,  316,  317,  318,  319,  320,  321,  322,  323,
  324,  325,  292,   -1,  328,  329,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  303,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 340
#if YYDEBUG
char *cl_scope_name[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"'!'",0,0,0,"'%'","'&'",0,"'('","')'","'*'","'+'","','","'-'","'.'","'/'",0,0,0,
0,0,0,0,0,0,0,"':'","';'","'<'","'='","'>'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,"'['",0,"']'","'^'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,"'{'","'|'","'}'","'~'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"LE_AUTO",
"LE_DOUBLE","LE_INT","LE_STRUCT","LE_BREAK","LE_ELSE","LE_LONG","LE_SWITCH",
"LE_CASE","LE_ENUM","LE_REGISTER","LE_TYPEDEF","LE_CHAR","LE_EXTERN",
"LE_RETURN","LE_UNION","LE_CONST","LE_FLOAT","LE_SHORT","LE_UNSIGNED",
"LE_CONTINUE","LE_FOR","LE_SIGNED","LE_VOID","LE_DEFAULT","LE_GOTO","LE_SIZEOF",
"LE_VOLATILE","LE_DO","LE_IF","LE_STATIC","LE_WHILE","LE_NEW","LE_DELETE",
"LE_THIS","LE_OPERATOR","LE_CLASS","LE_PUBLIC","LE_PROTECTED","LE_PRIVATE",
"LE_VIRTUAL","LE_FRIEND","LE_INLINE","LE_OVERLOAD","LE_TEMPLATE","LE_TYPENAME",
"LE_IDENTIFIER","LE_STRINGliteral","LE_FLOATINGconstant","LE_INTEGERconstant",
"LE_CHARACTERconstant","LE_OCTALconstant","LE_HEXconstant","LE_POUNDPOUND",
"LE_CComment","LE_CPPComment","LE_NAMESPACE","LE_TYPEDEFname","LE_ARROW",
"LE_ICR","LE_DECR","LE_LS","LE_RS","LE_LE","LE_GE","LE_EQ","LE_NE","LE_ANDAND",
"LE_OROR","LE_ELLIPSIS","LE_CLCL","LE_DOTstar","LE_ARROWstar","LE_MULTassign",
"LE_DIVassign","LE_MODassign","LE_PLUSassign","LE_MINUSassign","LE_LSassign",
"LE_RSassign","LE_ANDassign","LE_ERassign","LE_ORassign","LE_MACRO",
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
"opt_template_qualifier :",
"opt_template_qualifier : LE_TEMPLATE '<' template_arg_list '>'",
"derivation_list :",
"derivation_list : parent_class",
"derivation_list : derivation_list ',' parent_class",
"parent_class : access_specifier LE_IDENTIFIER opt_template_specifier",
"opt_template_specifier :",
"opt_template_specifier : '<' template_parameter_list '>'",
"access_specifier :",
"access_specifier : LE_PUBLIC",
"access_specifier : LE_PRIVATE",
"access_specifier : LE_PROTECTED",
"template_parameter_list :",
"template_parameter_list : template_parameter",
"template_parameter_list : template_parameter_list ',' template_parameter",
"template_parameter : const_spec nested_scope_specifier LE_IDENTIFIER special_star_amp",
"namespace_decl : stmnt_starter LE_NAMESPACE LE_IDENTIFIER '{'",
"namespace_decl : stmnt_starter LE_NAMESPACE '{'",
"opt_class_qualifier :",
"opt_class_qualifier : LE_MACRO",
"class_decl : stmnt_starter opt_template_qualifier class_keyword opt_class_qualifier LE_IDENTIFIER '{'",
"class_decl : stmnt_starter opt_template_qualifier class_keyword opt_class_qualifier LE_IDENTIFIER ':' derivation_list '{'",
"scope_reducer : '}'",
"scope_increaer : '{'",
"class_keyword : LE_CLASS",
"class_keyword : LE_STRUCT",
"func_name : LE_IDENTIFIER",
"func_name : LE_OPERATOR any_operator",
"any_operator : '+'",
"any_operator : '-'",
"any_operator : '*'",
"any_operator : '/'",
"any_operator : '%'",
"any_operator : '^'",
"any_operator : '&'",
"any_operator : '|'",
"any_operator : '~'",
"any_operator : '!'",
"any_operator : '<'",
"any_operator : '>'",
"any_operator : LE_LS",
"any_operator : LE_RS",
"any_operator : LE_ANDAND",
"any_operator : LE_OROR",
"any_operator : LE_ARROW",
"any_operator : LE_ARROWstar",
"any_operator : '.'",
"any_operator : LE_DOTstar",
"any_operator : LE_ICR",
"any_operator : LE_DECR",
"any_operator : LE_LE",
"any_operator : LE_GE",
"any_operator : LE_EQ",
"any_operator : LE_NE",
"any_operator : '(' ')'",
"any_operator : '[' ']'",
"any_operator : LE_NEW",
"any_operator : LE_DELETE",
"any_operator : ','",
"function_decl : stmnt_starter virtual_spec const_spec variable_decl nested_scope_specifier func_name '(' variable_list ')' const_spec '{'",
"nested_scope_specifier :",
"nested_scope_specifier : nested_scope_specifier scope_specifier",
"scope_specifier : LE_IDENTIFIER LE_CLCL",
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
"stmnt_starter : ':'",
"variable_decl : nested_scope_specifier basic_type_name special_star_amp",
"variable_decl : nested_scope_specifier LE_IDENTIFIER special_star_amp",
"variable_decl : nested_scope_specifier LE_IDENTIFIER '<' template_parameter_list '>' special_star_amp",
"variable_name :",
"variable_name : LE_IDENTIFIER",
"variable_arg : const_spec variable_decl variable_name",
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
	//int ch = cl_scope_lex();
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
std::string get_scope_name(const std::string &in, bool onlyNamedScope)
{
	if( !setLexerInput(in) )
	{
		return "";
	}
	
	g_onlyNamedScope = onlyNamedScope;
	
	//call tghe main parsing routine
	cl_scope_parse();
	std::string scope = getCurrentScope();
	//do the lexer cleanup
	cl_scope_lex_clean();
	return scope;
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
case 19:
{ 
								/*printf("CodeLite: syntax error, unexpected token '%s' found at line %d \n", cl_scope_text, cl_scope_lineno);*/
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
case 27:
{ yyval = yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 28:
{yyval = "";}
break;
case 29:
{yyval = yyvsp[0];}
break;
case 30:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 31:
{yyval = yyvsp[-2] + " " + yyvsp[-1] + yyvsp[0];}
break;
case 32:
{yyval = "";}
break;
case 33:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 34:
{yyval = "";}
break;
case 35:
{yyval = yyvsp[0];}
break;
case 36:
{yyval = yyvsp[0];}
break;
case 37:
{yyval = yyvsp[0];}
break;
case 38:
{yyval = "";}
break;
case 39:
{yyval = yyvsp[0];}
break;
case 40:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 41:
{yyval = yyvsp[-3] + yyvsp[-2] + yyvsp[-1] +yyvsp[0];}
break;
case 42:
{
							currentScope.push_back(yyvsp[-1]);
						}
break;
case 43:
{
							/*anonymouse namespace*/
							increaseScope();
							printScopeName();
						}
break;
case 44:
{yyval = "";}
break;
case 45:
{yyval = yyvsp[0];}
break;
case 46:
{
					/*increase the scope level*/
					currentScope.push_back(yyvsp[-1]);
					printScopeName();
				}
break;
case 47:
{
					/*increase the scope level*/
					currentScope.push_back(yyvsp[-3]);
					printScopeName();
				}
break;
case 48:
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
case 49:
{
								/*increase random scope*/
								increaseScope();
								printScopeName();
							 }
break;
case 50:
{yyval = yyvsp[0];}
break;
case 51:
{yyval = yyvsp[0];}
break;
case 85:
{
						/*trim down trailing '::' from scope name*/
						yyvsp[-6].erase(yyvsp[-6].find_last_not_of(":")+1);
						currentScope.push_back(yyvsp[-6]);
						printScopeName();
					}
break;
case 86:
{yyval = "";}
break;
case 87:
{	yyval = yyvsp[-1] + " " + yyvsp[0];}
break;
case 88:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 89:
{yyval = ""; }
break;
case 90:
{ yyval = yyvsp[0]; }
break;
case 91:
{yyval = ""; }
break;
case 92:
{ yyval = yyvsp[0]; }
break;
case 93:
{yyval = ""; }
break;
case 94:
{ yyval = yyvsp[0]; }
break;
case 95:
{yyval = ""; }
break;
case 96:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 97:
{ yyval = yyvsp[-1] + yyvsp[0]; }
break;
case 98:
{yyval = "";}
break;
case 99:
{ yyval = ";";}
break;
case 100:
{ yyval = ":";}
break;
case 101:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0]  ;}
break;
case 102:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0]  ;}
break;
case 103:
{yyval = yyvsp[-5] + yyvsp[-4] + yyvsp[-3]  + yyvsp[-2] + yyvsp[-1] + yyvsp[0] ;}
break;
case 104:
{yyval = "";}
break;
case 105:
{yyval = yyvsp[0];}
break;
case 106:
{yyval = yyvsp[-2] + yyvsp[-1];}
break;
case 107:
{yyval = "";}
break;
case 108:
{yyval = yyvsp[0];}
break;
case 109:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 110:
{currentScope.push_back(yyvsp[-1]); printScopeName();}
break;
case 111:
{	
							currentScope.pop_back();/*reduce the scope*/
							printScopeName();
							printf("found enum: %s, args are: %s\n", yyvsp[-5].c_str(), yyvsp[-2].c_str());
						}
break;
case 112:
{yyval = "";}
break;
case 113:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 114:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 115:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 116:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 117:
{yyval = "";}
break;
case 118:
{yyval = yyvsp[0];}
break;
case 119:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 120:
{	
								currentScope.push_back(yyvsp[-1]);
								printScopeName();
								consumeDecl();
								printScopeName();
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
