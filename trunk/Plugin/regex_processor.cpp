#include "regex_processor.h"
#include "macros.h"

#if wxUSE_UNICODE
    #ifndef _U
        #define _U(x) wxString((x),wxConvUTF8)
    #endif
#else
    #ifndef _U
        #define _U(x) (x)
    #endif
#endif

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
	const wxCharBuffer pline = str.mb_str();

	int retval = re_exec(m_re, pline.data(), re_nsubexp(m_re), &matches[0]);
	if(retval < 1){
        delete[] matches;
        return false;
    }
	
	std::string out_s = std::string(pline.data() + matches[grp].begin, pline.data()+matches[grp].end);
	out = _U(out_s.c_str());

	printf("out=%s\n", out_s.c_str());
	delete [] matches;
	return true;
}
