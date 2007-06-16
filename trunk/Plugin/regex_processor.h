#ifndef REGEX_PROCESSOR_H
#define REGEX_PROCESSOR_H

#include "wx/string.h"
#include "regexp.h"
#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK
class WXDLLIMPEXP_LE_SDK RegexProcessor {
	regexp *m_re;
	bool m_isOk;

public:
	RegexProcessor(const wxString &reStr);
	virtual ~RegexProcessor();
	bool GetGroup(const wxString &str, int grp, wxString &out);
};

#endif // REGEX_PROCESSOR_H
