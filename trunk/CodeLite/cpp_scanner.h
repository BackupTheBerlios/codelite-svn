#ifndef CODELITE_CPPSCANNER_H
#define CODELITE_CPPSCANNER_H

#include "FlexLexer.h"
#include "smart_ptr.h"

namespace codelite
{

class CppScanner : public flex::yyFlexLexer
{
public:
	CppScanner();
	~CppScanner(void);
	
	/// Override the LexerInput function
	int LexerInput(char *buf, int max_size);
	void SetText(const char* data);
	void Reset();

	 
	///	Note about comment and line number:
	///	If the last text consumed is a comment, the line number 
	///	returned is the line number of the last line of the comment
	///	incase the comment spans over number of lines
	///	(i.e. /* ... */ comment style)

	const int& LineNo() const { return yylineno; }
	inline void ClearComment() { m_comment = wxEmptyString; }
	inline const wxChar* GetComment() const { return m_comment.GetData(); }
	inline void KeepComment(const int& keep) { m_keepComments = keep; }
	inline void ReturnWhite(const int& rw) { m_returnWhite = rw; }

private:
	char *m_data;
	char *m_pcurr;
	int   m_total;
	int   m_curr;
};

typedef SmartPtr<CppScanner> CppScannerPtr;
} // codelite

#endif // CODELITE_CPPSCANNER_H
