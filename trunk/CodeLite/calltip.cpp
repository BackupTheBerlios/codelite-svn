#include "calltip.h"

#ifdef __VISUALC__
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif 

CallTip::CallTip(const std::vector<wxString> &tips)
: m_tips(tips)
, m_curr(0)
{
}

CallTip::CallTip(const CallTip& rhs)
{
	*this = rhs;
}

CallTip& CallTip::operator =(const CallTip& rhs)
{
	if(this == &rhs)
		return *this;
	m_tips = rhs.m_tips;
	return *this;
}

wxString CallTip::Next() 
{
	// format a tip string and return it
	wxString tip;
	if( m_tips.empty() )
		return wxEmptyString;

	if( m_curr >= m_tips.size() )
		m_curr = 0;

	if( m_tips.size() > 1 )
		tip << _T("\n\001 ") << static_cast<int>(m_curr)+1 << _T(" of ") << static_cast<int>(m_tips.size()) << _T(" \002 ")
			<< m_tips.at( m_curr ) << _T("\n");
	else
		tip << _T("\n") << m_tips.at( m_curr ) << _T("\n");
	m_curr++;
	return tip;
}

wxString CallTip::Prev() 
{
	// format a tip string and return it
	wxString tip;
	if( m_tips.empty() )
		return wxEmptyString;

	if( m_tips.size() > 1 )
		tip << _T("\n\001 ") << static_cast<int>(m_curr)+1 << _T(" of ") << static_cast<int>(m_tips.size()) << _T(" \002 ")
		<< m_tips.at( m_curr ) << _T("\n");
	else
		tip << _T("\n") << m_tips.at( m_curr ) << _T("\n");

	if( m_curr == 0 )
		m_curr = m_tips.size() - 1;
	else
		m_curr--;
	return tip;
}

std::vector<wxString>::size_type CallTip::Count() const
{
	return m_tips.size();
}
