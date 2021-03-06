%{
// Copyright Eran Ifrah(c)
%}

%{
/*************** Includes and Defines *****************************/
#include "string"
#include "vector"
#include "stdio.h"
#include "map"
#include "variable.h"

#ifdef yylex
#undef yylex
#define yylex cl_scope_lex
#endif 

#define YYSTYPE std::string
#define YYDEBUG 0        /* get the pretty debugging code to compile*/

void cl_scope_error(char *string);
int  cl_var_parse();
void syncParser();

static VariableList *gs_vars = NULL;
static std::vector<std::string> gs_names;

Variable curr_var;

//---------------------------------------------
// externs defined in the lexer
//---------------------------------------------
extern char *cl_scope_text;
extern int cl_scope_lex();
extern int cl_scope_lineno;
extern std::vector<std::string> currentScope;
extern bool setLexerInput(const std::string &in);
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
%token  LE_MACRO 
%token  LE_DYNAMIC_CAST
%token  LE_STATIC_CAST
%token  LE_CONST_CAST
%token  LE_REINTERPRET_CAST

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
						
external_decl		:	{curr_var.Reset(); gs_names.clear();} variables	
						| 	error { 
								yyclearin;	//clear lookahead token
								yyerrok;
								//printf("CodeLite: syntax error, unexpected token '%s' found at line %d \n", cl_scope_text, cl_scope_lineno);
								var_syncParser();
							}
						;
						
/* the following rules are for template parameters no declarations! */
parameter_list	: /* empty */		{$$ = "";}
							| template_parameter	{$$ = $1;}
							| parameter_list ',' template_parameter {$$ = $1 + $2 + $3;}
							;

template_parameter	:	const_spec nested_scope_specifier LE_IDENTIFIER special_star_amp 
								{$$ = $1 + " " + $2 + " " + $3 +$4;}
							|  const_spec nested_scope_specifier basic_type_name special_star_amp 
								{$$ = $1 + " " + $2 + " " + $3 +$4;}
							;
							
//the main rule for finding variables
//in the code. if this rule succeeded, the variables 
//is added to the gs_vars vriable
variables			: stmnt_starter variable_decl special_star_amp variable_name_list postfix 
						{
							if(gs_vars)
							{
								Variable var;
								std::string pattern;
								curr_var.m_pattern = "/^";
								curr_var.m_pattern += $1 + " " + $2 + " " + $3 + " " + $4 + "$/";
								curr_var.m_isPtr = ($3.find("*") != (size_t)-1);
								curr_var.m_starAmp = $3;
								for(size_t i=0; i< gs_names.size(); i++)
								{
									//create new variable for every variable name found
									var = curr_var;
									var.m_name = gs_names.at(i);
									gs_vars->push_back(var); 
								}
								curr_var.Reset();
								gs_names.clear();
							}
						}
						;
						
variable_name_list: LE_IDENTIFIER {gs_names.push_back($1);}
						/*| variable_name_list ',' LE_IDENTIFIER 
						{ 
							//collect all the names
							gs_names.push_back($3);
							$$ = $1 + $2 + " " + $3;
						}*/
						;
postfix: ';'
		 | '='
		 | ')'
		 | ','  
		 | '(' { $$ = $1 + var_consumeFuncArgList();} 
		 ; 
/* 
applicable for C++, for cases where a function is declared as
void scope::foo(){ ... }
*/
scope_specifier	:	LE_IDENTIFIER LE_CLCL {$$ = $1+ $2;}
						|	LE_IDENTIFIER  '<' {var_consumeTemplateDecl();} LE_CLCL {$$ = $1 + $4;}
						;

nested_scope_specifier: /*empty*/ {$$ = "";}
							| nested_scope_specifier scope_specifier {	$$ = $1 + $2;}
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

special_star_amp	:	star_list amp_item { $$ = $1 + $2; }
						;

stmnt_starter		:	/*empty*/ {$$ = "";}
						| ';' { $$ = ";";}
						| '{' { $$ = "{";}
						| '(' { $$ = "(";}
						| '}' { $$ = "}";}
						| ':' { $$ = ":";}	//e.g. private: std::string m_name;
						| '=' { $$ = "=";}
						;
						
/** Variables **/
variable_decl		:	const_spec basic_type_name   
						{
							$$ = $1 + " " + $2;
							$2.erase($2.find_last_not_of(":")+1);
							curr_var.m_type = $2;
						}
						|	const_spec nested_scope_specifier LE_IDENTIFIER 
						{
							$$ = $1 + " " + $2 + $3;
							$2.erase($2.find_last_not_of(":")+1);
							curr_var.m_typeScope = $2;
							curr_var.m_type = $3;
						}
						| 	const_spec nested_scope_specifier LE_IDENTIFIER '<' parameter_list '>'  
						{
							$$ = $1 + " " + $2 + $3 + " " + $4 + " " + $5 + " " + $6;
							$2.erase($2.find_last_not_of(":")+1);
							curr_var.m_typeScope = $2;
							curr_var.m_type = $3;
							curr_var.m_isTemplate = true;
							curr_var.m_templateDecl = $4 + $5 + $6;
						}
						;
						
%%
void yyerror(char *s) {}


std::string var_consumeFuncArgList()
{
	std::string consumedData;
	int depth = 1;
	while(depth > 0)
	{
		int ch = cl_scope_lex();
		//printf("ch=%d\n", ch);
		fflush(stdout);
		if(ch ==0){
			break;
		}
		
		consumedData += cl_scope_text;
		consumedData += " ";
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
	return consumedData;
}

void var_consumeTemplateDecl()
{
	int depth = 1;
	while(depth > 0)
	{
		int ch = cl_scope_lex();
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


void var_syncParser(){
	//dont do anything, a hook to allow us to implement some
	//nice error recovery if needed
}

// return the scope name at the end of the input string
void get_variables(const std::string &in, VariableList &li)
{
	//provide the lexer with new input
	if( !setLexerInput(in) )
	{
		return;
	}
	
	//set the parser local output to our variable list 
	gs_vars = &li;
	
	//call tghe main parsing routine
	cl_var_parse();
	gs_vars = NULL;
	
	//do the lexer cleanup
	cl_scope_lex_clean();
}
