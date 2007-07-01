%{
// Copyright Eran Ifrah(c)
%}

%{
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

//---------------------------------------------
// externs defined in the lexer
//---------------------------------------------
extern char *cl_scope_text;
extern int cl_scope_lex();
extern bool setLexerInput(const std::string &in);
extern int cl_scope_lineno;
extern std::vector<std::string> currentScope;
//extern std::map<std::string, bool> g_symbols;
extern void printScopeName();	//print the current scope name
extern void increaseScope();	//increase scope with anonymouse value
extern std::string getCurrentScope();
extern void cl_scope_lex_clean();

/*************** Standard ytab.c continues here *********************/
%}

/*************************************************************************/

/* This group is used by the C/C++ language parser */
%token  LE_AUTO            LE_DOUBLE          LE_INT             LE_STRUCT
%token  LE_BREAK           LE_ELSE            LE_LONG            LE_SWITCH
%token  LE_CASE            LE_ENUM            LE_REGISTER        LE_TYPEDEF
%token  LE_CHAR            LE_EXTERN          LE_RETURN          LE_UNION
%token  LE_CONST           LE_FLOAT           LE_SHORT           LE_UNSIGNED
%token  LE_CONTINUE        LE_FOR             LE_SIGNED          LE_VOID
%token  LE_DEFAULT         LE_GOTO            LE_SIZEOF          LE_VOLATILE
%token  LE_DO              LE_IF              LE_STATIC          LE_WHILE

/* The following are used in C++ only.  ANSI C would call these IDENTIFIERs */
%token  LE_NEW             LE_DELETE
%token  LE_THIS
%token  LE_OPERATOR
%token  LE_CLASS
%token  LE_PUBLIC          LE_PROTECTED       LE_PRIVATE
%token  LE_VIRTUAL         LE_FRIEND
%token  LE_INLINE          LE_OVERLOAD
%token  LE_TEMPLATE		  LE_TYPENAME

/* ANSI C Grammar suggestions */
%token  LE_IDENTIFIER              LE_STRINGliteral
%token  LE_FLOATINGconstant        LE_INTEGERconstant        LE_CHARACTERconstant
%token  LE_OCTALconstant           LE_HEXconstant
%token  LE_POUNDPOUND LE_CComment LE_CPPComment LE_NAMESPACE

/* New Lexical element, whereas ANSI C suggested non-terminal */
%token  LE_TYPEDEFname

/* Multi-Character operators */
%token   LE_ARROW            											/*    ->                              */
%token   LE_ICR LE_DECR         										/*    ++      --                      */
%token   LE_LS LE_RS            										/*    <<      >>                      */
%token   LE_LE LE_GE LE_EQ LE_NE      								/*    <=      >=      ==      !=      */
%token   LE_ANDAND LE_OROR      										/*    &&      ||                      */
%token   LE_ELLIPSIS         											/*    ...                             */
			/* Following are used in C++, not ANSI C        */
%token   LE_CLCL             											/*    ::                              */
%token   LE_DOTstar LE_ARROWstar										/*    .*       ->*                    */

/* modifying assignment operators */
%token  LE_MULTassign  LE_DIVassign    LE_MODassign   	/*   *=      /=      %=      */
%token  LE_PLUSassign  LE_MINUSassign              		/*   +=      -=              */
%token  LE_LSassign    LE_RSassign                 		/*   <<=     >>=             */
%token  LE_ANDassign   LE_ERassign     LE_ORassign    	/*   &=      ^=      |=      */

%start   translation_unit

%%
/* Costants */
basic_type_name:
        LE_INT				{ $$ = $1; }
        | LE_CHAR			{ $$ = $1; }
        | LE_SHORT		{ $$ = $1; }
        | LE_LONG			{ $$ = $1; }
        | LE_FLOAT		{ $$ = $1; }
        | LE_DOUBLE		{ $$ = $1; }
        | LE_SIGNED		{ $$ = $1; }
        | LE_UNSIGNED	{ $$ = $1; }
        | LE_VOID			{ $$ = $1; }
        ;
	

/* ========================================================================*/
/* find declarations																   */
/* ========================================================================*/

translation_unit	:		/*empty*/
						| translation_unit external_decl
						;
						
