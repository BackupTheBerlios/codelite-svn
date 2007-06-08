#include "regex_processor.h"
#include "macros.h"

RegexProcessor::RegexProcessor(const wxString &reStr)
: m_isOk(true)
{
	if(re_comp(&m_re, reStr.ToAscii().data())){
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
	int retval = re_exec(m_re, str.ToAscii().data(), re_nsubexp(m_re), &matches[0]);
	if(retval < 1)                                              // line G
    {
        delete[] matches;
        return false;
    }
	out = str.Mid(matches[grp].begin, matches[grp].end - matches[grp].begin);
	delete [] matches;
	return true;
}
