#ifndef DATABASE_IMPL_H
#define DATABASE_IMPL_H

#include "idatabase.h"
#include "wx/filename.h"

class wxSQLite3Database;

/**
 * \ingroup CodeLite2
 * SQLite3 database implementation of IDatabase iterface
 */
class DatabaseImpl : public IDatabase 
{
	wxSQLite3Database *m_db;
private:
	void CreateSchema();

public:
	DatabaseImpl();
	virtual ~DatabaseImpl();

	//------------------------------------
	// API
	//------------------------------------

	/**
	 * open database
	 * \param name database file name
	 * \return true on success, false othewise
	 */
	bool OpenDatabase(const wxFileName &name);

	/**
	 * return true if a token with 'name' exists in database
	 */
	bool IsTokenExist(const wxString &name) const;

	/**
	 * add token to the database
	 */
	void AddToken(clTokenPtr token);
};
#endif //DATABASE_IMPL_H