external_decl			:	class_decl	
						|	enum_decl
						|	union_decl
						| 	function_decl	/* function decl also includes variables */
						|	namespace_decl
						| 	scope_reducer
						| 	scope_increaer
						| 	error { 
								printf("CodeLite: syntax error, unexpected token '%s' found at line %d \n", cl_scope_text, cl_scope_lineno);
								syncParser();
							}
						;
/*templates*/
template_arg		:	/* empty */	{ $$ = "";}
						| template_specifiter LE_IDENTIFIER {$$ = $1 + " " + $2;}
						;
						
template_arg_list	:	template_arg	{ $$ = $1; }
						| 	template_arg_list ',' template_arg	{ $$ = $1 + " " + $2 + " " + $3; }
						;

template_specifiter	:	LE_CLASS	{ $$ = $1; }
							|	LE_TYPENAME	{ $$ = $1; }
							;

opt_class_qualifier	:	/* empty */			{ $$ = "";}
							//|	LE_IDENTIFIER	{ $$ = $1;}
							;
							
opt_template_qualifier	: /*empty*/
							| LE_TEMPLATE '<' template_arg_list '>'	{ $$ = $1 + $2 + $3 + $4;}
							;
/*inheritance*/
derivation_list			:	/*empty*/ {$$ = "";}
							|	parent_class {$$ = $1;}
							| 	derivation_list ',' parent_class {$$ = $1 + $2 + $3;}
							;

parent_class				: 	access_specifier LE_IDENTIFIER	opt_template_specifier {$$ = $1 + " " + $2 + $3;}
							;
							
opt_template_specifier	: /*empty*/	{$$ = "";}
							| '<' template_parameter_list '>' {$$ = $1 + $2 + $3;}
							;
							
access_specifier			:	/*empty*/	{$$ = "";}
							|	LE_PUBLIC {$$ = $1;}
							| 	LE_PRIVATE {$$ = $1;}
							| 	LE_PROTECTED {$$ = $1;}
							;
							
/* the following rules are for template parameters no declarations! */
template_parameter_list	: /* empty */		{$$ = "";}
							| template_parameter	{$$ = $1;}
							| template_parameter_list ',' template_parameter {$$ = $1 + $2 + $3;}
							;

template_parameter		:	const_spec nested_scope_specifier LE_IDENTIFIER special_star_amp {$$ = $1 + $2 + $3 +$4;}
							;
/* namespace */
namespace_decl	:	stmnt_starter LE_NAMESPACE LE_IDENTIFIER '{'		
						{
							//g_symbols[$3] = true;
							currentScope.push_back($3);
						}
					|	stmnt_starter LE_NAMESPACE '{'		
						{
							//anonymouse namespace
							printf("found anonymous namespace\n");
							increaseScope();
							printScopeName();
						}
					;
				
/* the class rule itself */
class_decl	:	stmnt_starter opt_template_qualifier class_keyword opt_class_qualifier LE_IDENTIFIER ';' 
				{
					printf("Found class decl: %s\n", $5.c_str());
					//g_symbols[$5] = true;
				}

				| 	stmnt_starter opt_template_qualifier class_keyword opt_class_qualifier LE_IDENTIFIER '{' 
				{
					printf("Found class impl: %s\n", $5.c_str());
					//add new symbol to symbol table
					//g_symbols[$5] = true;
					
					//increase the scope level
					currentScope.push_back($5);
					printScopeName();
				}
				
				| 	stmnt_starter opt_template_qualifier class_keyword opt_class_qualifier LE_IDENTIFIER ':' derivation_list '{' 
				{
					printf("Found class impl: %s, derivation list: %s\n", $5.c_str(), $7.c_str());
					//add new symbol to symbol table
					//g_symbols[$5] = true;
					
					//increase the scope level
					currentScope.push_back($5);
					printScopeName();
				}
				;

scope_reducer		:	'}'	{
								if(currentScope.empty())
								{
									//fatal error!
									printf("CodeLite: fatal error - cant go beyond global scope!\n");
								}
								else
								{
									currentScope.pop_back();
									printScopeName();
								}
							}
					;
