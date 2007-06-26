#include "codelite.h"

CodeLite::CodeLite()
{
}

CodeLite::~CodeLite()
{
}

CodeLite *CodeLite::Get()
{
	static CodeLite theInstnace;
	return &theInstnace;
}
