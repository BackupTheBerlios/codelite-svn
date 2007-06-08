#ifndef REGEX_PROCESSOR_H
#define REGEX_PROCESSOR_H

#include "wx/string.h"

class wxRegEx;

class RegexProcessor {
	wxRegEx *m_re;
public:
	RegexProcessor(const wxString &reStr);
	virtual ~RegexProcessor();
	bool GetGroup(const wxString &str, int grp, wxString &out);
};

#endif // REGEX_PROCESSOR_H