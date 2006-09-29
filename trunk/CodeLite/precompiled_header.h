#ifndef CODELITE_PRECOMPILED_HEADER_H
#define CODELITE_PRECOMPILED_HEADER_H

#include <wx/wxprec.h>

#ifdef __VISUALC__
	#ifdef _DEBUG
		#include <crtdbg.h>
		#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
	#else
		#define DEBUG_NEW new
	#endif
#endif 

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
 
#ifndef WX_PRECOMP
    // Only include the headers we need here
	#include <wx/wx.h>
	#include <wx/process.h>
	#include <wx/filename.h>
	#include <wx/treectrl.h>
#endif

#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <wx/wxsqlite3.h>
#include <wx/string.h>
#include "readtags.h"

namespace codelite
{

/////////////////////////////////////////////////////////////////////////////
// Unicode:
// Use the following macros, in the following cases:
// 
// 1. If your API requires const char*, wrap wxString with _C( ) macro
// 2. To convert char* ( not litteral ) use _U( ) 
// 3. To convert litteral strings, use wxT()
/////////////////////////////////////////////////////////////////////////////

#ifdef __APPLE__
    #include <ctype.h>
    #undef _C /* _CTYPE_C */
    #undef _U /* _CTYPE_U */
#endif

#if wxUSE_UNICODE
    #ifndef _U
        #define _U(x) wxString((x),wxConvUTF8)
    #endif
    #ifndef _UU
        #define _UU(x,y) wxString((x),y)
    #endif
    #ifndef _CC
        #define _CC(x,y) (x).mb_str((y))
    #endif
#else
    #ifndef _U
        #define _U(x) (x)
    #endif
    #ifndef _UU
        #define _UU(x,y) (x)
    #endif
    #ifndef _CC
        #define _CC(x,y) (x)
    #endif
#endif

#ifndef _C
    #define _C(x) _CC((x),wxConvUTF8)
#endif

#ifdef USE_TRACE
    #define DEBUG_MSG(msg) { wxString tmpLogMsgString; tmpLogMsgString << msg; wxLogMessage(tmpLogMsgString); }
#else
    #define DEBUG_MSG(msg) { wxString tmpLogMsgString; tmpLogMsgString << msg; }
#endif

} // codelite

#endif // CODELITE_PRECOMPILED_HEADER_H
