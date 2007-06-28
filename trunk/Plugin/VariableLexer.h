#ifndef VARIABLELEXER_H_INCLUDED
#define VARIABLELEXER_H_INCLUDED

#include "wx/string.h"
#include "wx/arrstr.h"

#include <map>
//#include <pair>

#ifndef WXDLLIMPEXP_LE_SDK
#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK
#endif // WXDLLIMPEXP_LE_SDK

typedef std::map<wxString, wxString> Tokens;
typedef std::pair<wxString, wxString> Token;
typedef Tokens::iterator TokensI;

class WXDLLIMPEXP_LE_SDK VariableLexer
{
	public:
		VariableLexer(wxArrayString lines);
		wxArrayString getResult();
		void DebugMessage(const wxString& msg);
	
	private:
		void Control(); 
		bool ReadVariables();
		bool ExpandVariables();
		
		wxArrayString m_lines;
		wxArrayString m_result;
		size_t m_current;
		Tokens m_tokens;
};

#endif // VARIABLELEXER_H_INCLUDED
