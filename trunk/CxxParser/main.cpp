//test the parser
#include "stdio.h"
#include "errno.h"
#include "string.h"
#include "string"
#include "variable.h"
#include <windows.h>

extern std::string get_scope_name(const std::string &in, bool onlyNamedScope);
extern void get_variables(const std::string &in, VariableList &li);

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
	
	//print the scope name
	
	time_t start = GetTickCount();
	std::string scope = get_scope_name(buf, true);
	time_t end = GetTickCount();
	
	//print all members found in the file
	//time_t start = GetTickCount();
	VariableList li;
	//printf("===== Testing Variable parser ======\n");
	//fflush(stdout);
	//get_variables(buf, li);
	//time_t end = GetTickCount();
	printf("total time: %d\n", end-start);
	free(buf);
}
