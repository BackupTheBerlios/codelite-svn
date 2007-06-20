#ifndef MAKEFILEPARSER_H_INCLUDED
#define MAKEFILEPARSER_H_INCLUDED

#include "LineTypes.h"

#include "wx/string.h"
#include "wx/arrstr.h"

class MakefileParser
{
	public:
		MakefileParser(wxArrayString lines);
		TypedStrings getResult();
	
	private:
		void Lex();
		LINETYPES::LineType Deduct();
		
		wxArrayString m_lines;
		TypedStrings m_result;
		
		size_t m_current;
		bool m_continued;
};

#endif // MAKEFILEPARSER_H_INCLUDED
