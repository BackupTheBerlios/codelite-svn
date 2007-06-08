#ifndef REGEX_PROCESSOR_H
#define REGEX_PROCESSOR_H

#include "wx/string.h"
#include "regexp.h"

class RegexProcessor {
	regexp *m_re;
	bool m_isOk;

public:
	RegexProcessor(const wxString &reStr);
	virtual ~RegexProcessor();
	bool GetGroup(const wxString &str, int grp, wxString &out);
};

#endif // REGEX_PROCESSOR_H
