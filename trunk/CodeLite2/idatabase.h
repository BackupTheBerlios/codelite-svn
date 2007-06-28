#ifndef IDATABSE_H
#define IDATABSE_H

#include "cltoken.h"

/**
 * \ingroup CodeLite2
 * Defines the database API required by CodeLite2 library
 */
class IDatabase 
{
public:
	/**
	 * return true if a token with 'name' exists in database
	 */
	virtual bool IsTokenExist(const wxString &name) const = 0;

	/**
	 * add token to the database
	 */
	virtual void AddToken(clTokenPtr token) = 0;
};
#endif // IDATABSE_H
