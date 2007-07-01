#include "stdio.h"
#include "stdlib.h"
#define ENABLE 1

template <class T, class B> struct TagEntry : public Singleton<TagEntry>, private wxEvtHandler
{
	union Eran {
		int main;
		char *name;
	}
	m