scope_increaer	:	'{' {
								//increase random scope
								increaseScope();
								printScopeName();
							 }

class_keyword: 	LE_CLASS		{$$ = $1;}
					|	LE_STRUCT	{$$ = $1;}
					;
					
/* functions */
function_decl		:	stmnt_starter virtual_spec variable_decl nested_scope_specifier LE_IDENTIFIER '(' variable_list ')' const_spec pure_virtual_spec ';'  					 
						{
							printf("Found function: %s\n", $5.c_str());
						}
						
					|	stmnt_starter virtual_spec variable_decl nested_scope_specifier LE_IDENTIFIER '(' variable_list ')' const_spec  '{'  					 
						{
							printf("Found function: %s\n", $5.c_str());
							currentScope.push_back($5);
							printScopeName();
						}
					;

/* 
applicable for C++, for cases where a function is declared as
void scope::foo(){ ... }
*/
nested_scope_specifier		: /*empty*/ {$$ = "";}
								| nested_scope_specifier scope_specifier {$$ = $1 + " " + $2;}
								;

scope_specifier			:	LE_IDENTIFIER LE_CLCL {$$ = $1 + $2;}
							;

pure_virtual_spec	:	/*empty*/				{$$ = ""; }
						| '=' LE_OCTALconstant	{ $$ = $1 + " " + $2; }
						;
						
virtual_spec		:	/* empty */	{$$ = ""; }
						| 	LE_VIRTUAL 	{ $$ = $1; }
						;
						
const_spec			:	/* empty */	{$$ = ""; }
						| 	LE_CONST 	{ $$ = $1; }
						;
						
amp_item				:	/*empty*/	{$$ = ""; }
						|   '&' 			{ $$ = $1; }
						;
						
star_list			: 	/*empty*/		{$$ = ""; }
						|	star_list '*'	{$$ = $1 + $2;}
						;

special_star_amp		:	star_list amp_item { $$ = $1 + $2; }
						;

stmnt_starter			:	/*empty*/ {$$ = "";}
						| ';' {$$ = ";";}
						;
						
/** Variables **/
variable_decl			:	const_spec nested_scope_specifier basic_type_name special_star_amp  
							{$$ = $1 + $2 + $3  + $4;}
						|	const_spec nested_scope_specifier LE_IDENTIFIER special_star_amp
							{$$ = $1 + $2 + $3  + $4;}
						| 	const_spec nested_scope_specifier LE_IDENTIFIER '<' template_parameter_list '>' special_star_amp 
							{$$ = $1 + $2 + $3  + $4 + $5 + $6 + $7;}
						;
						
variable_name			: /*empty*/	{$$ = "";}
						| LE_IDENTIFIER {$$ = $1;}
						;

variable_arg			:	variable_decl variable_name{$$ = $1 + $2;}
						;

variable_list			:	/*empty*/	{$$ = "";}
						|	variable_arg {$$ = $1;}
						|	variable_list ',' variable_arg {$$ = $1 + $2 + $3;}
						;

enum_decl				:	stmnt_starter LE_ENUM LE_IDENTIFIER '{' {currentScope.push_back($3); printScopeName();} enum_arg_list '}' 
						{	
							currentScope.pop_back();//reduce the scope
							printScopeName();
							printf("found enum: %s, args are: %s\n", $2.c_str(), $5.c_str());
						}
						;

enum_optional_assign	:	/*empty*/ {$$ = "";}
						|	'=' LE_HEXconstant	{$$ = $1 + $2;}
						|	'='	 LE_OCTALconstant {$$ = $1 + $2;}
						|	'='	 LE_INTEGERconstant {$$ = $1 + $2;}
						;
						
enum_argument			:	LE_IDENTIFIER	enum_optional_assign {$$ = $1 + $2;}
						;
enum_arg_list			:	/*empty*/ {$$ = "";}
						|	enum_argument	{$$ = $1;}
						|	enum_arg_list ',' enum_argument {$$ = $1 + $2 + $3;}
						;

union_decl			:	stmnt_starter LE_UNION LE_IDENTIFIER '{' 
							{	
								currentScope.push_back($3);
								printScopeName();
								consumeDecl();
								printScopeName();
							}
						;
%%
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
