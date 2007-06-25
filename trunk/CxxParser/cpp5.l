%{

    /* Copyright (C) 1989-1991 James A. Roskind, All rights reserved.
    This lexer description was written by James A.  Roskind.  Copying
    of  this  file, as a whole, is permitted providing this notice is
    intact  and  applicable   in   all   complete   copies.    Direct
    translations  as a whole to other lexer generator input languages
    (or lexical description languages)  is  permitted  provided  that
    this  notice  is  intact and applicable in all such copies, along
    with a disclaimer that  the  contents  are  a  translation.   The
    reproduction  of derived files or text, such as modified versions
    of this file, or the output of scanner generators, is  permitted,
    provided   the  resulting  work  includes  the  copyright  notice
    "Portions Copyright (c) 1989, 1990 James  A.   Roskind".  Derived
    products  must  also  provide  the notice "Portions Copyright (c)
    1989, 1990 James A.  Roskind" in  a  manner  appropriate  to  the
    utility,   and  in  keeping  with  copyright  law  (e.g.:  EITHER
    displayed when first invoked/executed; OR displayed  continuously
    on  display terminal; OR via placement in the object code in form
    readable in a printout, with or near the title of the work, or at
    the end of the file).  No royalties, licenses or  commissions  of
    any  kind  are  required  to copy this file, its translations, or
    derivative products, when the copies are made in compliance  with
    this  notice.  Persons  or  corporations  that  do make copies in
    compliance  with  this  notice  may  charge  whatever  price   is
    agreeable  to  a buyer, for such copies or derivative works. THIS
    FILE IS PROVIDED ``AS IS'' AND WITHOUT  ANY  EXPRESS  OR  IMPLIED
    WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

    James A. Roskind
    Independent Consultant
    516 Latania Palm Drive
    Indialantic FL, 32903
    (407)729-4348
    jar@hq.ileaf.com
    or ...!uunet!leafusa!jar

    ---end of copyright notice---


COMMENTS-

My  goal  is  to  see  software  developers adopt my C++ grammar as a
standard until such time as a better  standard  is  accessible.   The
only  way  to  get it to become a standard, is to be sure that people
know that derivations are based on a specific work.   The  intent  of
releasing  this Flex input file is to facilitate experimentation with
my C++ grammar. The intent  of  the  copyright  notice  is  to  allow
arbitrary  commercial and non-commercial use of this file, as long as
reference is given to my standardization effort.   Without  reference
to  a specific standard, many alternative grammars would develop.  By
referring to the standard, the C++ grammar is given publicity,  which
should  lead  to further use in compatible products and systems.  The
benefits  of  such  a  standard  to  commercial  products  (browsers,
beautifiers,  translators,  compilers,  ...) should be obvious to the
developers, in that other compatible products will  emerge,  and  the
value  of  all  conforming  products  will rise.  Most developers are
aware of the value of acquiring  a  fairly  complete  grammar  for  a
language,  and  the  copyright  notice (and the resulting affiliation
with my work) should not be too high a price to pay.  By copyrighting
my work, I have some minor control over what this standard is, and  I
can  (hopefully)  keep it from degrading without my approval.  I will
consistently attempt to provide upgraded grammars that are  compliant
with  the  current  art, and the ANSI C++ Committee recommendation in
particular.  A  developer  is  never  prevented  from  modifying  the
grammar or this file to improve it in whatever way is seen fit. There
is  also  no  restriction on the sale of copies, or derivative works,
providing the request in the copyright notice are satisfied.

If you are not "copying" my work, but  are  rather  only  abstracting
some of my work, an acknowledgment with references to such a standard
would  be  appreciated.  Specifically, agreements with my grammar and
its resolution of otherwise ambiguous constructs, should be noted.

Simply put: "make whatever use you would like of the grammar and this
file, but include the ``portions Copyright ...'' as  a  reference  to
this standard."


*/

/* Last modified 7/4/91, Version 2.0 */

/* File cpp5.l, becomes lex.yy.c after processing by FLEX  */

/*This file is a dramatically cut down version of the FLEX input file
used in  my  ANSI  C  Preprocessor.   The  executable  version  of  my
preprocessor  is  available on many platforms (shareware), but this is
the only source extract that is currently being distributed.   If  you
need   a   full   ANSI   C  preprocessor,  with  extensive  diagnostic
capabilities and customization facilities, please contact  me  at  the
addresses given above.  Current platforms include IBMPC (DOS/OS2), Sun
(SPARC   and  Motorola),  and  IBM  R/6000.   ...  end  of  commercial
announcement.

This file is being distributed to facilitate experimentation  and  use
of my C and C++ grammar.


Comment  removal  must  be done during the lexing, as context (such as
enclosure in string literals) must be  observed.   For  this  cut-down
lexer,  we  will  assume that comments have been removed (don't assume
this if you are writing a compiler or browser!).


For each IDENTIFIER like string that is found,  there  are  several
distinct interpretations that can be applied:

1)  The  preprocessor  may  interpret  the  string as a "keyword" in a
directive (eg: "pragma" or "include", "defined").

2) The parser may interpret the string as a keyword. (eg: "int").

3) Both parser and preprocessor may interpret the string as a  keyword
(eg: "if").

Since  this  file  is based on source that actually lexically analyses
text for both preprocessing and parsing, macro definitions  were  used
throughout.   The macro definitions supplied here have been customized
to a C++ parse only, and  all  preprocessor  keywords  are  passed  as
IDENTIFIER  or  TYPEDEFname.   Also, since there is no symbol table to
interrogate to decide whether a string  is  a  TYPEDEFname,  I  simply
assume  that  any  identifier beginning with an upper case letter is a
TYPEDEFname.  This hack  should  allow  you  to  check  out  how  code
segments  are  parsed  using my grammar.  Unfortunately, if you really
want to parse major league code, you have to write a symbol table, and
maintain appropriate scoping information.



*/


/* Included code before lex code */
/*************** Includes and Defines *****************************/
#include "string"
#include "map"
#include "y.tab.h" /* YACC generated definitions based on C++ grammar */
#include "symbol_table.h"

extern std::string yylval;  /*  We  will always point at the text of the lexeme.
//         This makes it easy to print out nice trees when  YYDEBUG  is
//         enabled.   (see  C++  grammar  file  and  its  definition of
//          YYDEBUG_LEXER_TEXT to be "yylval" */

#include <stdlib.h>
#include <string.h>
#include <vector>

std::vector<std::string> currentScope;
std::string g_fileName;

void setFileName(const char *fileName);
std::string &getFileName();
bool setLexerInput(const char *fileName);
std::string getCurrentScope();
void printScopeName();

//TODO:: add a symbol table access in this function
bool isaTYPE(char *string);

/* Prototypes */
#define WHITE_RETURN(x) /* do nothing */

#define PA_KEYWORD_RETURN(x)   RETURN_VAL(x)  /* standard C PArser Keyword */
#define CPP_KEYWORD_RETURN(x)  PA_KEYWORD_RETURN(x)  /* C++ keyword */
#define PPPA_KEYWORD_RETURN(x) RETURN_VAL(x)  /* both PreProcessor and PArser keyword */
#define PP_KEYWORD_RETURN(x)   IDENTIFIER_RETURN()

#define IDENTIFIER_RETURN() RETURN_VAL(isaTYPE(yytext) ? LE_TYPEDEFname: LE_IDENTIFIER )


#define PPOP_RETURN(x)       RETURN_VAL((int)*yytext) /* PreProcess and Parser operator */
#define NAMED_PPOP_RETURN(x) RETURN_VAL(x)
#define ASCIIOP_RETURN(x)    RETURN_VAL((int)*yytext) /* a single character operator */
#define NAMEDOP_RETURN(x)    RETURN_VAL(x)            /* a multichar operator, with a name */

#define NUMERICAL_RETURN(x) RETURN_VAL(x)            /* some sort of constant */
#define LITERAL_RETURN(x)   RETURN_VAL(x)            /* a string literal */
#define C_COMMENT_RETURN(x) RETURN_VAL(x)	     /* C Style comment  */
#define RETURN_VAL(x) {yylval = yytext; return(x);}

#ifdef yywrap
#undef yywrap
#endif

#define yywrap() 1

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
"#"                  {NAMED_PPOP_RETURN('#') ;}
"##"                 {NAMED_PPOP_RETURN(LE_POUNDPOUND);}

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
"^="                 {NAMEDOP_RETURN(LE_ERassign);}
"|="                 {NAMEDOP_RETURN(LE_ORassign);}
"..."                {NAMEDOP_RETURN(LE_ELLIPSIS);}
.	       			   {return yytext[0];}


%%
bool isaTYPE(char *string)
{
	bool res = SymbolTable::instance().IsSymbolExist(string, "");
	return res;
}

/**
 * scope util functions
 */
void printScopeName()
{
	if(currentScope.empty())
	{
		printf("current scope is global scope\n");
	}
	else
	{
		printf("current scope is %s\n", getCurrentScope().c_str());
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

void setFileName(const char *fileName)
{
	g_fileName = fileName;
}

std::string &getFileName()
{
	return g_fileName;
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
bool setLexerInput(const char *fileName) 
{
	FILE *fp;
	long len;
	char *buf = NULL;
	
	fp = fopen(fileName, "rb");
	if(!fp)
	{
		printf("failed to open file '%s': %s\n", fileName, strerror(errno));
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
		printf("failed to read from file '%s': %s\n", fileName, strerror(errno));
		return false;
	}
	
	buf[len] = 0;	// make it null terminated string
	fclose(fp);
	
	yy_scan_string(buf);
	free(buf);
	
	//update the working file name
	setFileName(fileName);
	return true;
}
