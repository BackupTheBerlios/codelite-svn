%{
/* Included code before lex code */
/*************** Includes and Defines *****************************/


#include "map"
#include "cpp_lexer.h"		// YACC generated definitions based on C++ grammar
#include "errno.h"

#define YYSTYPE std::string

#include "string"
#include <stdlib.h>
#include <string.h>
#include <vector>

extern std::string cl_scope_lval;
extern std::string cl_var_lval;

std::vector<std::string> currentScope;

bool setLexerInput(const std::string &in);
std::string getCurrentScope();
void printScopeName();
void cl_scope_lex_clean();

//we keep a very primitive map with only symbol name
//that we encountered so far
std::map<std::string, bool> g_symbols;
std::map<std::string, bool> g_macros;

bool isaTYPE(char *string);
bool isaMACRO(char *string);
static bool defineFound = false;

/* Prototypes */
#define WHITE_RETURN(x) /* do nothing */

#define PA_KEYWORD_RETURN(x)   RETURN_VAL(x)  /* standard C PArser Keyword */
#define CPP_KEYWORD_RETURN(x)  PA_KEYWORD_RETURN(x)  /* C++ keyword */
#define PPPA_KEYWORD_RETURN(x) RETURN_VAL(x)  /* both PreProcessor and PArser keyword */
#define PP_KEYWORD_RETURN(x)   IDENTIFIER_RETURN()

#define IDENTIFIER_RETURN(){\
										if(isaTYPE(yytext)){\
											RETURN_VAL(LE_TYPEDEFname);\
										}else if(isaMACRO(yytext)){\
											RETURN_VAL(LE_MACRO);\
										}else{ RETURN_VAL(LE_IDENTIFIER);}\
									}


#define PPOP_RETURN(x)       RETURN_VAL((int)*yytext) /* PreProcess and Parser operator */
#define NAMED_PPOP_RETURN(x) RETURN_VAL(x)
#define ASCIIOP_RETURN(x)    RETURN_VAL((int)*yytext) /* a single character operator */
#define NAMEDOP_RETURN(x)    RETURN_VAL(x)            /* a multichar operator, with a name */

#define NUMERICAL_RETURN(x) RETURN_VAL(x)            /* some sort of constant */
#define LITERAL_RETURN(x)   RETURN_VAL(x)            /* a string literal */
#define C_COMMENT_RETURN(x) RETURN_VAL(x)	     /* C Style comment  */
#define RETURN_VAL(x) {\
								cl_scope_lval = yytext;\
								cl_var_lval = yytext;\
								return(x);}

%}

%option yylineno

identifier [a-zA-Z_][0-9a-zA-Z_]*

exponent_part [eE][-+]?[0-9]+
fractional_constant ([0-9]*"."[0-9]+)|([0-9]+".")
floating_constant (({fractional_constant}{exponent_part}?)|([0-9]+{exponent_part}))[FfLl]?

integer_suffix_opt ([uU]?[lL]?)|([lL][uU])
decimal_constant [1-9][0-9]*{integer_suffix_opt}
octal_constant "0"[0-7]*{integer_suffix_opt}
hex_constant "0"[xX][0-9a-fA-F]+{integer_suffix_opt}

