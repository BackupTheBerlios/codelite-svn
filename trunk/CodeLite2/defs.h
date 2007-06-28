#ifndef CL_DEFS_H
#define CL_DEFS_H

#ifdef __APPLE__
    #include <ctype.h>
    #undef _C /* _CTYPE_C */
    #undef _U /* _CTYPE_U */
#endif

#ifdef wxUSE_UNICODE
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

#endif  //CL_DEFS_H
