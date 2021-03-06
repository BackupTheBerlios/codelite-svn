%{
// Copyright Eran Ifrah(c)
%}

%{
/*************** Includes and Defines *****************************/
#include "string"
#include "vector"
#include "stdio.h"
#include "map"
#include "expression_result.h"

#define YYSTYPE std::string
#define YYDEBUG 0        /* get the pretty debugging code to compile*/

void cl_expr_error(char *string);

static ExpressionResult result;

//---------------------------------------------
// externs defined in the lexer
//---------------------------------------------
extern char *cl_expr_text;
extern int cl_expr_lex();
extern int cl_expr_parse();
extern int cl_expr_lineno;
extern std::vector<std::string> currentScope;
extern bool setExprLexerInput(const std::string &in);
extern void cl_expr_lex_clean();

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
%token  LE_MACRO 
%token  LE_DYNAMIC_CAST
%token  LE_STATIC_CAST
%token  LE_CONST_CAST
%token  LE_REINTERPRET_CAST

%start   translation_unit

%%
/* ========================================================================*/
/* find declarations																   */
/* ========================================================================*/

translation_unit	:		/*empty*/
						| translation_unit primary_expr
						;
						
primary_expr		:	{result.Reset();} simple_expr	
						| 	error { 
								yyclearin;	//clear lookahead token
								yyerrok;
								//fprintf(stderr, "CodeLite: syntax error, unexpected token '%s' found at line %d \n", cl_expr_text, cl_expr_lineno);
								fflush(stderr);
								expr_syncParser();
							}
						;

simple_expr	:	stmnt_starter special_cast '<' cast_type '>' '(' 
					{
						expr_FuncArgList(); 
						$$ = $4;
						result.m_isaType = true;
						result.m_name = $4;
						result.m_isFunc = false;
						//result.Print();
					}
				| 	stmnt_starter LE_THIS 
					{
						$$ = $2;
						result.m_isaType = false;
						result.m_name = $$;
						result.m_isFunc = false;
						result.m_isThis = true;
						result.m_isPtr = true;
						//result.Print();
					}
				| 	stmnt_starter '*' LE_THIS 
					{
						$$ = $3;
						result.m_isaType = false;
						result.m_name = $$;
						result.m_isFunc = false;
						result.m_isThis = true;
						//result.Print();
					}
				| 	stmnt_starter '*' LE_IDENTIFIER 
					{
						$$ = $3;
						result.m_isaType = false;
						result.m_name = $$;
						result.m_isFunc = false;
						result.m_isThis = false;
						result.m_isPtr = false;
						//result.Print();
					}
				| 	stmnt_starter '(' cast_type ')' special_star_amp LE_IDENTIFIER 
					{
						$$ = $3;
						result.m_isaType = true;
						result.m_name = $$;
						result.m_isFunc = false;
						result.m_isThis = false;
						//result.Print();
					}
				| 	stmnt_starter '(' '(' cast_type ')' special_star_amp LE_IDENTIFIER ')'
					{
						$$ = $4;
						result.m_isaType = true;
						result.m_name = $$;
						result.m_isFunc = false;
						result.m_isThis = false;
						//result.Print();
					}
				| 	stmnt_starter nested_scope_specifier LE_IDENTIFIER optinal_postifx 
					{
						result.m_isaType = false;
						result.m_name = $3;
						result.m_isThis = false;
						$2.erase($2.find_last_not_of(":")+1);
						result.m_scope = $2;
						//result.Print();
					}
				;
				
optinal_postifx: 	/*empty*/ {$$ = "";}
					|	'(' 
					{
						$$ = $1; 
						expr_FuncArgList();
						result.m_isFunc = true;
					}
					|  '<' 
					{
						$$ = $1;
						expr_consumeTemplateDecl(); 
						result.m_isFunc = false;
					}
					;
					
special_cast 	: 	LE_DYNAMIC_CAST {$$ = $1;}
					|	LE_STATIC_CAST {$$ = $1;}
					|	LE_CONST_CAST {$$ = $1;}
					|	LE_REINTERPRET_CAST {$$ = $1;}
					;

amp_item				:	/*empty*/	{$$ = ""; }
						|   '&' 			{ $$ = $1; }
						;
						
star_list			: 	/*empty*/		{$$ = ""; }
						|	star_list '*'	{$$ = $1 + $2;}
						;

special_star_amp	:	star_list amp_item { $$ = $1 + $2; }
						;

stmnt_starter		:	/*empty*/ {$$ = "";}
						| ';' { $$ = ";";}
						| ':' { $$ = ":";}	//e.g. private: std::string m_name;
						;

cast_type: nested_scope_specifier LE_IDENTIFIER '<' {expr_consumeTemplateDecl();} special_star_amp
				{
					$$ = $1 + $2; 
					$1.erase($1.find_last_not_of(":")+1);
					result.m_scope = $1; 
					result.m_name = $2;
					result.m_isPtr = ($5.find("*") != (size_t)-1);;
				}
			| nested_scope_specifier LE_IDENTIFIER special_star_amp
				{
					$$ = $1 + $2; 
					$1.erase($1.find_last_not_of(":")+1);
					result.m_scope = $1; 
					result.m_name = $2;
					result.m_isPtr = ($3.find("*") != (size_t)-1);;
				}
			;

nested_scope_specifier	: /*empty*/ {$$ = "";}
								| nested_scope_specifier scope_specifier {	$$ = $1 + $2;}
								;

scope_specifier	:	LE_IDENTIFIER LE_CLCL {$$ = $1+ $2;}
						;
						
%%
void yyerror(char *s) {}


void expr_FuncArgList()
{
	int depth = 1;
	while(depth > 0)
	{
		int ch = cl_expr_lex();
		//printf("ch=%d\n", ch);
		fflush(stdout);
		if(ch ==0){
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

void expr_consumeTemplateDecl()
{
	int depth = 1;
	while(depth > 0)
	{
		int ch = cl_expr_lex();
		//printf("ch=%d\n", ch);
		fflush(stdout);
		if(ch ==0){
			break;
		}
		
		if(ch == '>')
		{
			depth--;
			continue;
		}
		else if(ch == '<')
		{
			depth ++ ;
			continue;
		}
	}
}

void expr_syncParser(){
	//dont do anything, a hook to allow us to implement some
	//nice error recovery if needed
}

// return the scope name at the end of the input string
ExpressionResult &parse_expression(const std::string &in)
{
	result.Reset();
	//provide the lexer with new input
	if( !setExprLexerInput(in) ){
		return result;
	}
	
	//printf("parsing...\n");
	cl_expr_parse();
	//do the lexer cleanup
	cl_expr_lex_clean();
	
	return result;
}