simple_escape [abfnrtv'"?\\]
octal_escape  [0-7]{1,3}
hex_escape "x"[0-9a-fA-F]+

escape_sequence [\\]({simple_escape}|{octal_escape}|{hex_escape})
c_char [^'\\\n]|{escape_sequence}
s_char [^"\\\n]|{escape_sequence}

h_tab [\011]
form_feed [\014]
v_tab [\013]
c_return [\015]

horizontal_white [ ]|{h_tab}

%x PREPR
%x WRAP_PREP

%%

"/*" {
	/* Handle C-Style comment */
	register int c;
	for ( ; ; )
	{
		while ( (c = yyinput()) != '*' && c != EOF )
		{
			; /* eat up text of comment */
		}
		if ( c == '*' )
		{
			while ( (c = yyinput()) == '*' )
			{
				;
			}
			if ( c == '/' )
			{
				break; /* found the end */
			}
		}
		if ( c == EOF )
		{
			break;
		}
	}
     }

"//" {
	register int c;
	while((c = yyinput()) != '\n' && c != EOF)
	{
		;
	}
     }

{horizontal_white}+     {
			WHITE_RETURN(' ');
			}

({v_tab}|{c_return}|{form_feed})+   {
			WHITE_RETURN(' ');
			}


({horizontal_white}|{v_tab}|{c_return}|{form_feed})*"\n"   {
			WHITE_RETURN('\n');
			}

auto                {PA_KEYWORD_RETURN(LE_AUTO);}
break               {PA_KEYWORD_RETURN(LE_BREAK);}
case                {PA_KEYWORD_RETURN(LE_CASE);}
char                {PA_KEYWORD_RETURN(LE_CHAR);}
const               {PA_KEYWORD_RETURN(LE_CONST);}
continue            {PA_KEYWORD_RETURN(LE_CONTINUE);}
default             {PA_KEYWORD_RETURN(LE_DEFAULT);}
define             {PP_KEYWORD_RETURN(LE_DEFINE);}
defined            {PP_KEYWORD_RETURN(LE_OPDEFINED);}
do                  {PA_KEYWORD_RETURN(LE_DO);}
double              {PA_KEYWORD_RETURN(LE_DOUBLE);}
elif                {PP_KEYWORD_RETURN(LE_ELIF);}
else              {PPPA_KEYWORD_RETURN(LE_ELSE);}
endif              {PP_KEYWORD_RETURN(LE_ENDIF);}
enum                {PA_KEYWORD_RETURN(LE_ENUM);}
error              {PP_KEYWORD_RETURN(LE_ERROR);}
extern              {PA_KEYWORD_RETURN(LE_EXTERN);}
float               {PA_KEYWORD_RETURN(LE_FLOAT);}
for                 {PA_KEYWORD_RETURN(LE_FOR);}
goto                {PA_KEYWORD_RETURN(LE_GOTO);}
if                {PPPA_KEYWORD_RETURN(LE_IF);}
ifdef              {PP_KEYWORD_RETURN(LE_IFDEF);}
ifndef             {PP_KEYWORD_RETURN(LE_IFNDEF);}
include            {PP_KEYWORD_RETURN(LE_INCLUDE); }
int                 {PA_KEYWORD_RETURN(LE_INT);}
line               {PP_KEYWORD_RETURN(LE_LINE);}
long                {PA_KEYWORD_RETURN(LE_LONG);}
pragma             {PP_KEYWORD_RETURN(LE_PRAGMA);}
register            {PA_KEYWORD_RETURN(LE_REGISTER);}
return              {PA_KEYWORD_RETURN(LE_RETURN);}
short               {PA_KEYWORD_RETURN(LE_SHORT);}
signed              {PA_KEYWORD_RETURN(LE_SIGNED);}
sizeof              {PA_KEYWORD_RETURN(LE_SIZEOF);}
static              {PA_KEYWORD_RETURN(LE_STATIC);}
struct              {PA_KEYWORD_RETURN(LE_STRUCT);}
switch              {PA_KEYWORD_RETURN(LE_SWITCH);}
typedef             {PA_KEYWORD_RETURN(LE_TYPEDEF);}
undef              {PP_KEYWORD_RETURN(LE_UNDEF);}
union               {PA_KEYWORD_RETURN(LE_UNION);}
unsigned            {PA_KEYWORD_RETURN(LE_UNSIGNED);}
void                {PA_KEYWORD_RETURN(LE_VOID);}
volatile            {PA_KEYWORD_RETURN(LE_VOLATILE);}
while               {PA_KEYWORD_RETURN(LE_WHILE);}


class               {CPP_KEYWORD_RETURN(LE_CLASS);}
namespace           {CPP_KEYWORD_RETURN(LE_NAMESPACE);}
delete              {CPP_KEYWORD_RETURN(LE_DELETE);}
friend              {CPP_KEYWORD_RETURN(LE_FRIEND);}
inline              {CPP_KEYWORD_RETURN(LE_INLINE);}
new                 {CPP_KEYWORD_RETURN(LE_NEW);}
operator            {CPP_KEYWORD_RETURN(LE_OPERATOR);}
overload            {CPP_KEYWORD_RETURN(LE_OVERLOAD);}
protected           {CPP_KEYWORD_RETURN(LE_PROTECTED);}
private             {CPP_KEYWORD_RETURN(LE_PRIVATE);}
public              {CPP_KEYWORD_RETURN(LE_PUBLIC);}
this                {CPP_KEYWORD_RETURN(LE_THIS);}
virtual             {CPP_KEYWORD_RETURN(LE_VIRTUAL);}
template				  {CPP_KEYWORD_RETURN(LE_TEMPLATE);}
typename				  {CPP_KEYWORD_RETURN(LE_TYPENAME);}
dynamic_cast		{CPP_KEYWORD_RETURN(LE_DYNAMIC_CAST);}
static_cast			{CPP_KEYWORD_RETURN(LE_STATIC_CAST);}
const_cast			{CPP_KEYWORD_RETURN(LE_CONST_CAST);}
reinterpret_cast 	{CPP_KEYWORD_RETURN(LE_REINTERPRET_CAST);}

{identifier}          {IDENTIFIER_RETURN();}

{decimal_constant}  {NUMERICAL_RETURN(LE_INTEGERconstant);}
{octal_constant}    {NUMERICAL_RETURN(LE_OCTALconstant);}
{hex_constant}      {NUMERICAL_RETURN(LE_HEXconstant);}
{floating_constant} {NUMERICAL_RETURN(LE_FLOATINGconstant);}


"L"?[']{c_char}+[']     {
			NUMERICAL_RETURN(LE_CHARACTERconstant);
			}


"L"?["]{s_char}*["]     {
			LITERAL_RETURN(LE_STRINGliteral);}




"("                  {PPOP_RETURN(LE_LP);}
")"                  {PPOP_RETURN(LE_RP);}
","                  {PPOP_RETURN(LE_COMMA);}
^"#"                 {BEGIN PREPR;}
"{"                  {ASCIIOP_RETURN(LE_LC);}
"}"                  {ASCIIOP_RETURN(LE_RC);}
"["                  {ASCIIOP_RETURN(LE_LB);}
"]"                  {ASCIIOP_RETURN(LE_RB);}
"."                  {ASCIIOP_RETURN(LE_DOT);}
"&"                  {ASCIIOP_RETURN(LE_AND);}
"*"                  {ASCIIOP_RETURN(LE_STAR);}
"+"                  {ASCIIOP_RETURN(LE_PLUS);}
"-"                  {ASCIIOP_RETURN(LE_MINUS);}
"~"                  {ASCIIOP_RETURN(LE_NEGATE);}
"!"                  {ASCIIOP_RETURN(LE_NOT);}
"/"                  {ASCIIOP_RETURN(LE_DIV);}
"%"                  {ASCIIOP_RETURN(LE_MOD);}
"<"                  {ASCIIOP_RETURN(LE_LT);}
">"                  {ASCIIOP_RETURN(LE_GT);}
"^"                  {ASCIIOP_RETURN(LE_XOR);}
"|"                  {ASCIIOP_RETURN(LE_PIPE);}
"?"                  {ASCIIOP_RETURN(LE_QUESTION);}
":"                  {ASCIIOP_RETURN(LE_COLON);}
";"                  {ASCIIOP_RETURN(LE_SEMICOLON);}
"="                  {ASCIIOP_RETURN(LE_ASSIGN);}

".*"                 {NAMEDOP_RETURN(LE_DOTstar);}
"::"                 {NAMEDOP_RETURN(LE_CLCL);}
"->"                 {NAMEDOP_RETURN(LE_ARROW);}
"->*"                {NAMEDOP_RETURN(LE_ARROWstar);}
"++"                 {NAMEDOP_RETURN(LE_ICR);}
"--"                 {NAMEDOP_RETURN(LE_DECR);}
"<<"                 {NAMEDOP_RETURN(LE_LS);}
">>"                 {NAMEDOP_RETURN(LE_RS);}
"<="                 {NAMEDOP_RETURN(LE_LE);}
">="                 {NAMEDOP_RETURN(LE_GE);}
"=="                 {NAMEDOP_RETURN(LE_EQ);}
"!="                 {NAMEDOP_RETURN(LE_NE);}
"&&"                 {NAMEDOP_RETURN(LE_ANDAND);}
"||"                 {NAMEDOP_RETURN(LE_OROR);}
"*="                 {NAMEDOP_RETURN(LE_MULTassign);}
"/="                 {NAMEDOP_RETURN(LE_DIVassign);}
"%="                 {NAMEDOP_RETURN(LE_MODassign);}
"+="                 {NAMEDOP_RETURN(LE_PLUSassign);}
"-="                 {NAMEDOP_RETURN(LE_MINUSassign);}
"<<="                {NAMEDOP_RETURN(LE_LSassign);}
">>="                {NAMEDOP_RETURN(LE_RSassign);}
"&="                 {NAMEDOP_RETURN(LE_ANDassign);}
"^="                	{NAMEDOP_RETURN(LE_ERassign);}
"|="					{NAMEDOP_RETURN(LE_ORassign);}
"..."					{NAMEDOP_RETURN(LE_ELLIPSIS);}
<<EOF>> 				{	
							//reset lexer
							yyterminate();
						}
.						{return yytext[0];}
<PREPR>\n		{ 
						defineFound = false;
						BEGIN INITIAL;
					}
<PREPR>\\		{
						BEGIN WRAP_PREP;
					}
<PREPR>define	{
						defineFound = true;
					}
<WRAP_PREP>\n	{
						BEGIN PREPR;
					}
<WRAP_PREP>{identifier}   { 
						if(defineFound)
						{
							defineFound = false;
							g_macros[yytext] = true;
						}
					}				
<PREPR>{identifier}       { 
						if(defineFound)
						{
							defineFound = false;
							g_macros[yytext] = true;
						}
					}
<WRAP_PREP>.	{}					
<PREPR>.			{}					
%%

bool isaTYPE(char *string)
{
	return g_symbols.find(string) != g_symbols.end();
}

bool isaMACRO(char *string)
{
	return g_macros.find(string) != g_macros.end();
}

void cl_scope_lex_clean()
{
	yy_flush_buffer(YY_CURRENT_BUFFER); 
	yy_delete_buffer(YY_CURRENT_BUFFER);
	cl_scope_lineno = 1;
	currentScope.clear();
	g_symbols.clear();
	g_macros.clear();
}

/**
 * scope util functions
 */
void printScopeName()
{
	if(currentScope.empty())
	{
		printf("%d: current scope is global scope\n", cl_scope_lineno );
	}
	else
	{
		printf("%d: current scope is %s\n", cl_scope_lineno, getCurrentScope().c_str());
	}
}

void increaseScope()
{
	static int value = 0;
	std::string scopeName("__anon_");
	
	char buf[100];
	sprintf(buf, "%d", value++);
	scopeName += buf;
	currentScope.push_back(scopeName);
}

std::string getCurrentScope()
{
	//format scope name
	std::string scope;
	for(size_t i=0; i<currentScope.size(); i++)
	{
		scope += currentScope[i];
		scope += "::";
	}
	//remove the trailing '::'
	scope.erase(scope.find_last_not_of(":")+1);
	return scope;
}

/*******************************************************************/
bool setLexerInput(const std::string &in) 
{
	yy_scan_string(in.c_str());
	
	//update the working file name
	return true;
}

int yywrap()
{
	return 1;
}