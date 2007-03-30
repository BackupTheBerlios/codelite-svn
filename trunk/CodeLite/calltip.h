#ifndef CODELITE_CALLTIP_H
#define CODELITE_CALLTIP_H

#include "tokenizer.h"
#include "smart_ptr.h"

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
class CallTip
{
	std::vector<wxString> m_tips;
	std::vector<wxString>::size_type m_curr;

public:
	/**
	 * Constructor
	 * \param tips input tips 
	 */
	CallTip(const std::vector<wxString> & tips );

	/**
	 * default constructor
	 */
	CallTip() {}

	/**
	 * Copy constructor
	 */
	CallTip(const CallTip& rhs);

	/**
	 * Assignment operator
	 * \param rhs right hand side
	 * \return this
	 */
	CallTip& operator=(const CallTip& rhs);

	/**
	 * Destructor
	 * \return 
	 */
	virtual ~CallTip(){}

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
	 * Return number of tips stored in this object
	 * \return number of tips
	 */
	std::vector<wxString>::size_type Count() const;
};

typedef SmartPtr<CallTip> CallTipPtr;
#endif // CODELITE_CALLTIP_H
