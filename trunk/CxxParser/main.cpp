//test the parser
#include "stdio.h"
#include "errno.h"
#include "string.h"
#include "string"
#include "variable.h"
#include "function.h"
#include "expression_result.h"

#include <windows.h>

extern std::string get_scope_name(const std::string &in, std::string &lastFuncName, std::string &lastFuncSignature);
extern void get_variables(const std::string &in, VariableList &li);
extern ExpressionResult &parse_expression(const std::string &in);
extern void get_functions(const std::string &in, FunctionList &li);

void testScopeParser(char *buf);
void testVarParser(char *buf);
void testExprParser(char *buf);
void testFuncParser(char *buf);
char *loadFile(const char *fileName);

int main()
{
	char *buf = loadFile("test.h");
	//print the scope name
	testScopeParser(buf);
	//testVarParser(buf);
	//testExprParser(buf);
	//testFuncParser(buf);
	free(buf);
}

void testFuncParser(char *buf)
{
	printf("===== Testing function parser ======\n");
	time_t start = GetTickCount();
	FunctionList li;
	fflush(stdout);
	get_functions(buf, li);
	time_t end = GetTickCount();
	for(FunctionList::iterator iter = li.begin(); iter != li.end(); iter++)
	{
		//test the var parser on the function argument list:
		Function f = (*iter);
		f.Print();
		testVarParser((char*)f.m_signature.c_str());
	}
	
	printf("total time: %d\n", end-start);
	printf("matches found: %d\n", li.size());
}

void testExprParser(char *buf)
{
	printf("===== Testing expression parser ======\n");
	ExpressionResult res = parse_expression(buf);
	res.Print();
}


void testScopeParser(char *buf)
{
	std::string lastFuncName, lastFuncSig;
	printf("===== Testing Scope parser ======\n");
	time_t start = GetTickCount();
	std::string scope = get_scope_name(buf, lastFuncName, lastFuncSig);
	time_t end = GetTickCount();
	printf("total time: %d\n", end-start);
	printf("scope name=%s\n", scope.c_str());
	printf("Last function seen:%s signature:%s\n", lastFuncName.c_str(), lastFuncSig.c_str());
}

void testVarParser(char *buf)
{
	printf("===== Testing Variable parser ======\n");
	time_t start = GetTickCount();
	VariableList li;
	fflush(stdout);
	get_variables(buf, li);
	time_t end = GetTickCount();
	for(VariableList::iterator iter = li.begin(); iter != li.end(); iter++)
	{
		(*iter).Print();
	}
	printf("total time: %d\n", end-start);
	printf("matches found: %d\n", li.size());
}

//-------------------------------------------------------
// Help function
//-------------------------------------------------------
char *loadFile(const char *fileName)
{
	FILE *fp;
	long len;
	char *buf = NULL;
	
	fp = fopen(fileName, "rb");
	if(!fp)
	{
		printf("failed to open file 'test.h': %s\n", strerror(errno));
		return NULL;
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
		return NULL;
	}
	
	buf[len] = 0;	// make it null terminated string
	fclose(fp);
	return buf;
}
