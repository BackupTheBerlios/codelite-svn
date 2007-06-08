#include "regex_processor.h"
#include "macros.h"

RegexProcessor::RegexProcessor(const wxString &reStr)
: m_isOk(true)
{
#if wxUSE_UNICODE
	if(re_comp(&m_re, reStr.ToAscii().data())){
#else
	if(re_comp(&m_re, reStr.ToAscii())){
#endif
		m_isOk = false;
	}
}

RegexProcessor::~RegexProcessor()
{
	re_free(m_re);
}

bool RegexProcessor::GetGroup(const wxString &str, int grp, wxString &out)
{
	if(!m_isOk){
		return false;
	}

	if(grp >= re_nsubexp(m_re)){
		return false;
	}

	regmatch* matches = new regmatch[re_nsubexp(m_re)];
#if wxUSE_UNICODE
	int retval = re_exec(m_re, str.ToAscii().data(), re_nsubexp(m_re), &matches[0]);
#else
	int retval = re_exec(m_re, str.ToAscii(), re_nsubexp(m_re), &matches[0]);
#endif
	if(retval < 1)                                              // line G
    {
        delete[] matches;
        return false;
    }

	printf("matches[grp].begin = %ld\n", matches[grp].begin);
	printf("matches[grp+1].begin = %ld\n", matches[grp+1].begin);

	out = str.Mid(matches[grp].begin, matches[grp+1].begin - matches[grp].begin);
	delete [] matches;
	return true;
}
