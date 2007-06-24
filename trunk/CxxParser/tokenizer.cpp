// StringTokenizer.cpp: implementation of the StringTokenizer class.
//
//////////////////////////////////////////////////////////////////////
#include "tokenizer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StringTokenizer::StringTokenizer(const std::string& str,
								 const std::string& strDelimiter,
								 const bool &bAllowEmptyTokens /* false */)
{
	Initialize();

	int nEnd = (int)str.find(strDelimiter, 0);
	int nStart = 0;
	std::string token;
	while( nEnd != -1 )
	{
		if( nEnd != nStart)
			token = str.substr(nStart, nEnd-nStart);
		else
			token.empty();

		if(!token.empty())
			m_tokensArr.push_back(token);
		else if( bAllowEmptyTokens )
			m_tokensArr.push_back(token);

		// next token
		nStart = nEnd + (int)strDelimiter.length();
		nEnd = (int)str.find(strDelimiter, nStart );
	}
	if( nStart != (int)str.length() )
	{
		//We have another token which is not delimited 
		std::string token = str.substr(nStart);
		m_tokensArr.push_back(token);
	}
}

// Default
StringTokenizer::StringTokenizer()
{
	Initialize();
}

// Copy constructor
StringTokenizer::StringTokenizer(const StringTokenizer &src)
{
	*this = src;
}

StringTokenizer& StringTokenizer::operator =(const StringTokenizer &src)
{
	if( &src == this)
		return *this;
	Initialize();

	// Copy the tokens
	m_tokensArr.clear();
	for( int i=0; i<(int)src.m_tokensArr.size() ; i++)
		m_tokensArr.push_back(src.m_tokensArr[i]);
	m_nCurr = src.m_nCurr;
	return *this;
}

StringTokenizer::~StringTokenizer()
{
	m_tokensArr.clear();
	m_nCurr = 0;
}

// Return current token and advance to the next. If there are no more tokens, return empty string.
std::string StringTokenizer::Next()
{
	if( m_nCurr == (int)m_tokensArr.size() )
	{
		// We are at the end of the tokens array
		return "";
	}
	std::string strToken = m_tokensArr[m_nCurr];
	m_nCurr++;
	return strToken;
}

// We return the previous token, if we are already at the start, return empty string.
std::string StringTokenizer::Previous()
{
	if(m_nCurr == 0)
	{
		return "";
	}
	if(m_tokensArr.size() == 0)
	{
		return "";
	}
	m_nCurr--;
	return m_tokensArr[m_nCurr];
}

bool StringTokenizer::HasMore()
{
	if(m_nCurr < (int)m_tokensArr.size())
	{
		return true;
	}
	return false;
}

std::string StringTokenizer::First()
{
	if(m_tokensArr.size()>0)
	{
		m_nCurr = 1;	//The next one
		return m_tokensArr[0];
	}
	else
	{
		return "";
	}
}

// This function is much similar to the 'next()' function with one difference, 
// it does not advance the pointer to the next token.
std::string StringTokenizer::Current()
{
	if( m_nCurr == (int)m_tokensArr.size() )
	{
		// We are at the end of the tokens array
		return "";
	}
	return m_tokensArr[m_nCurr];
}

const int StringTokenizer::Count() const
{
	return (int)m_tokensArr.size();
}

void StringTokenizer::Initialize()
{
	m_tokensArr.clear();
	m_nCurr = 0;
}

// Return last token
std::string StringTokenizer::Last()
{
	if(m_tokensArr.size() == 0)
		return "";
	m_nCurr = (int)m_tokensArr.size()-1;
	return m_tokensArr[m_tokensArr.size()-1];
}

std::string StringTokenizer::operator[](const int nIndex)
{
	if(m_tokensArr.size() == 0)
		return "";
	if( nIndex>=(int)m_tokensArr.size() || nIndex<0)
		return "";
	return m_tokensArr[nIndex];
}


