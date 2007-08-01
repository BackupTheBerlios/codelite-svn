#ifndef CODELITE_CALLTIP_H
#define CODELITE_CALLTIP_H

#include "tokenizer.h"
#include "smart_ptr.h"
 
#ifdef WXMAKINGDLL_CODELITE
#    define WXDLLIMPEXP_CL WXEXPORT
#elif defined(WXUSINGDLL_CODELITE)
#    define WXDLLIMPEXP_CL WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_CL
#endif // WXMAKINGDLL_CODELITE

/**
 * A call tip function that wraps a tip strings for function prototypes.
 *
 * \ingroup CodeLite
 * \version 1.0
 * first version
 *
 * \date 09-18-2006
 * \author Eran
 */
class WXDLLIMPEXP_CL clCallTip
{
	std::vector<wxString> m_tips;
	int m_curr;

public:
	/**
	 * Constructor
	 * \param tips input tips 
	 */
	clCallTip(const std::vector<wxString> & tips );

	/**
	 * default constructor
	 */
	clCallTip() {}

	/**
	 * Copy constructor
	 */
	clCallTip(const clCallTip& rhs);

	/**
	 * Assignment operator
	 * \param rhs right hand side
	 * \return this
	 */
	clCallTip& operator=(const clCallTip& rhs);

	/**
	 * Destructor
	 * \return 
	 */
	virtual ~clCallTip(){}

	/**
	 * Show next tip, if we are at last tip, return the first tip or empty string if no tips exists
	 * \return next tip
	 */
	wxString Next() ;

	/**
	 * Show previous tip, if we are at first tip, return the last tip or empty string if no tips exists
	 * \return previous tip
	 */
	wxString Prev() ;

	/**
	 * return the first tip
	 */
	wxString First();

	/**
	 * Return number of tips stored in this object
	 * \return number of tips
	 */
	int Count() const;
private:
	wxString TipAt(int at); 
};

typedef SmartPtr<clCallTip> clCallTipPtr;
#endif // CODELITE_CALLTIP_H
